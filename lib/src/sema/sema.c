
#include "sema/sema.h"
#include "sema/scope.h"
#include "common/mem.h"

#include <stdio.h>

typedef  struct sema_t*  sema_t;

sema_t sema_new(array_t(unit_t) unit_s);
void   sema_free(sema_t sema);

size_t sema_errors(sema_t sema);
void   sema_analysis(sema_t sema);

static
void display_error(source_t source, span_t span, const char *msg);

struct sema_t
{
    array_t(unit_t) unit_s;
    scope_t scope;

    unit_t unit;
    declaration_t declaration;
    function_t function;

    size_t errors;
};

compilation_t semantic_analysis(array_t(unit_t) unit_s)
{
    sema_t sema = sema_new(unit_s);
    sema_analysis(sema);

    size_t errors = sema_errors(sema);
    if (errors > 0)
    {
        fprintf(stderr, "compilation failed with %zu errors\n", errors);
        array_cleanup_free(unit_s, (release_t)unit_free);
        sema_free(sema);
        return NULL;
    }

    compilation_t compilation_unit = compilation_new(unit_s);
    sema_free(sema);

    return compilation_unit;
}

sema_t sema_new(array_t(unit_t) unit_s)
{
    sema_t sema = mem_alloc(sizeof(struct sema_t));

    sema->unit_s = unit_s;
    sema->scope = scope_new(NULL);
    sema->errors = 0;

    return sema;
}

void sema_free(sema_t sema)
{
    scope_free(sema->scope);
    mem_free(sema);
}

size_t sema_errors(sema_t sema)
{
    return sema->errors;
}

static inline
void enter_scope(sema_t sema)
{
    sema->scope = scope_new(sema->scope);
}

static inline
void leave_scope(sema_t sema)
{
    scope_t parent = scope_parent(sema->scope);
    scope_free(sema->scope);
    sema->scope = parent;
}

void unit_map_functions(sema_t sema, unit_t unit)
{
    size_t size = array_size(unit_declaration_s(unit));
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        if (!scope_add(sema->scope, declaration))
        {
            display_error(unit_source(unit), declaration_name(declaration), "redefinition of");
            sema->errors++;
        }
    }
}

void resolve_identifier(sema_t sema, identifier_t identifier)
{
    span_t name = identifier_name(identifier);

    declaration_t declaration = scope_find(sema->scope, name);
    if (declaration == NULL)
    {
        display_error(unit_source(sema->unit), name, "undeclared identifier");

        sema->errors++;
        return;
    }

    identifier_set_declaration(identifier, declaration);
}

expression_t implicit_cast(sema_t sema, expression_t expression, type_t expected_type)
{
    expression_kind_t kind = expression_kind(expression);

    if (kind == EXPRESSION_IDENTIFIER)
    {
        declaration_t declaration = identifier_declaration(IDENTIFIER(expression));
        type_t identifier_type = declaration_type(declaration);
        if (type_eq(expected_type, identifier_type))
        {
            return implicit_cast_new(IMPLICIT_CAST_LVALUE_TO_RVALUE, expression);
        }

        display_error(unit_source(sema->unit), declaration_name(declaration), "incompatible type");
        sema->errors++;
    }

    return expression;
}

void expression_analysis(sema_t sema, expression_t expression);

static
void constant_analysis(sema_t sema, constant_t constant)
{
    switch (constant_kind(constant)) // LCOV_EXCL_LINE
    {
        case CONSTANT_U64:
            break;
    }
}

static
void binary_analysis(sema_t sema, binary_t binary)
{
    expression_t lhs = binary_lhs(binary);
    expression_analysis(sema, lhs);

    type_t type_lhs = expression_type(lhs);
    binary_set_lhs(binary, implicit_cast(sema, lhs, type_lhs));

    expression_t rhs = binary_rhs(binary);
    expression_analysis(sema, rhs);

    type_t type_rhs = expression_type(rhs);
    binary_set_rhs(binary, implicit_cast(sema, rhs, type_rhs));
}

static
void call_analysis(sema_t sema, call_t call)
{
    expression_t callee = call_callee(call);
    expression_analysis(sema, callee);

    array_t(expression_t) argument_s = call_argument_s(call);
    size_t size = array_size(argument_s);
    for (int i = 0; i < size; i++)
    {
        expression_t argument = argument_s[i];
        expression_analysis(sema, argument);
    }
}

static
void assignment_analysis(sema_t sema, assignment_t assignment)
{
    expression_t lhs = assignment_lvalue(assignment);
    expression_analysis(sema, lhs);

    expression_t rhs = assignment_rvalue(assignment);
    expression_analysis(sema, rhs);
    type_t type_rhs = expression_type(rhs);
    assignment_set_rvalue(assignment, implicit_cast(sema, rhs, type_rhs));
}

void expression_analysis(sema_t sema, expression_t expression)
{
    switch (expression_kind(expression)) // LCOV_EXCL_LINE
    {
        case EXPRESSION_CONSTANT:
            constant_analysis(sema, CONSTANT(expression));
            break;
        case EXPRESSION_IDENTIFIER:
            resolve_identifier(sema, IDENTIFIER(expression));
            break;
        case EXPRESSION_BINARY:
            binary_analysis(sema, BINARY(expression));
            break;
        case EXPRESSION_CALL:
            call_analysis(sema, CALL(expression));
            break;
        case EXPRESSION_ASSIGNMENT:
            assignment_analysis(sema, ASSIGNMENT(expression));
            break;
        case EXPRESSION_IMPLICIT_CAST:
            // TODO: implement
            break;
    }
}

void statement_analysis(sema_t sema, statement_t statement);

static
void block_analysis(sema_t sema, block_t block)
{
    array_t(statement_t) statement_s = block_statement_s(block);
    size_t size = array_size(statement_s);
    for (int i = 0; i < size; i++)
    {
        statement_t statement = statement_s[i];
        statement_analysis(sema, statement);
    }
}

static
void return_analysis(sema_t sema, return_t ret)
{
    expression_t expression = return_expression(ret);
    expression_analysis(sema, expression);

    type_t expected = function_return_type(sema->function);
    type_t actual = expression_type(expression);

    bool eq = type_eq(expected, actual);
    if (!eq)
    {
        display_error(unit_source(sema->unit), function_name(sema->function), "return type mismatch");
        sema->errors++;
        return;
    }

    return_set_expression(ret, implicit_cast(sema, expression, expected));
}

static
void if_analysis(sema_t sema, if_t ifelse)
{
    expression_analysis(sema, if_condition(ifelse));

    statement_analysis(sema, if_then_branch(ifelse));

    statement_t else_branch = if_else_branch(ifelse);
    if (else_branch != NULL)
        statement_analysis(sema, else_branch);
}

static
void var_analysis(sema_t sema, var_t var)
{
    array_t(declaration_t) declaration_s = var_variable_s(var);
    size_t size = array_size(declaration_s);

    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = declaration_s[i];
        if (!scope_add(sema->scope, declaration))
        {
            display_error(unit_source(sema->unit), declaration_name(declaration), "redefinition of");
            sema->errors++;
        }

        variable_t variable = VARIABLE(declaration);
        expression_t initializer = variable_initializer(variable);
        if (initializer != NULL)
            expression_analysis(sema, initializer);
    }
}

static
void act_analysis(sema_t sema, act_t act)
{
    array_t(expression_t) expression_s = act_expression_s(act);
    size_t size = array_size(expression_s);
    for (int i = 0; i < size; i++)
    {
        expression_t expression = expression_s[i];
        expression_analysis(sema, expression);
    }
}

void statement_analysis(sema_t sema, statement_t statement)
{
    switch (statement_kind(statement)) // LCOV_EXCL_LINE
    {
        case STATEMENT_BLOCK:
            block_analysis(sema, BLOCK(statement));
            break;
        case STATEMENT_RETURN:
            return_analysis(sema, RETURN(statement));
            break;
        case STATEMENT_IF:
            if_analysis(sema, IF(statement));
            break;
        case STATEMENT_VAR:
            var_analysis(sema, VAR(statement));
            break;
        case STATEMENT_ACT:
            act_analysis(sema, ACT(statement));
            break;
    }
}

static
void map_argument_s(sema_t sema, array_t(declaration_t) argument_s)
{
    size_t size = array_size(argument_s);
    for (size_t i = 0; i < size; i++)
    {
        declaration_t argument = argument_s[i];
        if (!scope_add(sema->scope, argument))
        {
            display_error(unit_source(sema->unit), declaration_name(argument), "redefinition of");
            sema->errors++;
        }
    }
}

void function_analysis(sema_t sema, function_t function)
{
    enter_scope(sema);

    scope_add(sema->scope, (declaration_t)function);

    array_t(declaration_t) argument_s = function_argument_s(function);
    map_argument_s(sema, argument_s);

    statement_t statement = function_statement(function);
    statement_analysis(sema, statement);

    if (!statement_all_path_return_value(statement))
    {
        display_error(unit_source(sema->unit), function_name(function), "missing return statement");
        sema->errors++;
    }

    leave_scope(sema);
}

void unit_analysis(sema_t sema, unit_t unit)
{
    size_t size = array_size(unit_declaration_s(unit));
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        sema->declaration = declaration;
        if (declaration_kind(declaration) == DECLARATION_FUNCTION) // LCOV_EXCL_LINE
        {
            function_t function = FUNCTION(declaration);
            sema->function = function;

            function_analysis(sema, function);
            sema->function = NULL;
        }
        sema->declaration = NULL;
    }
}

void sema_analysis(sema_t sema)
{
    size_t size = array_size(sema->unit_s);
    for (int i = 0; i < size; i++)
    {
       sema->unit = sema->unit_s[i];
       unit_map_functions(sema, sema->unit);
       sema->unit = NULL;
    }

    for (int i = 0; i < size; i++)
    {
        sema->unit = sema->unit_s[i];
        unit_analysis(sema, sema->unit);
        sema->unit = NULL;
    }
}

#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

struct diagnostic_t
{
    char* line_start;
    int   line_size;
    int   line;
    int   column;
};

static
int get_line_size(char* line_start)
{
    int size = 0;
    while (line_start[size] != '\n' && line_start[size] != '\0')
        size++;

    return size;
}

static
void fill_diagnostic(struct diagnostic_t *diagnostic, char *code, span_t span)
{
    char *start = code;
    char *end = (char *)span.data;

    diagnostic->line_start = start;
    diagnostic->line = 0;
    diagnostic->column = 0;

    while (start < end)
    {
        char c = *start++;
        if (c != '\n')
        {
            diagnostic->column++;
        }
        else
        {
            diagnostic->line++;
            diagnostic->column = 0;
            diagnostic->line_start = start;
        }

    }

    diagnostic->line_size = get_line_size(diagnostic->line_start);
}

static
void display_error(source_t source, span_t span, const char *msg)
{
    struct diagnostic_t diagnostic;
    fill_diagnostic(&diagnostic, (char *)source_code(source), span);

    fprintf(stderr, BOLDWHITE "%s:%d:%d ", source_path(source), diagnostic.line + 1, diagnostic.column + 1);
    fprintf(stderr, BOLDRED "error: " BOLDWHITE "%s '%.*s'\n" RESET, msg, span.size, span.data);

    char line_counter[20] = { 0 };
    int line_counter_len = snprintf(line_counter, sizeof(line_counter), "%5u | ", diagnostic.line + 1);
    fprintf(stderr, "%s", line_counter);

    int line_size = get_line_size(diagnostic.line_start);
    fprintf(stderr, "%.*s\n", line_size, diagnostic.line_start);

    int size = line_counter_len + diagnostic.column;
    for (size_t i = 0; i < size; i++)
        fputc(' ', stderr);

    fprintf(stderr, BOLDGREEN);
    for (size_t i = 0; i < span.size; i++)
        fputc('^', stderr);
    fprintf(stderr, "\n" RESET);
}