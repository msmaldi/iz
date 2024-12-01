#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/ast.h"
#include "ast/type.h"
#include "ast/declaration.h"
#include "ast/statement.h"
#include "common/array.h"
#include "common/mem.h"

#define LF "\n"

static void test_data(void **arg)
{
    (void) arg;

    {
        char *code_a001 =
            "u64 one()"      LF
            "    return 1;"  LF
            ""               LF
            "u64 num(u64 n)" LF
            "    return n;"  LF
            ;
        array_t(declaration_t) declaration_s = array_empty();

        {
            //  u64 one()
            //      return 1;
            type_t u64_ty = type_int_new();

            span_t name = span_sz("one");

            array_t(declaration_t) argument_s = array_empty();

            expression_t expression = constant_u64_new(1);
            statement_t statement = return_new(expression);

            declaration_t one_fn = function_new(u64_ty, name, argument_s, statement);

            declaration_s = array_add(declaration_s, one_fn);
        }

        {
            //  u64 num(u64 n)
            //      return n;
            type_t int_ty = type_int_new();

            span_t name = span_sz("num");

            array_t(declaration_t) argument_s = array_empty();
            type_t arg0_ty = type_int_new();
            declaration_t arg0 = argument_new(arg0_ty, span_sz("n"));
            argument_s = array_add(argument_s, arg0);

            span_t n = span_sz("n");
            expression_t expression = identifier_new(n);
            statement_t statement = return_new(expression);

            declaration_t num_fn = function_new(int_ty, name, argument_s, statement);

            declaration_s = array_add(declaration_s, num_fn);
        }

        unit_t unit = unit_new(source_inline(code_a001, ""), declaration_s);

        array_t(declaration_t) unit_decl_s = unit_declaration_s(unit);
        assert_int_equal(2, array_size(unit_decl_s));

        unit_free(unit);
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