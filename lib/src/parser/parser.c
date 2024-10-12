#include "parser/parser.h"
#include "common/mem.h"

#include <ast/ast.h>
#include <stdlib.h>

parser_t parser_new(const char *code)
{
    parser_t parser = mem_alloc(sizeof(struct parser_t));
    parser->lexer = lexer_ctor((char *)code);

    return parser;
}

void parser_free(parser_t parser)
{
    mem_free(parser);
}

static inline
lexer_t lexer(parser_t parser)
{
    return &parser->lexer;
}

unit_t parse(const char *code)
{
    parser_t parser = parser_new(code);

    unit_t unit = scan_unit(parser);
    parser_free(parser);

    return unit;
}

unit_t scan_unit(parser_t parser)
{
    array_t(declaration_t) declaration_s = array_empty();
    while (!is_eof(lexer(parser)))
    {
        declaration_t declaration = scan_function(parser);
        if (declaration == NULL)
            goto cleanup_declaration_s;

        declaration_s = array_add(declaration_s, declaration);
    }

    return unit_new(lexer(parser)->code, declaration_s);

cleanup_declaration_s:
    array_cleanup_free(declaration_s, (release_t)declaration_free);
    return NULL;
}

expression_t scan_constant(parser_t parser)
{
    lexer_t lex = lexer(parser);
    if (is_integer_literal(lex))
    {
        char *end = NULL;
        uint64_t u64 = strtoull(lex->span.data, &end, 10);
        return constant_u64_new(u64);
    }

    return NULL;
}

expression_t scan_primary(parser_t parser)
{
    lexer_t lex = lexer(parser);
    if (is_identifier(lex))
        return identifier_new(lex->span);

    return NULL;
}

expression_t scan_expression(parser_t parser)
{
    expression_t constant = scan_constant(parser);
    if (constant != NULL)
        return constant;

    expression_t primary = scan_primary(parser);
    if (primary != NULL)
        return primary;

    return NULL;
}

statement_t finish_return(parser_t parser)
{
    expression_t expression = scan_expression(parser);
    if (expression == NULL)
        goto leave_null;

    if (!is_semicolon(lexer(parser)))
        goto leave_expression;

    return return_new(expression);

leave_expression:;
    expression_free(expression);
leave_null:;
    return NULL;
}

statement_t scan_statement(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_keyword_return(lex))
        return finish_return(parser);

    return NULL;
}

declaration_t scan_argument(parser_t parser)
{
    type_t ret_type = scan_type(parser);
    if (ret_type == NULL)
        goto leave_null;

    span_t identifier = scan_identifier(parser);
    if (identifier.data == NULL)
        goto leave_ret_type;

    return argument_new(ret_type, identifier);

leave_ret_type:;
    type_free(ret_type);
leave_null:;
    return NULL;
}

void argument_free(declaration_t declaration)
{
    argument_t argument = ARGUMENT(declaration);
    type_free(argument_type(argument));
    mem_free(declaration);
}

array_t(declaration_t) scan_argument_s(parser_t parser)
{
    array_t(declaration_t) argument_s = array_empty();

    if (!is_open_paren(lexer(parser)))
        goto leave_null;

    if (is_close_paren(lexer(parser)))
        return argument_s;

    while (true)
    {
        declaration_t argument = scan_argument(parser);
        if (argument == NULL)
            goto leave_null;

        argument_s = array_add(argument_s, argument);

        if (is_close_paren(lexer(parser)))
            break;

        if (!is_colon(lexer(parser)))
            goto leave_null;
    }

    return argument_s;

leave_null:;
    array_cleanup_free(argument_s, (release_t)argument_free);
    return NULL;
}

declaration_t scan_function(parser_t parser)
{
    type_t ret_type = scan_type(parser);
    if (ret_type == NULL)
        goto leave_null;

    span_t identifier = scan_identifier(parser);
    if (identifier.data == NULL)
        goto leave_ret_type;

    array_t(declaration_t) argument_s = scan_argument_s(parser);
    if (argument_s == NULL)
        goto leave_ret_type;

    statement_t statement = scan_statement(parser);
    if (statement == NULL)
        goto leave_ret_type;

    return function_new(ret_type, identifier, argument_s, statement);

leave_ret_type:;
    type_free(ret_type);
leave_null:;
    return NULL;
}

span_t scan_identifier(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_identifier(lex))
        return lex->span;

    return (span_t){ .data = NULL, .size = 0 };
}

type_t scan_type(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_keyword_int(lex))
        return type_int_new();

    return NULL;
}
