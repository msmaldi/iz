#ifndef _DECLARATION_H_
#define _DECLARATION_H_

#include "ast/ast.h"

enum declaration_kind_t
{
    DECLARATION_FUNCTION,
    DECLARATION_ARGUMENT,
};

declaration_t function_new(type_t return_type, span_t name, array_t(declaration_t) argument_s, statement_t statement);
declaration_t argument_new(type_t type, span_t name);

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

function_t FUNCTION(declaration_t declaration);
argument_t ARGUMENT(declaration_t declaration);


#endif
