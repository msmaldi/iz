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
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_data)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}