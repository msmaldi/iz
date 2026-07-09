#ifndef _LEXER_H_
#define _LEXER_H_

#include "common/source.h"

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

    TOKEN_AND_AND,
    TOKEN_OR_OR,
};

typedef enum keyword_t keyword_t;
enum keyword_t
{
    KEYWORD_BOOL,
    KEYWORD_INT,
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_CHAR,
};

typedef char* cursor_t;

typedef struct lexer_t* lexer_t;
struct lexer_t
{
    source_t   source;
    cursor_t   cursor;
    cursor_t   line_start;
    int        offset;
    int        line;
    int        column;
    struct location_t location;
    span_t     span;
    char       char_value;
};

struct lexer_t lexer_ctor(source_t source);
#define lexer_alloc(source) ((struct lexer_t[]){ lexer_ctor(source) })

bool is_eof(lexer_t lexer);

bool is_keyword_bool(lexer_t lexer);
bool is_keyword_int(lexer_t lexer);
bool is_keyword_return(lexer_t lexer);
bool is_keyword_if(lexer_t lexer);
bool is_keyword_else(lexer_t lexer);
bool is_keyword_true(lexer_t lexer);
bool is_keyword_false(lexer_t lexer);
bool is_keyword_char(lexer_t lexer);

bool is_identifier(lexer_t lexer);

bool is_comma(lexer_t lexer);
bool is_semicolon(lexer_t lexer);
bool is_open_paren(lexer_t lexer);
bool is_close_paren(lexer_t lexer);
bool is_open_brace(lexer_t lexer);
bool is_close_brace(lexer_t lexer);
bool is_eq(lexer_t lexer);

bool is_integer_literal(lexer_t lexer);
bool is_char_literal(lexer_t lexer);

token_kind_t match_assignment(lexer_t lexer);
token_kind_t match_equality(lexer_t lexer);
token_kind_t match_comparison(lexer_t lexer);
token_kind_t match_additive(lexer_t lexer);
token_kind_t match_multiplicative(lexer_t lexer);

token_kind_t match_conditional(lexer_t lexer);

#endif
