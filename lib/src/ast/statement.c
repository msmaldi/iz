#include "ast/statement.h"
#include "common/mem.h"

struct block_t
{
    array_t(statement_t) statement_s;
};

struct return_t
{
    expression_t expression;
};

struct if_t
{
    expression_t condition;
    statement_t then_branch;
    statement_t else_branch;
};

struct statement_t
{
    statement_kind_t kind;
    union
    {
        struct return_t  ret;
        struct block_t   block;
        struct if_t      ifelse;
    };
};

static
statement_t statement_new(statement_kind_t kind)
{
    statement_t statement = mem_alloc(sizeof(*statement));
    statement->kind = kind;
    return statement;
}

statement_t block_new(array_t(statement_t) statement_s)
{
    statement_t statement = statement_new(STATEMENT_BLOCK);

    block_t block = BLOCK(statement);
    block->statement_s = statement_s;

    return statement;
}

statement_t return_new(expression_t expression)
{
    statement_t statement = statement_new(STATEMENT_RETURN);

    return_t ret = RETURN(statement);
    ret->expression = expression;

    return statement;
}

statement_t if_new(expression_t condition, statement_t then_branch, statement_t else_branch)
{
    statement_t statement = statement_new(STATEMENT_IF);

    if_t ifelse = IF(statement);
    ifelse->condition = condition;
    ifelse->then_branch = then_branch;
    ifelse->else_branch = else_branch;

    return statement;
}

static
void block_free(block_t block)
{
    array_cleanup_free(block->statement_s, (release_t)statement_free);
}

static
void if_free(if_t ifelse)
{
    expression_free(ifelse->condition);
    statement_free(ifelse->then_branch);
    if (ifelse->else_branch)
        statement_free(ifelse->else_branch);
}

void statement_free(statement_t statement)
{
    switch (statement->kind) // LCOV_EXCL_LINE
    {
        case STATEMENT_BLOCK:
            block_free(BLOCK(statement));
            break;
        case STATEMENT_RETURN:
            expression_free(RETURN(statement)->expression);
            break;
        case STATEMENT_IF:
            if_free(IF(statement));
            break;
    }
    mem_free(statement);
}

statement_kind_t statement_kind(statement_t statement)
{
    return statement->kind;
}

static
bool block_all_path_return_value(block_t block)
{
    array_t(statement_t) statement_s = block->statement_s;
    size_t size = array_size(statement_s);

    for (size_t i = 0; i < size; i++)
    {
        statement_t statement = statement_s[i];
        if (statement_all_path_return_value(statement))
            return true;
    }

    return false;
}

static
bool return_all_path_return_value(return_t ret)
{
    (void)ret;
    return true;
}

static
bool if_all_path_return_value(if_t ifelse)
{
    if (ifelse->else_branch == NULL)
        return false;

    return statement_all_path_return_value(ifelse->then_branch) &&
           statement_all_path_return_value(ifelse->else_branch);
}

bool statement_all_path_return_value(statement_t statement)
{
    switch (statement->kind) // LCOV_EXCL_LINE
    {
        case STATEMENT_BLOCK:
            return block_all_path_return_value(BLOCK(statement));
        case STATEMENT_RETURN:
            return return_all_path_return_value(RETURN(statement));
        case STATEMENT_IF:
            return if_all_path_return_value(IF(statement));
    }
    return NULL;
}

array_t(statement_t) block_statement_s(block_t block)
{
    return block->statement_s;
}

expression_t return_expression(return_t ret)
{
    return ret->expression;
}

expression_t if_condition(if_t ifelse)
{
    return ifelse->condition;
}

statement_t if_then_branch(if_t ifelse)
{
    return ifelse->then_branch;
}

statement_t if_else_branch(if_t ifelse)
{
    return ifelse->else_branch;
}

block_t BLOCK(statement_t statement)
{
    return &statement->block;
}

return_t RETURN(statement_t statement)
{
    return &statement->ret;
}

if_t IF(statement_t statement)
{
    return &statement->ifelse;
}
