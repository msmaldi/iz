#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "parser/lexer.h"

#define SOURCE_LEXER_ALLOC(code) lexer_alloc(source_inline(code, ""))

static void test_data(void **arg)
{
    (void) arg;

    {
        assert_true(is_eof(SOURCE_LEXER_ALLOC("")));
        assert_true(is_eof(SOURCE_LEXER_ALLOC(" ")));
        assert_true(is_eof(SOURCE_LEXER_ALLOC("\n")));
    }

    {
        assert_false(is_keyword_return(SOURCE_LEXER_ALLOC("")));

        assert_true(is_keyword_bool(SOURCE_LEXER_ALLOC("bool")));
        assert_true(is_keyword_int(SOURCE_LEXER_ALLOC("int")));
        assert_true(is_keyword_return(SOURCE_LEXER_ALLOC("return")));
        assert_true(is_keyword_if(SOURCE_LEXER_ALLOC("if")));
        assert_true(is_keyword_else(SOURCE_LEXER_ALLOC("else")));
        assert_true(is_keyword_true(SOURCE_LEXER_ALLOC("true")));
        assert_true(is_keyword_false(SOURCE_LEXER_ALLOC("false")));
    }

    {
        assert_true(is_identifier(SOURCE_LEXER_ALLOC("one")));
        assert_false(is_identifier(SOURCE_LEXER_ALLOC("int")));
    }

    {
        assert_true(is_open_paren(SOURCE_LEXER_ALLOC("(")));
        assert_true(is_close_paren(SOURCE_LEXER_ALLOC(")")));
        assert_true(is_open_brace(SOURCE_LEXER_ALLOC("{")));
        assert_true(is_close_brace(SOURCE_LEXER_ALLOC("}")));
        assert_true(is_eq(SOURCE_LEXER_ALLOC("=")));
    }
    {
        assert_false(is_open_paren(SOURCE_LEXER_ALLOC("")));
        assert_false(is_close_paren(SOURCE_LEXER_ALLOC("")));
        assert_false(is_open_brace(SOURCE_LEXER_ALLOC("")));
        assert_false(is_close_brace(SOURCE_LEXER_ALLOC("")));
        assert_false(is_eq(SOURCE_LEXER_ALLOC("")));
        assert_false(is_eq(SOURCE_LEXER_ALLOC("==")));

    }

    {
        assert_true(is_semicolon(SOURCE_LEXER_ALLOC(";")));
        assert_false(is_semicolon(SOURCE_LEXER_ALLOC("")));
    }

    {
        assert_true(is_comma(SOURCE_LEXER_ALLOC(",")));
        assert_false(is_comma(SOURCE_LEXER_ALLOC("")));
    }

    {
        assert_false(is_integer_literal(SOURCE_LEXER_ALLOC("")));

        char *code = "123";
        lexer_t lexer = SOURCE_LEXER_ALLOC(code);
        assert_true(is_integer_literal(lexer));
        assert_int_equal(code, lexer->span.data);
        assert_int_equal(strlen(code), lexer->span.size);
    }

    {
        assert_int_equal(TOKEN_LT, match_comparison(SOURCE_LEXER_ALLOC("<")));
        assert_int_equal(TOKEN_GT, match_comparison(SOURCE_LEXER_ALLOC(">")));
        assert_int_equal(TOKEN_LT_EQ, match_comparison(SOURCE_LEXER_ALLOC("<=")));
        assert_int_equal(TOKEN_GT_EQ, match_comparison(SOURCE_LEXER_ALLOC(">=")));

        assert_int_equal(TOKEN_EQ_EQ, match_equality(SOURCE_LEXER_ALLOC("==")));
        assert_int_equal(TOKEN_NO_EQ, match_equality(SOURCE_LEXER_ALLOC("!=")));

        assert_int_equal(TOKEN_PLUS, match_additive(SOURCE_LEXER_ALLOC("+")));
        assert_int_equal(TOKEN_MINUS, match_additive(SOURCE_LEXER_ALLOC("-")));

        assert_int_equal(TOKEN_STAR, match_multiplicative(SOURCE_LEXER_ALLOC("*")));
        assert_int_equal(TOKEN_SLASH, match_multiplicative(SOURCE_LEXER_ALLOC("/")));
        assert_int_equal(TOKEN_PERCENT, match_multiplicative(SOURCE_LEXER_ALLOC("%")));

        assert_int_equal(TOKEN_EQ, match_assignment(SOURCE_LEXER_ALLOC("=")));

        assert_int_equal(TOKEN_AND_AND, match_conditional(SOURCE_LEXER_ALLOC("&&")));
        assert_int_equal(TOKEN_OR_OR, match_conditional(SOURCE_LEXER_ALLOC("||")));

        assert_int_equal(TOKEN_UNEXPECTED, match_equality(SOURCE_LEXER_ALLOC("=")));
        assert_int_equal(TOKEN_UNEXPECTED, match_equality(SOURCE_LEXER_ALLOC("!")));
        assert_int_equal(TOKEN_UNEXPECTED, match_equality(SOURCE_LEXER_ALLOC("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_comparison(SOURCE_LEXER_ALLOC("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_additive(SOURCE_LEXER_ALLOC("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_assignment(SOURCE_LEXER_ALLOC("")));
        assert_int_equal(TOKEN_UNEXPECTED, match_assignment(SOURCE_LEXER_ALLOC("==")));
        assert_int_equal(TOKEN_UNEXPECTED, match_conditional(SOURCE_LEXER_ALLOC("&")));
        assert_int_equal(TOKEN_UNEXPECTED, match_conditional(SOURCE_LEXER_ALLOC("|")));
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