#include "sema/sema.h"
#include "sema/scope.h"
#include "common/mem.h"

struct sema_t
{
    array_t(unit_t) unit_s;
    scope_t scope;
    size_t errors;
};

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

static
void enter_scope(sema_t sema)
{
    sema->scope = scope_new(sema->scope);
}

static
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
            sema->errors++;
    }
}

void resolve_identifier(sema_t sema, identifier_t identifier)
{
    span_t name = identifier_name(identifier);

    declaration_t declaration = scope_find(sema->scope, name);
    if (declaration == NULL)
    {
        sema->errors++;
        return;
    }

    identifier_set_declaration(identifier, declaration);
}

void expression_analysis(sema_t sema, expression_t expression)
{
    switch (expression_kind(expression)) // LCOV_EXCL_LINE
    {
    case EXPRESSION_IDENTIFIER:
        resolve_identifier(sema, IDENTIFIER(expression));
        break;
    case EXPRESSION_CONSTANT:
        break;
    }
}

void statement_analysis(sema_t sema, statement_t statement)
{
    switch (statement_kind(statement)) // LCOV_EXCL_LINE
    {
    case STATEMENT_RETURN:
        expression_analysis(sema, return_expression(RETURN(statement)));
        break;
    default: __builtin_unreachable();
        break;
    }
}

void function_analysis(sema_t sema, function_t function)
{
    enter_scope(sema);

    array_t(declaration_t) argument_s = function_argument_s(function);
    size_t size = array_size(argument_s);
    for (size_t i = 0; i < size; i++)
    {
        declaration_t argument = argument_s[i];
        if (!scope_add(sema->scope, argument))
            sema->errors++;
    }

    statement_t statement = function_statement(function);
    statement_analysis(sema, statement);

    leave_scope(sema);
}

void unit_analysis(sema_t sema, unit_t unit)
{
    size_t size = array_size(unit_declaration_s(unit));
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        if (declaration_kind(declaration) == DECLARATION_FUNCTION) // LCOV_EXCL_LINE
            function_analysis(sema, FUNCTION(declaration));
    }
}

void sema_analysis(sema_t sema)
{
    size_t size = array_size(sema->unit_s);
    for (int i = 0; i < size; i++)
    {
        unit_t unit = sema->unit_s[i];
        unit_map_functions(sema, unit);
    }

    for (int i = 0; i < size; i++)
    {
        unit_t unit = sema->unit_s[i];
        unit_analysis(sema, unit);
    }
}
