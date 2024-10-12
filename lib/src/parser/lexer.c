#include "parser/lexer.h"

#include <ctype.h>

#define keywords_len 2
const char *keywords[keywords_len] = { "int", "return" };

struct lexer_t lexer_ctor(char *code)
{
    return (struct lexer_t)
    {
        .code = code,
        .cursor = code,
        .line = 1,
        .collumn = 1,
        .offset = 0
    };
}

static inline
void advance(lexer_t lexer, const size_t size)
{
    lexer->cursor += size;
    lexer->collumn += size;
    lexer->offset += size;
}

static inline
void newline(lexer_t lexer)
{
    lexer->cursor++;
    lexer->collumn = 1;
    lexer->offset++;
    lexer->line++;
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

bool is_keyword_int(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_INT);
}

bool is_keyword_return(lexer_t lexer)
{
    return is_keyword(lexer, KEYWORD_RETURN);
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

bool is_semicolon(lexer_t lexer)
{
    if (skip_whitespaces(lexer) != ';')
        return false;

    advance(lexer, 1);
    return true;
}

bool is_colon(lexer_t lexer)
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
