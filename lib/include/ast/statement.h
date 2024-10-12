#ifndef _STATEMENT_H_
#define _STATEMENT_H_

#include "ast/ast.h"

enum statement_kind_t
{
    STATEMENT_RETURN,
};

statement_t  return_new(expression_t expression);
void         statement_free(statement_t statement);

statement_kind_t  statement_kind(statement_t statement);

expression_t return_expression(return_t ret);

return_t     RETURN(statement_t statement);

#endif
