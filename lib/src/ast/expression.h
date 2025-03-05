#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "ast/ast.h"

enum expression_kind_t
{
    EXPRESSION_CONSTANT,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_BINARY,
    EXPRESSION_CALL,
    EXPRESSION_ASSIGNMENT,
    EXPRESSION_IMPLICIT_CAST,
    EXPRESSION_CONDITIONAL
};

enum constant_kind_t
{
    CONSTANT_BOOL,
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
    BINARY_REM
};

enum implicit_cast_kind_t
{
    IMPLICIT_CAST_LVALUE_TO_RVALUE,
    IMPLICIT_CAST_FUNCTION_TO_POINTER_DECAY,
};

enum conditional_kind_t
{
    CONDITIONAL_AND,
    CONDITIONAL_OR
};

expression_t     constant_bool_new(bool _bool);
expression_t     constant_u64_new(uint64_t u64);
expression_t     identifier_new(span_t name);
expression_t     binary_new(expression_t lhs, binary_kind_t op, expression_t rhs);
expression_t     call_new(expression_t callee, array_t(expression_t) argument_s);
expression_t     assignment_new(expression_t lvalue, expression_t rvalue);
expression_t     implicit_cast_new(implicit_cast_kind_t kind, expression_t expression);
expression_t     conditional_new(expression_t lhs, conditional_kind_t op, expression_t rhs);
void             expression_free(expression_t expression);

expression_kind_t expression_kind(expression_t expression);
type_t            expression_type(expression_t expression);

constant_kind_t  constant_kind(constant_t constant);
uint64_t         constant_u64(constant_t constant);
bool             constant_bool(constant_t constant);

span_t         identifier_name(identifier_t identifier);
declaration_t  identifier_declaration(identifier_t identifier);
void           identifier_set_declaration(identifier_t identifier, declaration_t declaration);

expression_t   binary_lhs(binary_t binary);
void           binary_set_lhs(binary_t binary, expression_t lhs);
expression_t   binary_rhs(binary_t binary);
void           binary_set_rhs(binary_t binary, expression_t rhs);

binary_kind_t  binary_op(binary_t binary);

expression_t           call_callee(call_t call);
array_t(expression_t)  call_argument_s(call_t call);

expression_t   assignment_lvalue(assignment_t assignment);
expression_t   assignment_rvalue(assignment_t assignment);
void           assignment_set_rvalue(assignment_t assignment, expression_t rvalue);

type_t               implicit_cast_type(implicit_cast_t implicit_cast);
implicit_cast_kind_t implicit_cast_kind(implicit_cast_t implicit_cast);
expression_t         implicit_cast_expression(implicit_cast_t implicit_cast);

expression_t   conditional_lhs(conditional_t conditional);
void           conditional_set_lhs(conditional_t conditional, expression_t lhs);
expression_t   conditional_rhs(conditional_t conditional);
void           conditional_set_rhs(conditional_t conditional, expression_t rhs);
conditional_kind_t conditional_op(conditional_t conditional);

constant_t      CONSTANT(expression_t expression);
identifier_t    IDENTIFIER(expression_t expression);
binary_t        BINARY(expression_t expression);
call_t          CALL(expression_t expression);
assignment_t    ASSIGNMENT(expression_t expression);
implicit_cast_t IMPLICIT_CAST(expression_t expression);
conditional_t   CONDITIONAL(expression_t expression);

const char* binary_kind_string(binary_kind_t kind);

#endif
