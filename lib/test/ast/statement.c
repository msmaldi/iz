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
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_data)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}