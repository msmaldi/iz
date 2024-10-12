#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "parser/parser.h"
#include "sema/sema.h"

#define LF "\n"

static void success_branch(void **arg)
{
    (void) arg;

    {
        char *code_a001 =
        "int one()"      LF
        "    return 1;"  LF
        ""               LF
        "int num(int n)" LF
        "    return n;"  LF
        ;

        unit_t unit = parse(code_a001);

        array_t(unit_t) unit_s = array_empty();
        unit_s =  array_add(unit_s, unit);

        sema_t sema = sema_new(unit_s);

        sema_analysis(sema);
        size_t errors = sema_errors(sema);
        assert_int_equal(0, errors);

        sema_free(sema);

        array_cleanup_free(unit_s, (release_t)unit_free);
    }
}

static void failure_branch(void **arg)
{
    (void) arg;

    {
        char *code_duplication_num_declaration =
        "int num(int n)" LF
        "    return n;"  LF
        "int num(int n)" LF
        "    return n;"  LF
        ;

        unit_t unit = parse(code_duplication_num_declaration);
        array_t(unit_t) unit_s = array_empty();
        unit_s =  array_add(unit_s, unit);

        sema_t sema = sema_new(unit_s);

        sema_analysis(sema);
        size_t errors = sema_errors(sema);
        assert_int_equal(1, errors);

        sema_free(sema);
        array_cleanup_free(unit_s, (release_t)unit_free);
    }

    {
        char *code_return_identifier_not_exist =
        "int num(int n)"        LF
        "    return not_exist;" LF
        ;

        unit_t unit = parse(code_return_identifier_not_exist);
        array_t(unit_t) unit_s = array_empty();
        unit_s =  array_add(unit_s, unit);

        sema_t sema = sema_new(unit_s);

        sema_analysis(sema);
        size_t errors = sema_errors(sema);
        assert_int_equal(1, errors);

        sema_free(sema);
        array_cleanup_free(unit_s, (release_t)unit_free);
    }

    {
        char *code_two_argument_s_with_same_identifier =
        "int num(int n, int n)" LF
        "    return n;"         LF
        ;

        unit_t unit = parse(code_two_argument_s_with_same_identifier);
        array_t(unit_t) unit_s = array_empty();
        unit_s =  array_add(unit_s, unit);

        sema_t sema = sema_new(unit_s);

        sema_analysis(sema);
        size_t errors = sema_errors(sema);
        assert_int_equal(1, errors);

        sema_free(sema);
        array_cleanup_free(unit_s, (release_t)unit_free);
    }
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