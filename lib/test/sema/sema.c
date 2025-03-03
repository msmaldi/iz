#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/ast_print.h"
#include "parser/parser.h"
#include "sema/sema.h"

#define LF "\n"

static void success_branch(void **arg)
{
    (void) arg;

    {
        source_t code_v001 = source_load("../docs/samples/v0.0.1.iz");
        unit_t unit = syntax_analysis(code_v001);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_non_null(compilation);

        compilation_free(compilation);
    }

    {
        source_t code_v002 = source_load("../docs/samples/v0.0.2.iz");
        unit_t unit = syntax_analysis(code_v002);
        array_t unit_s = array_add(array_empty(), unit);
        compilation_t compilation = semantic_analysis(unit_s);
        assert_non_null(compilation);

        compilation_free(compilation);
    }

    {
        source_t code_v003 = source_load("../docs/samples/v0.0.3.iz");
        unit_t unit = syntax_analysis(code_v003);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_non_null(compilation);

        compilation_free(compilation);
    }

    {
        source_t code_v004 = source_load("../docs/samples/v0.0.4.iz");
        unit_t unit = syntax_analysis(code_v004);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_non_null(compilation);

        compilation_print(compilation, stdout);

        compilation_free(compilation);
    }
}

static void failure_branch(void **arg)
{
    (void) arg;

    {
        char *code =
        "int num(int n)" LF
        "    return n;"  LF
        "int num(int n)" LF
        "    return n;"  LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error001.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int num(int n)"        LF
        "    return not_exist;" LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error002.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int num(int n, int n)" LF
        "    return n;"         LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error003.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int num(int num)" LF
        "    return num;"  LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error004.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "bool one()"     LF
        "    return 1;"  LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error005.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int num(int n)"   LF
        "    return n();"  LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error006.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int int_lt(int lhs, int rhs)"  LF
        "{"                             LF
        "    if (lhs < rhs)"            LF
        "    {"                         LF
        "    }"                         LF
        "    else"                      LF
        "    {"                         LF
        "    }"                         LF
        "}"                             LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error007.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int num(int n)" LF
        "{"              LF
        "    int a;"     LF
        "    int a;"     LF
        "    return n;"  LF
        "}"              LF
        ;

        unit_t unit = syntax_analysis(source_inline(code, "sema_error008.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(failure_branch),
        cmocka_unit_test(success_branch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}