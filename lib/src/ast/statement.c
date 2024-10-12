#include "ast/statement.h"
#include "common/mem.h"

struct return_t
{
    expression_t expression;
};

struct statement_t
{
    statement_kind_t kind;
    union
    {
        struct return_t ret;
    };
};

static
statement_t statement_new(statement_kind_t kind)
{
    statement_t statement = mem_alloc(sizeof(*statement));
    statement->kind = kind;
    return statement;
}

statement_t return_new(expression_t expression)
{
    statement_t statement = statement_new(STATEMENT_RETURN);

    return_t ret = RETURN(statement);
    ret->expression = expression;

    return statement;
}

void statement_free(statement_t statement)
{
    switch (statement->kind) // LCOV_EXCL_LINE
    {
    case STATEMENT_RETURN:
        expression_free(RETURN(statement)->expression);
        break;
    default: __builtin_unreachable();
        break;
    }
    mem_free(statement);
}

statement_kind_t statement_kind(statement_t statement)
{
    return statement->kind;
}

expression_t return_expression(return_t ret)
{
    return ret->expression;
}

return_t RETURN(statement_t statement)
{
    return &statement->ret;
}