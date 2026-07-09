#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#define LF "\n"

#include "parser/parser.h"
#include "sema/sema.h"
#include "llvm/backend.h"

static void success_branch(void **arg)
{
    (void) arg;

    {
        source_t code_v001 = source_load("../docs/samples/v0.0.1.iz");
        unit_t unit = syntax_analysis(code_v001);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        backend_optimize(backend);
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }

    {
        source_t code_v002 = source_load("../docs/samples/v0.0.2.iz");
        unit_t unit = syntax_analysis(code_v002);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        //backend_optimize(backend);
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }

    {
        source_t code_v003 = source_load("../docs/samples/v0.0.3.iz");
        unit_t unit = syntax_analysis(code_v003);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        //backend_optimize(backend);
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }

    {
        source_t code_v004 = source_load("../docs/samples/v0.0.4.iz");
        unit_t unit = syntax_analysis(code_v004);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        //backend_optimize(backend);
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }

    {
        source_t code_v005 = source_load("../docs/samples/v0.0.5.iz");
        unit_t unit = syntax_analysis(code_v005);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }

    {
        source_t code_v006 = source_load("../docs/samples/v0.0.6.iz");
        unit_t unit = syntax_analysis(code_v006);
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }

    {
        char *code =
        "int choose(int n)"     LF
        "{"                     LF
        "    int x = 1;"        LF
        "    if (n < 0)"        LF
        "    {"                 LF
        "        x = 2;"        LF
        "    }"                 LF
        "    else"              LF
        "    {"                 LF
        "        x = 3;"        LF
        "    }"                 LF
        "    return x;"         LF
        "}"
        ;

        unit_t unit = syntax_analysis(source_inline(code, "if_else_no_return.iz"));
        compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
        backend_t backend = backend_codegen(compilation);
        assert_non_null(backend);

        assert_true(backend_validate(backend));
        backend_dump(backend);
        backend_emit_object(backend);
        backend_emit_assembly(backend);
        backend_emit_llvm(backend);

        backend_free(backend);
    }
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(success_branch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}