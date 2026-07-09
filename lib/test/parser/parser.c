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

    {
        source_t code_v005 = source_load("../docs/samples/v0.0.5.iz");

        unit_t unit = syntax_analysis(code_v005);

        unit_free(unit);
    }

    {
        source_t code_v006 = source_load("../docs/samples/v0.0.6.iz");

        unit_t unit = syntax_analysis(code_v006);

        unit_free(unit);
    }

    {
        // C#-style pointer declarators: in "T* a, b;" the '*' belongs to the
        // shared type, so ALL declarators are pointers - unlike C, where
        // only 'a' would be a pointer and 'b' a plain T.
        char *code =
        "int f()"        LF
        "    int* a, b;" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "ptr_decl.iz"));
        assert_non_null(unit);

        declaration_t declaration = unit_declaration_s(unit)[0];
        statement_t statement = function_statement(FUNCTION(declaration));
        assert_int_equal(STATEMENT_VAR, statement_kind(statement));

        array_t(declaration_t) variable_s = var_variable_s(VAR(statement));
        assert_int_equal(2, array_size(variable_s));

        assert_int_equal(TYPE_POINTER, type_kind(variable_type(VARIABLE(variable_s[0]))));
        assert_int_equal(TYPE_POINTER, type_kind(variable_type(VARIABLE(variable_s[1]))));

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

    // The following exercise the "invalid right-hand side" error recovery
    // path at each binary/conditional operator precedence level.
    {
        char *code = "int one() return 1 * #;";
        unit_t unit = syntax_analysis(source_inline(code, "error026.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 / #;";
        unit_t unit = syntax_analysis(source_inline(code, "error027.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 % #;";
        unit_t unit = syntax_analysis(source_inline(code, "error028.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 + #;";
        unit_t unit = syntax_analysis(source_inline(code, "error029.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 - #;";
        unit_t unit = syntax_analysis(source_inline(code, "error030.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 < #;";
        unit_t unit = syntax_analysis(source_inline(code, "error031.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 > #;";
        unit_t unit = syntax_analysis(source_inline(code, "error032.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 <= #;";
        unit_t unit = syntax_analysis(source_inline(code, "error033.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 >= #;";
        unit_t unit = syntax_analysis(source_inline(code, "error034.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 == #;";
        unit_t unit = syntax_analysis(source_inline(code, "error035.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 != #;";
        unit_t unit = syntax_analysis(source_inline(code, "error036.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 && #;";
        unit_t unit = syntax_analysis(source_inline(code, "error037.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return 1 || #;";
        unit_t unit = syntax_analysis(source_inline(code, "error038.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return *#;";
        unit_t unit = syntax_analysis(source_inline(code, "error039.iz"));
        assert_null(unit);
    }

    {
        char *code = "int one() return &#;";
        unit_t unit = syntax_analysis(source_inline(code, "error040.iz"));
        assert_null(unit);
    }

    {
        // C-style per-declarator '*' (e.g. "int a, *b;") is rejected: this
        // language only supports the C#-style "T* a, b;" (see success_branch).
        char *code =
        "int f()"        LF
        "    int a, *b;" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "error041.iz"));
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