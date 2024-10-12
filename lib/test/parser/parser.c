#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "parser/parser.h"

#define LF "\n"

static void success_branch(void **arg)
{
    (void) arg;

    {
        parser_t parser = parser_new("");
        parser_free(parser);
    }

    {
        const char* code = "1";
        parser_t parser = parser_new(code);

        expression_t one = scan_expression(parser);

        assert_non_null(one);

        expression_free(one);
        parser_free(parser);
    }

    {
        const char* code = "n";
        parser_t parser = parser_new(code);

        expression_t n = scan_expression(parser);

        assert_non_null(n);

        expression_free(n);
        parser_free(parser);
    }

    {
        const char* code = "return 1;";
        parser_t parser = parser_new(code);

        statement_t return_1 = scan_statement(parser);
        assert_non_null(return_1);

        expression_t one = return_expression(RETURN(return_1));
        assert_non_null(one);

        constant_t constant_one = CONSTANT(one);
        assert_non_null(constant_one);
        assert_int_equal(constant_u64(constant_one), 1);

        statement_free(return_1);
        parser_free(parser);
    }

    {
        const char *code =
        "int one()"
        "    return 1;";

        parser_t parser = parser_new(code);

        declaration_t def_one = scan_function(parser);
        function_t fn_one = FUNCTION(def_one);
        assert_non_null(fn_one);

        declaration_free(def_one);

        parser_free(parser);
    }

    {
        const char *code =
        "int num(int n)"
        "    return n;";
        parser_t parser = parser_new(code);

        declaration_t def_num = scan_function(parser);
        function_t fn_one = FUNCTION(def_num);
        assert_non_null(fn_one);

        declaration_free(def_num);
        parser_free(parser);
    }

    {
        const char *code =
        "int num(int a, int b)"
        "    return a;"
        ;
        parser_t parser = parser_new(code);

        declaration_t def_num = scan_function(parser);
        function_t fn_one = FUNCTION(def_num);
        assert_non_null(fn_one);

        declaration_free(def_num);
        parser_free(parser);
    }

    {
        char *code_a001 =
        "int one()"      LF
        "    return 1;"  LF
        ""               LF
        "int num(int n)" LF
        "    return n;"  LF
        ;

        unit_t unit_a001 = parse(code_a001);

        unit_free(unit_a001);
    }



}

static void failure_branch(void **arg)
{
    (void) arg;

    {
        const char* code = "#";
        parser_t parser = parser_new(code);

        expression_t one = scan_expression(parser);

        assert_null(one);

        parser_free(parser);
    }

    {
        const char* code = "return #";
        parser_t parser = parser_new(code);

        statement_t return_1 = scan_statement(parser);
        assert_null(return_1);

        parser_free(parser);
    }

    {
        const char* code = "return 1#";
        parser_t parser = parser_new(code);

        statement_t return_1 = scan_statement(parser);
        assert_null(return_1);

        parser_free(parser);
    }

    {
        const char *code =
        "# one()"
        "    return 1;";

        parser_t parser = parser_new(code);

        declaration_t fn_one = scan_function(parser);
        assert_null(fn_one);

        parser_free(parser);
    }

    {
        const char *code =
        "int #()"
        "    return 1;";

        parser_t parser = parser_new(code);

        declaration_t fn_one = scan_function(parser);
        assert_null(fn_one);

        parser_free(parser);
    }

    {
        const char *code =
        "int one#)"
        "    return 1;";

        parser_t parser = parser_new(code);

        declaration_t fn_one = scan_function(parser);
        assert_null(fn_one);

        parser_free(parser);
    }

    {
        const char *code =
        "int one(#"
        "    return 1;";

        parser_t parser = parser_new(code);

        declaration_t fn_one = scan_function(parser);
        assert_null(fn_one);

        parser_free(parser);
    }

    {
        const char *code =
        "int one()"
        "    # 1;";

        parser_t parser = parser_new(code);

        declaration_t fn_one = scan_function(parser);
        assert_null(fn_one);

        parser_free(parser);
    }

    {
        const char *code =
        "int num(int ###)"
        "    return n;";
        parser_t parser = parser_new(code);

        declaration_t def_num = scan_function(parser);
        assert_null(def_num);

        parser_free(parser);
    }

    {
        const char *code =
        "int ##";
        parser_t parser = parser_new(code);

        declaration_t arg = scan_argument(parser);
        assert_null(arg);

        parser_free(parser);
    }

    {
        const char *code =
        "int num(int a ###)"
        "    return a;";
        parser_t parser = parser_new(code);

        declaration_t def_num = scan_function(parser);
        assert_null(def_num);

        parser_free(parser);
    }

    {
        const char *code =
        "int num(int a, int ###)"
        "    return a;";
        parser_t parser = parser_new(code);

        declaration_t def_num = scan_function(parser);
        assert_null(def_num);

        parser_free(parser);
    }

    {
        char *code =
        "int one()"      LF
        "    return 1;"  LF
        ""               LF
        "##############" LF
        ;

        parser_t parser = parser_new(code);

        unit_t unit_a001 = scan_unit(parser);

        assert_null(unit_a001);

        parser_free(parser);
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