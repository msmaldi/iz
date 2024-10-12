#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "ast/ast.h"
#include "common/span.h"

enum expression_kind_t
{
    EXPRESSION_CONSTANT,
    EXPRESSION_IDENTIFIER,
};

enum constant_kind_t
{
    CONSTANT_U64,
};

expression_t     constant_u64_new(uint64_t u64);
expression_t     identifier_new(span_t name);
void             expression_free(expression_t expression);

expression_kind_t expression_kind(expression_t expression);

constant_kind_t  constant_kind(constant_t constant);
uint64_t         constant_u64(constant_t constant);

span_t         identifier_name(identifier_t identifier);
declaration_t  identifier_declaration(identifier_t identifier);
void           identifier_set_declaration(identifier_t identifier, declaration_t declaration);

constant_t    CONSTANT(expression_t expression);
identifier_t  IDENTIFIER(expression_t expression);


#endif
