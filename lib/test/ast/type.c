#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/type.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        assert_int_equal(TYPE_BOOL, type_kind(type_bool()));
        assert_int_equal(TYPE_INT, type_kind(type_int()));
    }

    {
        type_t bool_ty = type_bool_new();

        assert_int_equal(type_kind(bool_ty), TYPE_BOOL);

        type_free(bool_ty);
    }

    {
        type_t int_ty = type_int_new();

        assert_int_equal(type_kind(int_ty), TYPE_INT);

        type_free(int_ty);
    }

    {
        type_t int_ty = type_int_new();
        array_t(type_t) param_s = array_empty();

        type_t type_callable = type_callable_new(int_ty, param_s);

        assert_int_equal(type_kind(type_callable), TYPE_CALLABLE);

        type_free(type_callable);
    }

    {
        assert_false(type_eq(type_bool(), type_int()));
        assert_false(type_eq(NULL, type_int()));
        assert_false(type_eq(type_bool(), NULL));
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