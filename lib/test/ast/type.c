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
        type_t type_int = type_int_new();

        assert_int_equal(type_kind(type_int), TYPE_INT);

        type_free(type_int);
    }

    {
        type_t type_int = type_int_new();
        array_t(type_t) param_s = array_empty();

        type_t type_callable = type_callable_new(type_int, param_s);

        assert_int_equal(type_kind(type_callable), TYPE_CALLABLE);

        type_free(type_callable);
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