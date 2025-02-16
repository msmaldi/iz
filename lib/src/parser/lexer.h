#ifndef _LEXER_H_
#define _LEXER_H_

#include <common/span.h>

#include <stdbool.h>
#include <stddef.h>

typedef enum token_kind_t token_kind_t;
enum token_kind_t
{
    TOKEN_UNEXPECTED,
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_LT,
    TOKEN_LT_EQ,
    TOKEN_GT,
    TOKEN_GT_EQ,
    TOKEN_EQ_EQ,
    TOKEN_NO_EQ,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_EQ,
};

typedef enum keyword_t keyword_t;
enum keyword_t
{
    KEYWORD_BOOL,
    KEYWORD_INT,
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
};

typedef char* cursor_t;

typedef struct location* location_t;

struct location
{
    cursor_t line_start;
    int      line;
    span_t   span;
};


typedef struct lexer_t* lexer_t;
struct lexer_t
{
    cursor_t   cursor;
    cursor_t   line_start;
    int        line;
    int        column;
    int        offset;
    span_t     span;
};

struct lexer_t lexer_ctor(char *code);
#define lexer_alloc(code) ((struct lexer_t[]){ lexer_ctor(code) })

bool is_eof(lexer_t lexer);

bool is_keyword_bool(lexer_t lexer);
bool is_keyword_int(lexer_t lexer);
bool is_keyword_return(lexer_t lexer);
bool is_keyword_if(lexer_t lexer);
bool is_keyword_else(lexer_t lexer);

bool is_identifier(lexer_t lexer);

bool is_comma(lexer_t lexer);
bool is_semicolon(lexer_t lexer);
bool is_open_paren(lexer_t lexer);
bool is_close_paren(lexer_t lexer);
bool is_open_brace(lexer_t lexer);
bool is_close_brace(lexer_t lexer);
bool is_eq(lexer_t lexer);

bool is_integer_literal(lexer_t lexer);

token_kind_t match_assignment(lexer_t lexer);
token_kind_t match_equality(lexer_t lexer);
token_kind_t match_comparison(lexer_t lexer);
token_kind_t match_additive(lexer_t lexer);
token_kind_t match_multiplicative(lexer_t lexer);

#endif
