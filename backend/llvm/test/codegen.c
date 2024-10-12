#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#define LF "\n"

#include "parser/parser.h"
#include "sema/sema.h"
#include "codegen.h"

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

        array_t(unit_t) unit_s = array_empty();
        unit_t unit = parse(code_a001);
        unit_s = array_add(unit_s, unit);

        sema_t sema = sema_new(unit_s);
        sema_analysis(sema);
        assert_false(sema_errors(sema));

        codegen_t codegen = codegen_new(unit_s);

        codegen_generate(codegen);

        assert_true(codegen_validate(codegen));

        codegen_dump(codegen);

        codegen_emit_object(codegen, "code_a001.o");

        sema_free(sema);
        codegen_free(codegen);
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