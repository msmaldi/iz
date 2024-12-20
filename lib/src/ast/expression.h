#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "ast/ast.h"

enum expression_kind_t
{
    EXPRESSION_CONSTANT,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_BINARY,
    EXPRESSION_CALL,
};

enum constant_kind_t
{
    CONSTANT_U64,
};

enum binary_kind_t
{
    BINARY_LT,
    BINARY_LE,
    BINARY_GT,
    BINARY_GE,
    BINARY_EQ,
    BINARY_NE,

    BINARY_ADD,
    BINARY_SUB,

    BINARY_MUL,
    BINARY_DIV,
    BINARY_REM,
};

expression_t     constant_u64_new(uint64_t u64);
expression_t     identifier_new(span_t name);
expression_t     binary_new(expression_t lhs, binary_kind_t op, expression_t rhs);
expression_t     call_new(expression_t callee, array_t(expression_t) argument_s);
void             expression_free(expression_t expression);

expression_kind_t expression_kind(expression_t expression);
type_t            expression_type(expression_t expression);

constant_kind_t  constant_kind(constant_t constant);
uint64_t         constant_u64(constant_t constant);

span_t         identifier_name(identifier_t identifier);
declaration_t  identifier_declaration(identifier_t identifier);
void           identifier_set_declaration(identifier_t identifier, declaration_t declaration);

expression_t   binary_lhs(binary_t binary);
expression_t   binary_rhs(binary_t binary);
binary_kind_t  binary_op(binary_t binary);

expression_t           call_callee(call_t call);
array_t(expression_t)  call_argument_s(call_t call);

constant_t    CONSTANT(expression_t expression);
identifier_t  IDENTIFIER(expression_t expression);
binary_t      BINARY(expression_t expression);
call_t        CALL(expression_t expression);

#endif
