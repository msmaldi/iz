#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include <unistd.h>

#include "common/source.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        const char *path = "/etc/lsb-release";
        source_t source = source_load(path);

        assert_non_null(source_code(source));
        assert_true(source_size(source) > 0);
        assert_int_equal(path, source_path(source));

        source_free(source);
    }

    {
        const char *path = "/dev/null/not/exist";
        source_t source = source_load(path);

        assert_null(source);
    }

    {
        const char *path = "../docs/samples/v0.0.1.iz";
        source_t source = source_load(path);

        assert_non_null(source_code(source));
        assert_true(source_size(source) > 0);
        assert_int_equal(path, source_path(source));

        source_free(source);
    }

    {
        const char *path = "../docs/samples/v0.0.2.iz";
        source_t source = source_load(path);

        assert_non_null(source_code(source));
        assert_true(source_size(source) > 0);
        assert_int_equal(path, source_path(source));

        source_free(source);
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