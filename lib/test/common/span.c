#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "common/span.h"

static void test_data(void **arg)
{
    (void) arg;

    char *content = "hello, hello";

    span_t s0 = span_ctor(5, content);
    span_t s1 = span_ctor(5, content + 7);
    assert_true(span_eq(s0, s1));

    span_t s2 = span_ctor(10, content);
    assert_false(span_eq(s0, s2));

    span_t s3 = span_ctor(10, content + 1);
    assert_false(span_eq(s2, s3));

    span_t s4 = span_sz(content);
    assert_int_equal(s4.size, strlen(content));
}

static void test_span_cmp(void **arg)
{
    (void) arg;

    span_t a = span_sz("a");
    span_t b = span_sz("b");
    span_t aa = span_sz("aa");

    assert_true(span_cmp(a, a) == 0);

    assert_true(span_cmp(a, b) < 0);
    assert_true(span_cmp(b, a) > 0);

    assert_true(span_cmp(a, aa) < 0);
    assert_true(span_cmp(aa, a) > 0);

}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_data),
        cmocka_unit_test(test_span_cmp),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}