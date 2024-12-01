#include "parser/lexer.h"

#include <ctype.h>

#define keywords_len 5
const char *keywords[keywords_len] = { "bool", "int", "return", "if", "else" };

struct lexer_t lexer_ctor(char *code)
{
    return (struct lexer_t)
    {
        .cursor = code,
        .line_start = code,
        .line = 0,
        .column = 0,
        .offset = 0,
        .span = { .data = NULL, .size = 0 }
    };
}

static inline
void advance(lexer_t lexer, const size_t size)
{
    lexer->cursor += size;
    lexer->column += size;
    lexer->offset += size;
}

static inline
void newline(lexer_t lexer)
{
    lexer->cursor++;
    lexer->column = 0;
    lexer->offset++;
    lexer->line++;
    lexer->line_start = lexer->cursor;
}

static inline
char skip_whitespaces(lexer_t lexer)
{
    while (true)
    {
        char c = lexer->cursor[0];
        switch (c)
        {
            case ' ': advance(lexer, 1); break;
            case '\n': newline(lexer);   break;
        default:
            return c;
        }
    }
}

bool is_eof(lexer_t lexer)
{
    return skip_whitespaces(lexer) == '\0';
}

span_t keyword_or_identifier(lexer_t lexer)
{
    char c = skip_whitespaces(lexer);

    if (!isalpha(c))
        return (span_t){ .data = NULL, .size = 0 };

    size_t size = 1;
    while (isalnum(lexer->cursor[size]) || lexer->cursor[size] == '_')
        size++;

    return (span_t){ .data = lexer->cursor, .size = size };
}

bool is_keyword(lexer_t lexer, keyword_t keyword)
{
    span_t kw = keyword_or_identifier(lexer);

    if (!span_eq(kw, span_sz(keywords[keyword])))
        return false;

    advance(lexer, kw.size);
    return true;
}

bool is_keyword_bool(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_BOOL);
}

bool is_keyword_int(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_INT);
}

bool is_keyword_return(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_RETURN);
}

bool is_keyword_if(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_IF);
}

bool is_keyword_else(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_ELSE);
}

bool is_identifier(lexer_t lexer)
{
    lexer->span.data = NULL;
    lexer->span.size = 0;

    span_t span = keyword_or_identifier(lexer);

    if (span.data == NULL)
        return false;

    for (int i = 0; i < keywords_len; i++)
        if (span_eq(span, span_sz(keywords[i])))
            return false;

    advance(lexer, span.size);
    lexer->span = span;
    return true;
}

bool is_open_paren(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != '(')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_close_paren(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != ')')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_open_brace(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != '{')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_close_brace(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != '}')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_semicolon(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != ';')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_comma(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != ',')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_integer_literal(lexer_t lexer)
{
    char c = skip_whitespaces(lexer);

    if (!isdigit(c))
        return false;

    char *data = lexer->cursor;
    size_t size = 1;
    while (isdigit(data[size]))
        size++;

    lexer->span.data = data;
    lexer->span.size = size;

    advance(lexer, size);
    return true;
}

token_kind_t match_found(lexer_t lexer, token_kind_t token_kind, char *data, size_t size)
{
    lexer->span.data = data;
    lexer->span.size = size;
    advance(lexer, size);
    return token_kind;
}

token_kind_t match_equality(lexer_t lexer)
{
    skip_whitespaces(lexer);
    char *cursor = lexer->cursor;

    if (cursor[0] == '=' && cursor[1] == '=')
        return match_found(lexer, TOKEN_EQ_EQ, cursor, 2);

    if (cursor[0] == '!' && cursor[1] == '=')
        return match_found(lexer, TOKEN_NO_EQ, cursor, 2);

    return TOKEN_UNEXPECTED;
}

token_kind_t match_comparison(lexer_t lexer)
{
    skip_whitespaces(lexer);
    char *cursor = lexer->cursor;

    if (cursor[0] == '<' && cursor[1] == '=')
        return match_found(lexer, TOKEN_LT_EQ, cursor, 2);

    if (cursor[0] == '>' && cursor[1] == '=')
        return match_found(lexer, TOKEN_GT_EQ, cursor, 2);

    if (cursor[0] == '<')
        return match_found(lexer, TOKEN_LT, cursor, 1);

    if (cursor[0] == '>')
        return match_found(lexer, TOKEN_GT, cursor, 1);

    return TOKEN_UNEXPECTED;
}

token_kind_t match_additive(lexer_t lexer)
{
    skip_whitespaces(lexer);
    char *cursor = lexer->cursor;

    if (cursor[0] == '+')
        return match_found(lexer, TOKEN_PLUS, cursor, 1);

    if (cursor[0] == '-')
        return match_found(lexer, TOKEN_MINUS, cursor, 1);

    return TOKEN_UNEXPECTED;
}

token_kind_t match_multiplicative(lexer_t lexer)
{
    skip_whitespaces(lexer);
    char *cursor = lexer->cursor;

    if (cursor[0] == '*')
        return match_found(lexer, TOKEN_STAR, cursor, 1);

    if (cursor[0] == '/')
        return match_found(lexer, TOKEN_SLASH, cursor, 1);

    if (cursor[0] == '%')
        return match_found(lexer, TOKEN_PERCENT, cursor, 1);

    return TOKEN_UNEXPECTED;
}
