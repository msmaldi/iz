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

        compilation_print(compilation, stdout);

        compilation_free(compilation);
    }

    {
        source_t code_v002 = source_load("../docs/samples/v0.0.2.iz");
        unit_t unit = syntax_analysis(code_v002);
        array_t unit_s = array_add(array_empty(), unit);
        compilation_t compilation = semantic_analysis(unit_s);
        assert_non_null(compilation);

        compilation_print(compilation, stdout);

        compilation_free(compilation);
    }

    {
        source_t code_v003 = source_load("../docs/samples/v0.0.3.iz");
        unit_t unit = syntax_analysis(code_v003);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_non_null(compilation);

        compilation_print(compilation, stdout);

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

    {
        source_t code_v005 = source_load("../docs/samples/v0.0.5.iz");
        unit_t unit = syntax_analysis(code_v005);
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

    {
        char *code =
        "int one()"           LF
        "{"                   LF
        "    int ret = true;" LF
        "    return ret;"     LF
        "}"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error009.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int num(bool b)" LF
        "{"               LF
        "    int x = b;"  LF
        "    return x;"   LF
        "}"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error010.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // int + bool: used to crash codegen (LLVMBuildAdd on mismatched
        // widths), now caught by binary_analysis's type check.
        char *code =
        "int add_bad(int n, bool b)" LF
        "    return n + b;"          LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error011.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // Undeclared identifier used as a binary operand: used to segfault
        // (implicit_cast dereferenced a NULL declaration); check_type's
        // NULL-type guard must avoid a second, cascading diagnostic. Tried
        // on both sides so both short-circuit paths of the NULL check (and
        // expression_location's identifier-with-no-declaration fallback)
        // get exercised.
        char *code =
        "int add_bad(int n)"        LF
        "    return not_exist + n;" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error012.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        char *code =
        "int add_bad(int n)"        LF
        "    return n + not_exist;" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error012b.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // Plain assignment (not a var declaration) with a mismatched type
        // on the right-hand side was never checked before.
        char *code =
        "int assign_bad(int x, bool b)" LF
        "{"                             LF
        "    x = b;"                    LF
        "    return x;"                 LF
        "}"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error013.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // if condition must be bool.
        char *code =
        "int if_bad(int n)" LF
        "{"                 LF
        "    if (n)"        LF
        "        return 1;" LF
        "    return 0;"     LF
        "}"
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error014.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // && / || operands must be bool.
        char *code =
        "bool cond_bad(int n)" LF
        "    return n && n;"   LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error015.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // Wrong number of arguments in a call.
        char *code =
        "int num(int n)"      LF
        "    return n;"       LF
        "int call_bad()"      LF
        "    return num(1, 2);" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error016.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // Argument type does not match the corresponding parameter type.
        char *code =
        "int num(int n)"       LF
        "    return n;"        LF
        "int call_bad()"       LF
        "    return num(true);" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error017.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        assert_null(compilation);
    }

    {
        // Calling an undeclared function: the callee never resolves to a
        // callable type, so no argument/arity checks should run either.
        char *code =
        "int call_bad()"          LF
        "    return not_a_function(1);" LF
        ;
        unit_t unit = syntax_analysis(source_inline(code, "sema_error018.iz"));
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