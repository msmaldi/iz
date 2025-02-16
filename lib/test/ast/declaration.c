#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/ast.h"
#include "ast/type.h"
#include "ast/declaration.h"
#include "ast/statement.h"
#include "common/array.h"
#include "common/mem.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        //  u64 one()
        //      return 1;
        type_t u64_ty = type_int_new();
        span_t name = span_sz("one");

        array_t(declaration_t) argument_s = array_empty();

        expression_t expression = constant_u64_new(1);
        statement_t statement = return_new(expression);

        declaration_t declaration = function_new(u64_ty, name, argument_s, statement);
        assert_int_equal(DECLARATION_FUNCTION, declaration_kind(declaration));

        function_t function = FUNCTION(declaration);
        assert_int_equal(u64_ty, function_return_type(function));
        assert_true(span_eq(name, function_name(function)));
        assert_int_equal(argument_s, function_argument_s(function));
        assert_int_equal(statement, function_statement(function));

        type_t function_ty = function_type(function);
        assert_int_equal(TYPE_CALLABLE, type_kind(function_ty));

        callable_t callable = CALLABLE(function_ty);
        type_t callable_return_ty = callable_return_type(callable);
        assert_int_equal(u64_ty, callable_return_ty);

        array_t(type_t) param_s = callable_param_s(callable);
        assert_int_equal(0, array_size(param_s));

        declaration_free(declaration);
    }

    {
        //  u64 num(u64 n)
        //      return n;
        type_t u64_ty = type_int_new();

        span_t name = span_sz("num");

        array_t(declaration_t) argument_s = array_empty();
        type_t arg0_ty = type_int_new();
        declaration_t arg0 = argument_new(arg0_ty, span_sz("n"));
        argument_s = array_add(argument_s, arg0);
        assert_int_equal(arg0_ty, declaration_type(arg0));

        argument_t argument = ARGUMENT(arg0);
        assert_int_equal(arg0_ty, argument_type(argument));
        assert_true(span_eq(span_sz("n"), argument_name(argument)));

        span_t n = span_sz("n");
        expression_t expression = identifier_new(n);
        assert_int_equal(EXPRESSION_IDENTIFIER, expression_kind(expression));
        identifier_t identifier = IDENTIFIER(expression);
        identifier_set_declaration(identifier, arg0);

        assert_int_equal(arg0, identifier_declaration(identifier));

        statement_t statement = return_new(expression);

        declaration_t declaration = function_new(u64_ty, name, argument_s, statement);
        assert_int_equal(DECLARATION_FUNCTION, declaration_kind(declaration));
        type_t declaration_ty = declaration_type(declaration);
        assert_int_equal(TYPE_CALLABLE, type_kind(declaration_ty));

        function_t function = FUNCTION(declaration);
        assert_int_equal(u64_ty, function_return_type(function));
        assert_true(span_eq(name, function_name(function)));
        assert_int_equal(argument_s, function_argument_s(function));

        assert_int_equal(statement, function_statement(function));

        type_t function_ty = function_type(function);
        assert_int_equal(TYPE_CALLABLE, type_kind(function_ty));

        callable_t callable = CALLABLE(function_ty);
        type_t callable_return_ty = callable_return_type(callable);
        assert_int_equal(u64_ty, callable_return_ty);

        array_t(type_t) param_s = callable_param_s(callable);
        assert_int_equal(1, array_size(param_s));

        declaration_free(declaration);
    }

    {
        type_t int_ty = type_int_new();
        span_t name = span_sz("n");

        declaration_t n = variable_new(int_ty, name, NULL);
        variable_t var_n = VARIABLE(n);

        assert_true(type_eq(variable_type(var_n), int_ty));
        assert_true(span_eq(variable_name(var_n), name));
        assert_null(variable_initializer(var_n));

        declaration_free(n);
    }

    {
        type_t int_ty = type_int_new();
        span_t name = span_sz("n");

        uint64_t two = 2;
        expression_t two_expr = constant_u64_new(two);

        declaration_t n = variable_new(int_ty, name, two_expr);

        declaration_free(n);
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