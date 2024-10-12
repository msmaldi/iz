#include "ast/expression.h"
#include "common/span.h"
#include "common/mem.h"

struct constant_t
{
    constant_kind_t kind;
    union
    {
        uint64_t u64;
    };
};

struct identifier_t
{
    span_t name;
    declaration_t declaration;
};


struct expression_t
{
    expression_kind_t kind;
    union
    {
        struct constant_t   constant;
        struct identifier_t identifier;
    };
};

static
expression_t expression_new(expression_kind_t kind)
{
    expression_t expression = mem_alloc(sizeof(struct expression_t));
    expression->kind = kind;
    return expression;
}

expression_t constant_u64_new(uint64_t u64)
{
    expression_t expression = expression_new(EXPRESSION_CONSTANT);

    constant_t constant = CONSTANT(expression);
    constant->kind = CONSTANT_U64;
    constant->u64 = u64;

    return expression;
}

expression_t identifier_new(span_t name)
{
    expression_t expression = expression_new(EXPRESSION_IDENTIFIER);
    identifier_t identifier = IDENTIFIER(expression);
    identifier->name = name;
    identifier->declaration = NULL;
    return expression;
}

void expression_free(expression_t expression)
{
    mem_free(expression);
}

expression_kind_t expression_kind(expression_t expression)
{
    return expression->kind;
}

constant_kind_t constant_kind(constant_t constant)
{
    return constant->kind;
}

uint64_t constant_u64(constant_t constant)
{
    return constant->u64;
}

span_t identifier_name(identifier_t identifier)
{
    return identifier->name;
}

declaration_t identifier_declaration(identifier_t identifier)
{
    return identifier->declaration;
}

void identifier_set_declaration(identifier_t identifier, declaration_t declaration)
{
    identifier->declaration = declaration;
}

constant_t CONSTANT(expression_t expression)
{
    return &expression->constant;
}

identifier_t IDENTIFIER(expression_t expression)
{
    return &expression->identifier;
}