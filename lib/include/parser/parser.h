#ifndef _PARSER_H_
#define _PARSER_H_

#include <ast/ast.h>
#include <parser/lexer.h>

typedef struct parser_t* parser_t;
struct parser_t
{
    struct lexer_t lexer;
};

parser_t parser_new(const char *code);
void parser_free(parser_t parser);

expression_t scan_primary(parser_t parser);
expression_t scan_expression(parser_t parser);
statement_t scan_statement(parser_t parser);
declaration_t scan_function(parser_t parser);
type_t scan_type(parser_t parser);
span_t scan_identifier(parser_t parser);
declaration_t scan_argument(parser_t parser);

unit_t scan_unit(parser_t parser);

unit_t parse(const char *code);

#endif