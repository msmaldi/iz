#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/expression.h"
#include "common/span.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        uint64_t one = 1;
        expression_t expression = constant_u64_new(one);
        assert_int_equal(EXPRESSION_CONSTANT, expression_kind(expression));

        constant_t constant = CONSTANT(expression);
        assert_non_null(constant);
        assert_int_equal(constant_kind(constant), CONSTANT_U64);
        assert_int_equal(constant_u64(constant), one);

        expression_free(expression);
    }

    {
        span_t name = span_sz("n");
        expression_t expression = identifier_new(name);
        assert_int_equal(EXPRESSION_IDENTIFIER, expression_kind(expression));

        identifier_t identifier = IDENTIFIER(expression);
        assert_non_null(identifier);
        assert_true(span_eq(name, identifier_name(identifier)));

        assert_null(identifier_declaration(identifier));

        expression_free(expression);
    }

    {
        span_t name = span_sz("n");
        expression_t n_expr = identifier_new(name);

        uint64_t one = 1;
        expression_t one_expr = constant_u64_new(one);

        expression_t expression = binary_new(n_expr, BINARY_ADD, one_expr);

        binary_t binary = BINARY(expression);
        assert_non_null(binary);

        assert_int_equal(n_expr, binary_lhs(binary));
        assert_int_equal(one_expr, binary_rhs(binary));
        assert_int_equal(BINARY_ADD, binary_op(binary));

        expression_free(expression);
    }

    {
        span_t name = span_sz("fib");
        expression_t fib_expr = identifier_new(name);

        uint64_t one = 1;
        expression_t one_expr = constant_u64_new(one);

        array_t(expression_t) argument_s = array_empty();
        argument_s = array_add(argument_s, one_expr);

        expression_t call_expr = call_new(fib_expr, argument_s);

        call_t call = CALL(call_expr);
        assert_non_null(call);

        assert_int_equal(fib_expr, call_callee(call));
        assert_int_equal(argument_s, call_argument_s(call));

        expression_free(call_expr);
    }

    {
        span_t n = span_sz("n");
        expression_t n_expr = identifier_new(n);

        uint64_t one = 1;
        expression_t one_expr = constant_u64_new(one);

        expression_t assignment_expr = assignment_new(n_expr, one_expr);

        assert_int_equal(EXPRESSION_ASSIGNMENT, expression_kind(assignment_expr));
        assignment_t assignment = ASSIGNMENT(assignment_expr);

        assert_int_equal(n_expr, assignment_lvalue(assignment));
        assert_int_equal(one_expr, assignment_rvalue(assignment));

        expression_free(assignment_expr);
    }
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_data)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}