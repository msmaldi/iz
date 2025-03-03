#include "ast/ast_print.h"


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

static
void tab_print(int tab, FILE* out)
{
    while (tab--)
        fprintf(out, "  ");
}

static
void unit_print(unit_t unit, FILE* out, int tab);

static
void declaration_print(declaration_t declaration, FILE* out, int tab);

static
void function_print(function_t function, FILE* out, int tab);

static
void argument_print(argument_t argument, FILE* out, int tab);

static
void variable_print(variable_t argument, FILE* out, int tab);

static
void type_print(type_t type, FILE* out);

static
void callable_print(callable_t callable, FILE* out);

static
void statement_print(statement_t statement, FILE* out, int tab);

static
void block_print(block_t block, FILE* out, int tab);

static
void return_print(return_t ret, FILE* out, int tab);

static
void if_print(if_t ifelse, FILE* out, int tab);

static
void var_print(var_t var, FILE* out, int tab);

static
void expression_print(expression_t expression, FILE* out, int tab);

static
void constant_print(constant_t constant, FILE* out, int tab);

static
void implicit_cast_print(implicit_cast_t cast, FILE* out, int tab);

static
void indentifier_print(identifier_t identifier, FILE* out, int tab);

static
void binary_print(binary_t binary, FILE* out, int tab);

static
void conditional_print(conditional_t conditional, FILE* out, int tab);

static
void call_print(call_t call, FILE* out, int tab);

void compilation_print(compilation_t compilation, FILE* out)
{
    array_t(unit_t) unit_s = compilation_unit_s(compilation);
    size_t size = array_size(unit_s);

    fprintf(out, BOLDMAGENTA "%s" BOLDYELLOW  " %p" WHITE "   %zu\n" RESET, "compilation_t", compilation, size);
    for (int i = 0; i < size; i++)
    {
        unit_t unit = unit_s[i];
        unit_print(unit, out, 1);
    }
}

static
void unit_print(unit_t unit, FILE* out, int tab)
{
    tab_print(tab, out);
    source_t source = unit_source(unit);

    array_t(declaration_t) declaration_s = unit_declaration_s(unit);
    size_t size = array_size(declaration_s);

    const char* format = BOLDMAGENTA "%s" BOLDYELLOW " %p" BLUE " %s" WHITE "    %zu\n" RESET;
    fprintf(out, format, "unit_t", unit, source_path(source), size);

    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = declaration_s[i];
        declaration_print(declaration, out, tab + 1);
    }
}

static
void declaration_print(declaration_t declaration, FILE* out, int tab)
{
    switch (declaration_kind(declaration))
    {
        case DECLARATION_FUNCTION:
            function_print(FUNCTION(declaration), out, tab);
            break;
        case DECLARATION_ARGUMENT:
            argument_print(ARGUMENT(declaration), out, tab);
            break;
        case DECLARATION_VARIABLE:
            variable_print(VARIABLE(declaration), out, tab);
            break;
    }
}

static
void function_print(function_t function, FILE* out, int tab)
{
    tab_print(tab, out);
    const char *format = BOLDMAGENTA "%s" BOLDYELLOW " %p" GREEN " %.*s " RESET;
    fprintf(out, format, "function_t", function, function_name(function).size, function_name(function).data);

    array_t(declaration_t) argument_s = function_argument_s(function);
    size_t size = array_size(argument_s);

    fprintf(out, "'");

    type_t ret_type = function_return_type(function);
    type_print(ret_type, out);

    fprintf(out, " (");

    for (int i = 0, last = size - 1; i < size; i++)
    {
        declaration_t argument = argument_s[i];
        type_t type = declaration_type(argument);
        type_print(type, out);

        if (i != last)
            fprintf(out, ", ");
    }
    fprintf(out, ")'\n");

    for (int i = 0; i < size; i++)
    {
        declaration_t argument = argument_s[i];
        declaration_print(argument, out, tab + 1);
    }

    statement_print(function_statement(function), out, tab + 1);

    fprintf(out, "\n");
}

static
void argument_print(argument_t argument, FILE* out, int tab)
{
    const char *format = BOLDMAGENTA "    %s" BOLDYELLOW " %p" GREEN " %.*s" RESET;
    fprintf(out, format, "argument_t", argument, argument_name(argument).size, argument_name(argument).data);

    type_t type = argument_type(argument);
    fprintf(out, " '");
    type_print(type, out);
    fprintf(out, "'\n");
}

static
void variable_print(variable_t variable, FILE* out, int tab)
{
    tab_print(tab, out);
    const char *format = BOLDMAGENTA "%s" BOLDYELLOW " %p" GREEN " %.*s" RESET;
    fprintf(out, format, "variable_t", variable, variable_name(variable).size, variable_name(variable).data);

    type_t type = variable_type(variable);
    fprintf(out, " '");
    type_print(type, out);
    fprintf(out, "'\n");
}

static
void type_print(type_t type, FILE* out)
{
    switch (type_kind(type))
    {
    case TYPE_INT:
        fprintf(out, CYAN "int" RESET);
        break;
    case TYPE_BOOL:
        fprintf(out, CYAN "bool" RESET);
        break;
    case TYPE_CALLABLE:
        callable_print(CALLABLE(type), out);
        break;
    default:
        break;
    }
}

static
void callable_print(callable_t callable, FILE* out)
{
    type_t return_type = callable_return_type(callable);
    array_t(type_t) param_s = callable_param_s(callable);

    type_print(return_type, out);
    fprintf(out, " (");
    for (int i = 0, last = array_size(param_s) - 1; i < array_size(param_s); i++)
    {
        type_print(param_s[i], out);
        if (i != last)
            fprintf(out, ", ");
    }
    fprintf(out, ")");
}

static
void statement_print(statement_t statement, FILE* out, int tab)
{
    switch (statement_kind(statement))
    {
    case STATEMENT_RETURN:
        return_print(RETURN(statement), out, tab);
        break;
    case STATEMENT_ACT:
        break;
    case STATEMENT_VAR:
        var_print(VAR(statement), out, tab);
        break;
    case STATEMENT_BLOCK:
        block_print(BLOCK(statement), out, tab);
        break;
    case STATEMENT_IF:
        if_print(IF(statement), out, tab);
        break;
    default:
        break;
    }
}

static
void block_print(block_t block, FILE* out, int tab)
{
    tab_print(tab, out);

    const char *format = BOLDCYAN "%s" BOLDYELLOW " %p\n" RESET;
    fprintf(out, format, "block_t", block);

    array_t(statement_t) statement_s = block_statement_s(block);
    size_t size = array_size(statement_s);

    for (int i = 0; i < size; i++)
    {
        statement_t statement = statement_s[i];
        statement_print(statement, out, tab + 1);
    }
}

static
void return_print(return_t ret, FILE* out, int tab)
{
    tab_print(tab, out);

    const char *format = BOLDCYAN "%s" BOLDYELLOW " %p\n" RESET;
    fprintf(out, format, "return_t", ret);

    expression_print(return_expression(ret), out, tab + 1);
}

static
void if_print(if_t ifelse, FILE* out, int tab)
{
    tab_print(tab, out);

    bool has_else = if_else_branch(ifelse) != NULL;
    const char *has_else_str = has_else ? "has_else" : "";
    const char *format = BOLDCYAN "%s" BOLDYELLOW " %p" RESET " %s\n";
    fprintf(out, format, "if_t", ifelse, has_else_str);


    expression_print(if_condition(ifelse), out, tab + 1);
    statement_print(if_then_branch(ifelse), out, tab + 1);

    if (has_else)
        statement_print(if_else_branch(ifelse), out, tab + 1);
}

static
void var_print(var_t var, FILE* out, int tab)
{
    tab_print(tab, out);

    const char *format = BOLDCYAN "%s" BOLDYELLOW " %p\n" RESET;
    fprintf(out, format, "var_t", var);

    array_t(declaration_t) variable_s = var_variable_s(var);
    size_t size = array_size(variable_s);

    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = variable_s[i];
        declaration_print(declaration, out, tab + 1);

        variable_t variable = VARIABLE(declaration);
        expression_t initializer = variable_initializer(variable);
        if (initializer != NULL)
            expression_print(initializer, out, tab + 2);
    }
}

static
void expression_print(expression_t expression, FILE* out, int tab)
{
    switch (expression_kind(expression))
    {
        case EXPRESSION_CONSTANT:
            constant_print(CONSTANT(expression), out, tab);
            break;
        case EXPRESSION_IMPLICIT_CAST:
            implicit_cast_print(IMPLICIT_CAST(expression), out, tab);
            break;
        case EXPRESSION_IDENTIFIER:
            indentifier_print(IDENTIFIER(expression), out, tab);
            break;
        case EXPRESSION_BINARY:
            binary_print(BINARY(expression), out, tab);
            break;
        case EXPRESSION_CALL:
            call_print(CALL(expression), out, tab);
            break;
        case EXPRESSION_CONDITIONAL:
            conditional_print(CONDITIONAL(expression), out, tab);
        default:
            break;
    }
}

static
void constant_print(constant_t constant, FILE* out, int tab)
{
    tab_print(tab, out);

    switch (constant_kind(constant))
    {
        case CONSTANT_BOOL:
        {
            const char *str = constant_bool(constant) ? "true" : "false";
            fprintf(out, BOLDGREEN "constant_t" BOLDYELLOW " %p" RESET " %s\n" RESET, constant, str);
            break;
        }
        case CONSTANT_U64:
            fprintf(out, BOLDGREEN "constant_t" BOLDYELLOW " %p" RESET " %zu\n" RESET, constant, constant_u64(constant));
            break;
    }
}

static
void implicit_cast_print(implicit_cast_t cast, FILE* out, int tab)
{
    tab_print(tab, out);

    const char *format = BOLDGREEN "implicit_cast_t" BOLDYELLOW " %p\n" RESET;
    fprintf(out, format, cast);

    expression_print(implicit_cast_expression(cast), out, tab + 1);
}

static
void indentifier_print(identifier_t identifier, FILE* out, int tab)
{
    tab_print(tab, out);

    span_t name = identifier_name(identifier);

    const char *format = BOLDGREEN "identifier_t" BOLDYELLOW " %p" GREEN " %.*s " RESET;
    fprintf(out, format, identifier, name.size, name.data);

    declaration_t declaration = identifier_declaration(identifier);
    type_t type = declaration_type(declaration);
    fprintf(out, "'");
    type_print(type, out);
    fprintf(out, "' ->");

    fprintf(out, BOLDYELLOW " %p" RESET, declaration);

    fprintf(out, "\n");
}

static
void binary_print(binary_t binary, FILE* out, int tab)
{
    tab_print(tab, out);

    const char *binary_kind_str = binary_kind_string(binary_op(binary));
    const char *format = BOLDGREEN "%s" BOLDYELLOW " %p" WHITE " '" BLUE "%s" WHITE "'" "\n" RESET;
    fprintf(out, format, "binary_t", binary, binary_kind_str);

    expression_print(binary_lhs(binary), out, tab + 1);
    expression_print(binary_rhs(binary), out, tab + 1);
}

static
void conditional_print(conditional_t conditional, FILE* out, int tab)
{
    tab_print(tab, out);
    const char *conditional_kind_str = conditional_op(conditional) == CONDITIONAL_AND ? "&&" : "||";
    const char *format = BOLDGREEN "%s" BOLDYELLOW " %p" WHITE " '" BLUE "%s" WHITE "'" "\n" RESET;
    fprintf(out, format, "conditional_t", conditional, conditional_kind_str);

    expression_print(conditional_lhs(conditional), out, tab + 1);
    expression_print(conditional_rhs(conditional), out, tab + 1);}

static
void call_print(call_t call, FILE* out, int tab)
{
    tab_print(tab, out);

    const char *format = BOLDGREEN "call_t" BOLDYELLOW " %p\n" RESET;
    fprintf(out, format, call);

    expression_print(call_callee(call), out, tab + 1);

    array_t(expression_t) argument_s = call_argument_s(call);
    size_t size = array_size(argument_s);

    for (int i = 0; i < size; i++)
    {
        expression_print(argument_s[i], out, tab + 1);
    }
}
