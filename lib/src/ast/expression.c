#include "ast/expression.h"
#include "common/mem.h"

struct constant_t
{
    union
    {
        uint64_t u64;
        bool _bool;
    };
    constant_kind_t kind;
};

struct identifier_t
{
    span_t name;
    declaration_t declaration;
};

struct binary_t
{
    expression_t lhs, rhs;
    binary_kind_t op;
};

struct call_t
{
    expression_t callee;
    array_t(expression_t) argument_s;
};

struct assignment_t
{
    expression_t lvalue;
    expression_t rvalue;
};

struct implicit_cast_t
{
    implicit_cast_kind_t kind;
    expression_t expression;
};

struct conditional_t
{
    expression_t lhs, rhs;
    conditional_kind_t op;
};

struct expression_t
{
    union
    {
        struct constant_t      constant;
        struct identifier_t    identifier;
        struct binary_t        binary;
        struct call_t          call;
        struct assignment_t    assignment;
        struct implicit_cast_t implicit_cast;
        struct conditional_t   conditional;
    };
    expression_kind_t kind;
};

static
expression_t expression_new(expression_kind_t kind)
{
    expression_t expression = mem_alloc(sizeof(struct expression_t));
    expression->kind = kind;
    return expression;
}

expression_t constant_bool_new(bool _bool)
{
    expression_t expression = expression_new(EXPRESSION_CONSTANT);

    constant_t constant = CONSTANT(expression);
    constant->kind = CONSTANT_BOOL;
    constant->_bool = _bool;

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

expression_t binary_new(expression_t lhs, binary_kind_t op, expression_t rhs)
{
    expression_t expression = expression_new(EXPRESSION_BINARY);
    binary_t binary = BINARY(expression);
    binary->lhs = lhs;
    binary->rhs = rhs;
    binary->op = op;
    return expression;
}

expression_t call_new(expression_t callee, array_t(expression_t) argument_s)
{
    expression_t expression = expression_new(EXPRESSION_CALL);
    call_t call = CALL(expression);
    call->callee = callee;
    call->argument_s = argument_s;
    return expression;
}


expression_t assignment_new(expression_t lvalue, expression_t rvalue)
{
    expression_t expression = expression_new(EXPRESSION_ASSIGNMENT);
    assignment_t assignment = ASSIGNMENT(expression);
    assignment->lvalue = lvalue;
    assignment->rvalue = rvalue;
    return expression;
}

expression_t implicit_cast_new(implicit_cast_kind_t kind, expression_t lvalue)
{
    expression_t expression = expression_new(EXPRESSION_IMPLICIT_CAST);
    implicit_cast_t implicit_cast = IMPLICIT_CAST(expression);
    implicit_cast->kind = kind;
    implicit_cast->expression = lvalue;
    return expression;
}

expression_t conditional_new(expression_t lhs, conditional_kind_t op, expression_t rhs)
{
    expression_t expression = expression_new(EXPRESSION_CONDITIONAL);
    conditional_t conditional = CONDITIONAL(expression);
    conditional->lhs = lhs;
    conditional->rhs = rhs;
    conditional->op = op;

    return expression;
}

static
void binary_free(binary_t binary)
{
    expression_free(binary->lhs);
    expression_free(binary->rhs);
}

static
void call_free(call_t call)
{
    expression_free(call->callee);
    array_cleanup_free(call->argument_s, (release_t)expression_free);
}

static
void assignment_free(assignment_t assignment)
{
    expression_free(assignment->lvalue);
    expression_free(assignment->rvalue);
}

static
void implict_cast_free(implicit_cast_t implicit_cast)
{
    expression_free(implicit_cast->expression);
}

static
void conditional_free(conditional_t conditional)
{
    expression_free(conditional->lhs);
    expression_free(conditional->rhs);
}

void expression_free(expression_t expression)
{
    switch (expression->kind) // LCOV_EXCL_LINE
    {
        case EXPRESSION_CONSTANT:
        case EXPRESSION_IDENTIFIER:
            break;
        case EXPRESSION_BINARY:
            binary_free(BINARY(expression));
            break;
        case EXPRESSION_CALL:
            call_free(CALL(expression));
            break;
        case EXPRESSION_ASSIGNMENT:
            assignment_free(ASSIGNMENT(expression));
            break;
        case EXPRESSION_IMPLICIT_CAST:
            implict_cast_free(IMPLICIT_CAST(expression));
            break;
        case EXPRESSION_CONDITIONAL:
            conditional_free(CONDITIONAL(expression));
            break;
    }

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

bool constant_bool(constant_t constant)
{
    return constant->_bool;
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

expression_t binary_lhs(binary_t binary)
{
    return binary->lhs;
}

void binary_set_lhs(binary_t binary, expression_t lhs)
{
    binary->lhs = lhs;
}

expression_t binary_rhs(binary_t binary)
{
    return binary->rhs;
}

void binary_set_rhs(binary_t binary, expression_t rhs)
{
    binary->rhs = rhs;
}

binary_kind_t binary_op(binary_t binary)
{
    return binary->op;
}

expression_t call_callee(call_t call)
{
    return call->callee;
}

array_t(expression_t) call_argument_s(call_t call)
{
    return call->argument_s;
}

expression_t assignment_lvalue(assignment_t assignment)
{
    return assignment->lvalue;
}

expression_t assignment_rvalue(assignment_t assignment)
{
    return assignment->rvalue;
}

void assignment_set_rvalue(assignment_t assignment, expression_t rvalue)
{
    assignment->rvalue = rvalue;
}

implicit_cast_kind_t implicit_cast_kind(implicit_cast_t implicit_cast)
{
    return implicit_cast->kind;
}

expression_t implicit_cast_expression(implicit_cast_t implicit_cast)
{
    return implicit_cast->expression;
}

expression_t conditional_lhs(conditional_t conditional)
{
    return conditional->lhs;
}

void conditional_set_lhs(conditional_t conditional, expression_t lhs)
{
    conditional->lhs = lhs;
}

expression_t conditional_rhs(conditional_t conditional)
{
    return conditional->rhs;
}

void conditional_set_rhs(conditional_t conditional, expression_t rhs)
{
    conditional->rhs = rhs;
}

conditional_kind_t conditional_op(conditional_t conditional)
{
    return conditional->op;
}

static
type_t constant_type(constant_t constant)
{
    switch (constant_kind(constant)) // LCOV_EXCL_LINE
    {
        case CONSTANT_BOOL:
            return type_bool();
        case CONSTANT_U64:
            return type_int();
    }
    __builtin_unreachable();
}

static
type_t identifier_type(identifier_t identifier)
{
    return declaration_type(identifier_declaration(identifier));
}

static
type_t binary_type(binary_t binary)
{
    type_t lhs = expression_type(binary_lhs(binary));
    type_t rhs = expression_type(binary_rhs(binary));
    (void)rhs;

    binary_kind_t op = binary_op(binary);
    switch (op) // LCOV_EXCL_LINE
    {
    case BINARY_EQ:
    case BINARY_NE:
    case BINARY_LT:
    case BINARY_LE:
    case BINARY_GT:
    case BINARY_GE:
        return type_bool();
    case BINARY_ADD:
    case BINARY_SUB:
    case BINARY_MUL:
    case BINARY_DIV:
    case BINARY_REM:
        return lhs;
    }
    __builtin_unreachable();
}

static
type_t call_type(call_t call)
{
    expression_t callee = call_callee(call);

    type_t callee_type = expression_type(callee);
    if (type_kind(callee_type) != TYPE_CALLABLE)
        return NULL;

    callable_t callable = CALLABLE(callee_type);
    return callable_return_type(callable);
}

static
type_t assignment_type(assignment_t assignment)
{
    return expression_type(assignment_lvalue(assignment));
}

type_t implicit_cast_type(implicit_cast_t implicit_cast)
{
    return expression_type(implicit_cast_expression(implicit_cast));
}

static
type_t conditional_type(conditional_t conditional)
{
    return type_bool();
}

type_t expression_type(expression_t expression)
{
    switch (expression_kind(expression)) // LCOV_EXCL_LINE
    {
        case EXPRESSION_CONSTANT:
            return constant_type(CONSTANT(expression));
        case EXPRESSION_IDENTIFIER:
            return identifier_type(IDENTIFIER(expression));
        case EXPRESSION_BINARY:
            return binary_type(BINARY(expression));
        case EXPRESSION_CALL:
            return call_type(CALL(expression));
        case EXPRESSION_ASSIGNMENT:
            return assignment_type(ASSIGNMENT(expression));
        case EXPRESSION_IMPLICIT_CAST:
            return implicit_cast_type(IMPLICIT_CAST(expression));
        case EXPRESSION_CONDITIONAL:
            return conditional_type(CONDITIONAL(expression));
    }
    __builtin_unreachable();
}



constant_t CONSTANT(expression_t expression)
{
    return &expression->constant;
}

identifier_t IDENTIFIER(expression_t expression)
{
    return &expression->identifier;
}

binary_t BINARY(expression_t expression)
{
    return &expression->binary;
}

call_t CALL(expression_t expression)
{
    return &expression->call;
}

assignment_t ASSIGNMENT(expression_t expression)
{
    return &expression->assignment;
}

implicit_cast_t IMPLICIT_CAST(expression_t expression)
{
    return &expression->implicit_cast;
}

conditional_t CONDITIONAL(expression_t expression)
{
    return &expression->conditional;
}

const char* binary_kind_string(binary_kind_t kind)
{
    switch (kind)
    {
        case BINARY_ADD: return "+";
        case BINARY_SUB: return "-";
        case BINARY_MUL: return "*";
        case BINARY_DIV: return "/";
        case BINARY_REM: return "%";
        case BINARY_EQ:  return "==";
        case BINARY_NE:  return "!=";
        case BINARY_LT:  return "<";
        case BINARY_LE:  return "<=";
        case BINARY_GT:  return ">";
        case BINARY_GE:  return ">=";
    }
    __builtin_unreachable();
}