#ifndef _STATEMENT_H_
#define _STATEMENT_H_

#include "ast/ast.h"

enum statement_kind_t
{
    STATEMENT_BLOCK,
    STATEMENT_RETURN,
    STATEMENT_IF,
    STATEMENT_VAR,
    STATEMENT_ACT,
};

statement_t  block_new(array_t(statement_t) statement_s);
statement_t  return_new(expression_t expression);
statement_t  if_new(expression_t condition, statement_t then_branch, statement_t else_branch);
statement_t  var_new(array_t(declaration_t) variable_s);
statement_t  act_new(array_t(expression_t) expression_s);

void         statement_free(statement_t statement);

statement_kind_t  statement_kind(statement_t statement);
bool              statement_all_path_return_value(statement_t statement);

array_t(statement_t) block_statement_s(block_t block);

expression_t return_expression(return_t ret);
void         return_set_expression(return_t ret, expression_t expression);

expression_t if_condition(if_t ifelse);
statement_t  if_then_branch(if_t ifelse);
statement_t  if_else_branch(if_t ifelse);

array_t(declaration_t) var_variable_s(var_t var);

array_t(expression_t)  act_expression_s(act_t act);

block_t   BLOCK(statement_t statement);
return_t  RETURN(statement_t statement);
if_t      IF(statement_t statement);
var_t     VAR(statement_t statement);
act_t     ACT(statement_t statement);

#endif
