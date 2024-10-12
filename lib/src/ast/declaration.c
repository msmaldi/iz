#include "ast/declaration.h"
#include "ast/statement.h"
#include "ast/type.h"

#include "common/mem.h"

struct function_t
{
    type_t return_type;
    span_t name;
    array_t(declaration_t) argument_s;
    statement_t statement;
    type_t type;
};

struct argument_t
{
    type_t type;
    span_t name;
};

struct declaration_t
{
    declaration_kind_t kind;
    union
    {
        struct function_t function;
        struct argument_t argument;
    };
};

static
declaration_t declaration_new(declaration_kind_t kind)
{
    declaration_t declaration = mem_alloc(sizeof(struct declaration_t));
    declaration->kind = kind;
    return declaration;
}

static
type_t prepare_callable(type_t return_type, array_t(declaration_t) argument_s)
{
    size_t size = array_size(argument_s);
    array_t(type_t) param_s = array_empty();
    for (size_t i = 0; i < size; i++)
        param_s = array_add(param_s, argument_s[i]->argument.type);

    return type_callable_new(return_type, param_s);
}

declaration_t function_new(type_t return_type, span_t name, array_t(declaration_t) argument_s, statement_t statement)
{
    declaration_t declaration = declaration_new(DECLARATION_FUNCTION);
    function_t function = FUNCTION(declaration);
    function->return_type = return_type;
    function->name = name;
    function->argument_s = argument_s;
    function->statement = statement;
    function->type = prepare_callable(return_type, argument_s);
    return declaration;
}

declaration_t argument_new(type_t type, span_t name)
{
    declaration_t declaration = declaration_new(DECLARATION_ARGUMENT);
    argument_t argument = ARGUMENT(declaration);
    argument->type = type;
    argument->name = name;

    return declaration;
}

static
void function_free(function_t function)
{
    type_free(function->type);
    statement_free(function->statement);
    array_cleanup_free(function->argument_s, (release_t)declaration_free);
}

static
void argument_free(argument_t argument)
{
}

void declaration_free(declaration_t declaration)
{
    switch (declaration->kind) // LCOV_EXCL_LINE
    {
    case DECLARATION_FUNCTION:
        function_free(FUNCTION(declaration));
        break;
    case DECLARATION_ARGUMENT:
        argument_free(ARGUMENT(declaration));
        break;
    default: __builtin_unreachable(); break;
    }
    mem_free(declaration);
}

declaration_kind_t declaration_kind(declaration_t declaration)
{
    return declaration->kind;
}

span_t declaration_name(declaration_t declaration)
{
    switch (declaration->kind) // LCOV_EXCL_LINE
    {
    case DECLARATION_FUNCTION:
        return FUNCTION(declaration)->name;
    case DECLARATION_ARGUMENT:
        return ARGUMENT(declaration)->name;
    default: __builtin_unreachable();
        return span_ctor(0, NULL);
    }
}

type_t function_type(function_t function)
{
    return function->type;
}

type_t function_return_type(function_t function)
{
    return function->return_type;
}

span_t function_name(function_t function)
{
    return function->name;
}

array_t(declaration_t) function_argument_s(function_t function)
{
    return function->argument_s;
}

statement_t function_statement(function_t function)
{
    return function->statement;
}

type_t argument_type(argument_t argument)
{
    return argument->type;
}

span_t argument_name(argument_t argument)
{
    return argument->name;
}

function_t FUNCTION(declaration_t declaration)
{
    return &declaration->function;
}

argument_t ARGUMENT(declaration_t declaration)
{
    return &declaration->argument;
}