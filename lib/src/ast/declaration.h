#ifndef _DECLARATION_H_
#define _DECLARATION_H_

#include "ast/ast.h"

enum declaration_kind_t
{
    DECLARATION_FUNCTION,
    DECLARATION_ARGUMENT,
    DECLARATION_VARIABLE,
};

declaration_t function_new(type_t return_type, span_t name, array_t(declaration_t) argument_s, statement_t statement);
declaration_t argument_new(type_t type, span_t name);
declaration_t variable_new(type_t type, span_t name, expression_t initializer);

void          declaration_free(declaration_t declaration);

declaration_kind_t  declaration_kind(declaration_t declaration);
span_t              declaration_name(declaration_t declaration);
type_t              declaration_type(declaration_t declaration);

type_t                  function_type(function_t function);
type_t                  function_return_type(function_t function);
span_t                  function_name(function_t function);
array_t(declaration_t)  function_argument_s(function_t function);
statement_t             function_statement(function_t function);

type_t       argument_type(argument_t argument);
span_t       argument_name(argument_t argument);

type_t          variable_type(variable_t variable);
span_t          variable_name(variable_t variable);
expression_t    variable_initializer(variable_t variable);
void            variable_set_initializer(variable_t variable, expression_t initializer);

function_t FUNCTION(declaration_t declaration);
argument_t ARGUMENT(declaration_t declaration);
variable_t VARIABLE(declaration_t declaration);


#endif
