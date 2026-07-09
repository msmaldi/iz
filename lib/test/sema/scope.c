#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "sema/scope.h"

static declaration_t create_declaration_one()
{
    type_t int_ty = type_int_new();
    span_t name = span_sz("one");
    array_t(declaration_t) argument_s = array_empty();

    expression_t expression = constant_u64_new(1);
    statement_t statement = return_new(expression);

    return function_new(int_ty, (struct location_t){ .span = name }, argument_s, statement);
}

static void success_branch(void **arg)
{
    (void) arg;

    {
        scope_t global = scope_new(NULL);

        declaration_t fn_one = create_declaration_one();
        assert_true(scope_add(global, fn_one));

        declaration_t fn_another_one = create_declaration_one();
        assert_false(scope_add(global, fn_another_one));

        // This is creation of function num, this is importante create here
        // because we gonna test insertion on argument 'n' in inner scope
        type_t int_ty = type_int_new();
        span_t name = span_sz("num");
        array_t(declaration_t) argument_s = array_empty();
        type_t arg0_ty = type_int_new();
        declaration_t arg0 = argument_new(arg0_ty, (struct location_t){ .span = span_sz("n") });
        argument_s = array_add(argument_s, arg0);
        span_t n = span_sz("n");

        struct location_t location = { .span = n, .line = 0, .column = 0 };
        expression_t expression = identifier_new(location);
        statement_t statement = return_new(expression);
        declaration_t fn_num = function_new(int_ty, (struct location_t){ .span = name }, argument_s, statement);

        assert_true(scope_add(global, fn_num));

        scope_t fn_num_scope = scope_new(global);
        assert_true(scope_add(fn_num_scope, arg0));
        assert_int_equal(global, scope_parent(fn_num_scope));

        declaration_t arg_n = scope_find(fn_num_scope, span_sz("n"));
        assert_int_equal(arg0, arg_n);

        declaration_t fn_one_find = scope_find(fn_num_scope, span_sz("one"));
        assert_int_equal(fn_one, fn_one_find);

        assert_null(scope_find(global, span_sz("n")));

        scope_free(fn_num_scope);
        declaration_free(fn_num);
        declaration_free(fn_another_one);
        declaration_free(fn_one);
        scope_free(global);
    }

    {
        // Insertion order found (by simulating insert_node/fix_up offline) to
        // drive the red-black tree through rotate_left, rotate_right -
        // including the case where the rotated node already has a right
        // child - and the color-flip branch inside fix_up.
        const char *names[] = { "a", "b", "c", "f", "g", "d", "e" };
        size_t size = sizeof(names) / sizeof(names[0]);

        scope_t scope = scope_new(NULL);
        declaration_t declaration_s[size];
        type_t type_s[size];

        for (size_t i = 0; i < size; i++)
        {
            // argument_free() does not release the argument's type (that is
            // normally done through the owning function's callable type), so
            // since these arguments have no owning function here, the types
            // are tracked and released separately below.
            type_s[i] = type_int_new();
            declaration_t argument = argument_new(type_s[i], (struct location_t){ .span = span_sz(names[i]) });
            declaration_s[i] = argument;

            assert_true(scope_add(scope, argument));
        }

        for (size_t i = 0; i < size; i++)
        {
            declaration_t found = scope_find(scope, span_sz(names[i]));
            assert_int_equal(declaration_s[i], found);
        }

        scope_free(scope);
        for (size_t i = 0; i < size; i++)
        {
            declaration_free(declaration_s[i]);
            type_free(type_s[i]);
        }
    }
}

static void failure_branch(void **arg)
{
    (void) arg;
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(success_branch),
        cmocka_unit_test(failure_branch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}