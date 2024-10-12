#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "parser/lexer.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        assert_true(is_eof(lexer_alloc("")));
        assert_true(is_eof(lexer_alloc(" ")));
        assert_true(is_eof(lexer_alloc("\n")));
    }

    {
        assert_false(is_keyword_return(lexer_alloc("")));

        assert_true(is_keyword_int(lexer_alloc("int")));
        assert_true(is_keyword_return(lexer_alloc("return")));
    }

    {
        assert_true(is_identifier(lexer_alloc("one")));
        assert_false(is_identifier(lexer_alloc("int")));
    }

    {
        assert_true(is_open_paren(lexer_alloc("(")));
        assert_false(is_open_paren(lexer_alloc("")));
    }
    {
        assert_true(is_close_paren(lexer_alloc(")")));
        assert_false(is_close_paren(lexer_alloc("")));
    }

    {
        assert_true(is_semicolon(lexer_alloc(";")));
        assert_false(is_semicolon(lexer_alloc("")));
    }

    {
        assert_true(is_colon(lexer_alloc(",")));
        assert_false(is_colon(lexer_alloc("")));
    }

    {
        assert_false(is_integer_literal(lexer_alloc("")));

        char *code = "123";
        lexer_t lexer = lexer_alloc(code);
        assert_true(is_integer_literal(lexer));
        assert_int_equal(code, lexer->span.data);
        assert_int_equal(strlen(code), lexer->span.size);
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