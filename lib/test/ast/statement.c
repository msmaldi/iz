#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/statement.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        uint64_t one = 1;
        expression_t expression = constant_u64_new(one);

        statement_t statement = return_new(expression);
        assert_int_equal(STATEMENT_RETURN, statement_kind(statement));
        return_t ret = RETURN(statement);

        assert_non_null(ret);
        assert_int_equal(return_expression(ret), expression);

        statement_free(statement);
    }

    {
        span_t name = span_sz("n");
        expression_t n_expr = identifier_new(name);

        uint64_t two = 2;
        expression_t two_expr = constant_u64_new(two);

        statement_t then_branch = return_new(identifier_new(name));
        statement_t else_branch = NULL;

        expression_t condition = binary_new(n_expr, BINARY_LT, two_expr);

        statement_t statement = if_new(condition, then_branch, else_branch);

        if_t ifelse = IF(statement);
        assert_non_null(ifelse);
        assert_int_equal(condition, if_condition(ifelse));
        assert_int_equal(then_branch, if_then_branch(ifelse));
        assert_int_equal(else_branch, if_else_branch(ifelse));

        statement_free(statement);
    }



    {
        span_t name = span_sz("n");
        expression_t n_expr = identifier_new(name);

        uint64_t two = 2;
        expression_t two_expr = constant_u64_new(two);

        statement_t then_branch = return_new(identifier_new(name));
        statement_t else_branch = block_new(array_empty());

        expression_t condition = binary_new(n_expr, BINARY_LT, two_expr);

        statement_t statement = if_new(condition, then_branch, else_branch);

        if_t ifelse = IF(statement);
        assert_non_null(ifelse);
        assert_int_equal(condition, if_condition(ifelse));
        assert_int_equal(then_branch, if_then_branch(ifelse));
        assert_int_equal(else_branch, if_else_branch(ifelse));

        block_t block = BLOCK(else_branch);
        assert_non_null(block);
        assert_int_equal(array_empty(), block_statement_s(block));

        statement_free(statement);
    }

    {
        type_t int_ty = type_int_new();
        span_t name = span_sz("n");

        uint64_t two = 2;
        expression_t two_expr = constant_u64_new(two);

        declaration_t n = variable_new(int_ty, name, two_expr);

        array_t(declaration_t) var_s = array_empty();
        var_s = array_add(var_s, n);

        statement_t var_stmt = var_new(var_s);
        var_t var = VAR(var_stmt);

        assert_int_equal(var_variable_s(var), var_s);

        statement_free(var_stmt);
    }

    {
        span_t name = span_sz("fib");
        expression_t fib_expr = identifier_new(name);

        uint64_t two = 2;
        expression_t two_expr = constant_u64_new(two);

        array_t(expression_t) argument_s = array_empty();
        argument_s = array_add(argument_s, two_expr);

        expression_t call_expr = call_new(fib_expr, argument_s);

        array_t(expression_t) expression_s = array_empty();
        expression_s = array_add(expression_s, call_expr);

        statement_t statement = act_new(expression_s);

        statement_free(statement);
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