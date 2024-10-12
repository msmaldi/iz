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
    TOKEN_KW_INT,
};

typedef enum keyword_t keyword_t;
enum keyword_t
{
    KEYWORD_INT,
    KEYWORD_RETURN,
};

typedef char* cursor_t;

typedef struct lexer_t* lexer_t;
struct lexer_t
{
    char*    code;
    cursor_t cursor;
    size_t   line;
    size_t   collumn;
    size_t   offset;

    span_t   span;
};

struct lexer_t lexer_ctor(char *code);
#define lexer_alloc(code) ((struct lexer_t[]){ lexer_ctor(code) })

bool is_eof(lexer_t lexer);

bool is_keyword_int(lexer_t lexer);
bool is_keyword_return(lexer_t lexer);

bool is_identifier(lexer_t lexer);

bool is_colon(lexer_t lexer);
bool is_semicolon(lexer_t lexer);
bool is_open_paren(lexer_t lexer);
bool is_close_paren(lexer_t lexer);

bool is_integer_literal(lexer_t lexer);


#endif
