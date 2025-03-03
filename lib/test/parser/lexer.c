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

        assert_true(is_keyword_bool(lexer_alloc("bool")));
        assert_true(is_keyword_int(lexer_alloc("int")));
        assert_true(is_keyword_return(lexer_alloc("return")));
        assert_true(is_keyword_if(lexer_alloc("if")));
        assert_true(is_keyword_else(lexer_alloc("else")));
        assert_true(is_keyword_true(lexer_alloc("true")));
        assert_true(is_keyword_false(lexer_alloc("false")));
    }

    {
        assert_true(is_identifier(lexer_alloc("one")));
        assert_false(is_identifier(lexer_alloc("int")));
    }

    {
        assert_true(is_open_paren(lexer_alloc("(")));
        assert_true(is_close_paren(lexer_alloc(")")));
        assert_true(is_open_brace(lexer_alloc("{")));
        assert_true(is_close_brace(lexer_alloc("}")));
        assert_true(is_eq(lexer_alloc("=")));
    }
    {
        assert_false(is_open_paren(lexer_alloc("")));
        assert_false(is_close_paren(lexer_alloc("")));
        assert_false(is_open_brace(lexer_alloc("")));
        assert_false(is_close_brace(lexer_alloc("")));
        assert_false(is_eq(lexer_alloc("")));
        assert_false(is_eq(lexer_alloc("==")));

    }

    {
        assert_true(is_semicolon(lexer_alloc(";")));
        assert_false(is_semicolon(lexer_alloc("")));
    }

    {
        assert_true(is_comma(lexer_alloc(",")));
        assert_false(is_comma(lexer_alloc("")));
    }

    {
        assert_false(is_integer_literal(lexer_alloc("")));

        char *code = "123";
        lexer_t lexer = lexer_alloc(code);
        assert_true(is_integer_literal(lexer));
        assert_int_equal(code, lexer->span.data);
        assert_int_equal(strlen(code), lexer->span.size);
    }

    {
        assert_int_equal(TOKEN_LT, match_comparison(lexer_alloc("<")));
        assert_int_equal(TOKEN_GT, match_comparison(lexer_alloc(">")));
        assert_int_equal(TOKEN_LT_EQ, match_comparison(lexer_alloc("<=")));
        assert_int_equal(TOKEN_GT_EQ, match_comparison(lexer_alloc(">=")));

        assert_int_equal(TOKEN_EQ_EQ, match_equality(lexer_alloc("==")));
        assert_int_equal(TOKEN_NO_EQ, match_equality(lexer_alloc("!=")));

        assert_int_equal(TOKEN_PLUS, match_additive(lexer_alloc("+")));
        assert_int_equal(TOKEN_MINUS, match_additive(lexer_alloc("-")));

        assert_int_equal(TOKEN_STAR, match_multiplicative(lexer_alloc("*")));
        assert_int_equal(TOKEN_SLASH, match_multiplicative(lexer_alloc("/")));
        assert_int_equal(TOKEN_PERCENT, match_multiplicative(lexer_alloc("%")));

        assert_int_equal(TOKEN_EQ, match_assignment(lexer_alloc("=")));

        assert_int_equal(TOKEN_AND_AND, match_conditional(lexer_alloc("&&")));
        assert_int_equal(TOKEN_OR_OR, match_conditional(lexer_alloc("||")));

        assert_int_equal(TOKEN_UNEXPECTED, match_equality(lexer_alloc("=")));
        assert_int_equal(TOKEN_UNEXPECTED, match_equality(lexer_alloc("!")));
        assert_int_equal(TOKEN_UNEXPECTED, match_equality(lexer_alloc("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_comparison(lexer_alloc("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_additive(lexer_alloc("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_assignment(lexer_alloc("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_assignment(lexer_alloc("==")));
        assert_int_equal(TOKEN_UNEXPECTED, match_conditional(lexer_alloc("&")));
        assert_int_equal(TOKEN_UNEXPECTED, match_conditional(lexer_alloc("|")));
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