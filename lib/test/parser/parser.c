#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "parser/parser.h"
#include "common/source.h"

#define LF "\n"

static void success_branch(void **arg)
{
    (void) arg;

    {
        source_t code_v001 = source_load("../docs/samples/v0.0.1.iz");

        unit_t unit = syntax_analysis(code_v001);

        unit_free(unit);
    }

    {
        source_t code_v002 = source_load("../docs/samples/v0.0.2.iz");

        unit_t unit = syntax_analysis(code_v002);

        unit_free(unit);
    }

    {
        source_t code_v003 = source_load("../docs/samples/v0.0.3.iz");

        unit_t unit = syntax_analysis(code_v003);

        unit_free(unit);
    }


    {
        source_t code_v004 = source_load("../docs/samples/v0.0.4.iz");

        unit_t unit = syntax_analysis(code_v004);

        unit_free(unit);
    }
}

static void failure_branch(void **arg)
{
    (void) arg;

    {
        const char *code =
        "# one()"
        "    return 1;";

        unit_t unit = syntax_analysis(source_inline(code, "error001.iz"));
        assert_null(unit);
    }


    {
        const char *code =
        "int #()"
        "    return 1;";

        unit_t unit = syntax_analysis(source_inline(code, "error002.iz"));
        assert_null(unit);
    }

    {
        const char *code =
        "int one#)"
        "    return 1;";

        unit_t unit = syntax_analysis(source_inline(code, "error003.iz"));
        assert_null(unit);
    }

    {
        const char *code =
        "int one(#"
        "    return 1;";

        unit_t unit = syntax_analysis(source_inline(code, "error004.iz"));
        assert_null(unit);
    }

    {
        const char *code =
        "int one()"  LF
        "    # 1;";

        unit_t unit = syntax_analysis(source_inline(code, "error005.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int one()"     LF
        "    return #"  LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error006.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int one()"      LF
        "    return 1"   LF
        "#"              LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error007.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int one()"         LF
        "    return fib(#"  LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error008.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int one()"          LF
        "    return fib(1#"  LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error009.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int one()"           LF
        "    return fib(1,#"  LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error010.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int one()"  LF
        "{"          LF
        "    #"      LF
        "}"          LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error011.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int min(int lhs, int rhs)" LF
        "{"                         LF
        "    if #"                  LF
        "}"                         LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error012.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int min(int lhs, int rhs)" LF
        "{"                         LF
        "    if (#"                 LF
        "}"                         LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error013.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int min(int lhs, int rhs)" LF
        "{"                         LF
        "    if (lhs < rhs #"       LF
        "}"                         LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error014.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int min(int lhs, int rhs)" LF
        "{"                         LF
        "    if (lhs < rhs) #"      LF
        "}"                         LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error015.iz"));
        assert_null(unit);
    }

    {
        const char* code =
        "int min(int lhs, int rhs)" LF
        "{"                         LF
        "    if (lhs < rhs)"        LF
        "        return lhs;"       LF
        "    else #"                LF
        "}"                         LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error016.iz"));
        assert_null(unit);
    }

    {
        const char *code =
        "int num(int ###)"
        "    return n;"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error017.iz"));
        assert_null(unit);
    }

    {
        const char *code =
        "int num(int a ###)"
        "    return a;"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error018.iz"));
        assert_null(unit);
    }

    {
        const char *code =
        "int num(int a, int ###)"
        "    return a;"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error019.iz"));
        assert_null(unit);
    }

    {
        char *code =
        "int one()"      LF
        "    return 1;"  LF
        ""               LF
        "##############" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error020.iz"));
        assert_null(unit);
    }

    {
        char *code =
        "int one()"      LF
        "{"              LF
        "    int #"      LF
        "}"              LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error021.iz"));
        assert_null(unit);
    }

    {
        char *code =
        "int one()"      LF
        "{"              LF
        "    int a = #"  LF
        "}"              LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error022.iz"));
        assert_null(unit);
    }

    {
        char *code =
        "int one()"        LF
        "{"                LF
        "    int a = 1 #"  LF
        "}"                LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error023.iz"));
        assert_null(unit);
    }

    {
        char *code =
        "int one()"        LF
        "{"                LF
        "    fib() #"      LF
        "}"                LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error024.iz"));
        assert_null(unit);
    }

    {
        char *code =
        "int one()"        LF
        "{"                LF
        "    int x;"       LF
        "    x = #"        LF
        "}"                LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error025.iz"));
        assert_null(unit);
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