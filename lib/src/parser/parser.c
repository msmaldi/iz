#include "parser/parser.h"
#include "common/mem.h"

#include <ast/ast.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct parser_t* parser_t;
struct parser_t
{
    struct lexer_t lexer;
    source_t       source;
};

static
void display_error(parser_t parser, const char *msg);

expression_t parse_primary(parser_t parser);
expression_t parse_expression(parser_t parser);
statement_t parse_statement(parser_t parser);
declaration_t parse_function(parser_t parser);
type_t parse_type(parser_t parser);
struct location_t parse_name(parser_t parser);
declaration_t parse_argument(parser_t parser);
unit_t parse_unit(parser_t parser);

array_t(expression_t) scan_expression_s(parser_t parser);

static inline
lexer_t lexer(parser_t parser)
{
    return &parser->lexer;
}

unit_t syntax_analysis(source_t source)
{
    struct parser_t parser = { .source = source, .lexer = lexer_ctor(source) };

    unit_t unit = parse_unit(&parser);

    return unit;
}

unit_t parse_unit(parser_t parser)
{
    array_t(declaration_t) declaration_s = array_empty();
    while (!is_eof(lexer(parser)))
    {
        declaration_t declaration = parse_function(parser);
        if (declaration == NULL)
            goto cleanup_declaration_s;

        declaration_s = array_add(declaration_s, declaration);
    }

    return unit_new(parser->source, declaration_s);

cleanup_declaration_s:
    array_cleanup_free(declaration_s, (release_t)declaration_free);
    return NULL;
}

statement_t scan_block(parser_t parser)
{
    array_t(statement_t) statements = array_empty();

    while (!is_close_brace(lexer(parser)))
    {
        statement_t statement = parse_statement(parser);
        if (statement == NULL)
            goto cleanup_statement_s;

        statements = array_add(statements, statement);
    }

    return block_new(statements);

cleanup_statement_s:
    array_cleanup_free(statements, (release_t)statement_free);
    return NULL;
}

statement_t finish_return(parser_t parser)
{
    expression_t expression = parse_expression(parser);
    if (expression == NULL)
        goto leave_null;

    if (!is_semicolon(lexer(parser)))
    {
        display_error(parser, "expected ';'");
        goto leave_expression;
    }

    return return_new(expression);

leave_expression:;
    expression_free(expression);
leave_null:;
    return NULL;
}

statement_t finish_if(parser_t parser)
{
    if (!is_open_paren(lexer(parser)))
    {
        display_error(parser, "expected '('");
        goto leave_null;
    }

    expression_t condition = parse_expression(parser);
    if (condition == NULL)
        goto leave_null;

    if (!is_close_paren(lexer(parser)))
    {
        display_error(parser, "expected ')'");
        goto leave_condition;
    }

    statement_t then_branch = parse_statement(parser);
    if (then_branch == NULL)
        goto leave_condition;

    statement_t else_branch = NULL;
    if (is_keyword_else(lexer(parser)))
    {
        else_branch = parse_statement(parser);
        if (else_branch == NULL)
            goto leave_then_branch;
    }

    return if_new(condition, then_branch, else_branch);

leave_then_branch:;
    statement_free(then_branch);
leave_condition:;
    expression_free(condition);
leave_null:;
    return NULL;
}

static
statement_t scan_act(parser_t parser, bool require_semicolon)
{
    array_t(expression_t) expression_s = scan_expression_s(parser);
    if (expression_s == NULL)
        goto leave_null;

    if (require_semicolon && !is_semicolon(lexer(parser))) // LCOV_EXCL_BR_LINE (only caller always passes true)
    {
        display_error(parser, "expected ';'");
        goto leave_expression_s;
    }

    return act_new(expression_s);

leave_expression_s:;
    array_cleanup_free(expression_s, (release_t)expression_free);
leave_null:;
    return NULL;
}

static
statement_t scan_var_or_expression_s(parser_t parser)
{
    type_t type = parse_type(parser);
    if (type == NULL)
        return scan_act(parser, true);

    array_t(declaration_t) variable_s = array_empty();
    while (true)
    {
        struct location_t identifier = parse_name(parser);
        if (identifier.span.data == NULL)
        {
            display_error(parser, "expected identifier");
            goto leave_variable_s;
        }

        expression_t initializer = NULL;
        if (is_eq(lexer(parser)))
        {
            initializer = parse_expression(parser);
            if (initializer == NULL)
                goto leave_variable_s;
        }

        declaration_t variable = variable_new(type_clone(type), identifier, initializer);
        variable_s = array_add(variable_s, variable);

        if (is_semicolon(lexer(parser)))
        {
            type_free(type);
            return var_new(variable_s);
        }

        if (!is_comma(lexer(parser)))
        {
            display_error(parser, "expected ',' or ';'");
            goto leave_variable_s;
        }
    }

leave_variable_s:
    array_cleanup_free(variable_s, (release_t)declaration_free);
    type_free(type);
    return NULL;
}

statement_t parse_statement(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_open_brace(lex))
        return scan_block(parser);

    if (is_keyword_return(lex))
        return finish_return(parser);

    if (is_keyword_if(lex))
        return finish_if(parser);

    return scan_var_or_expression_s(parser);
}

declaration_t parse_argument(parser_t parser)
{
    type_t ret_type = parse_type(parser);
    if (ret_type == NULL)
    {
        display_error(parser, "expected type");
        goto leave_null;
    }

    struct location_t identifier = parse_name(parser);
    if (identifier.span.data == NULL)
    {
        display_error(parser, "expected identifier");
        goto leave_ret_type;
    }

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
    {
        display_error(parser, "expected '('");
        goto leave_null;
    }

    if (is_close_paren(lexer(parser)))
        return argument_s;

    while (true)
    {
        declaration_t argument = parse_argument(parser);
        if (argument == NULL)
            goto leave_null;

        argument_s = array_add(argument_s, argument);

        if (is_close_paren(lexer(parser)))
            break;

        if (!is_comma(lexer(parser)))
        {
            display_error(parser, "expected ',' or ')'");
            goto leave_null;
        }
    }

    return argument_s;

leave_null:;
    array_cleanup_free(argument_s, (release_t)argument_free);
    return NULL;
}

declaration_t parse_function(parser_t parser)
{
    type_t ret_type = parse_type(parser);
    if (ret_type == NULL)
    {
        display_error(parser, "expected type");
        goto leave_null;
    }

    struct location_t identifier = parse_name(parser);
    if (identifier.span.data == NULL)
    {
        display_error(parser, "expected identifier");
        goto leave_ret_type;
    }

    array_t(declaration_t) argument_s = scan_argument_s(parser);
    if (argument_s == NULL)
        goto leave_ret_type;

    statement_t statement = parse_statement(parser);
    if (statement == NULL)
        goto leave_argument_s;

    return function_new(ret_type, identifier, argument_s, statement);

leave_argument_s:;
    array_cleanup_free(argument_s, (release_t)argument_free);
leave_ret_type:;
    type_free(ret_type);
leave_null:;
    return NULL;
}

struct location_t parse_name(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_identifier(lex))
        return (struct location_t){ .span = lex->span, .source = parser->source, .line = lex->line, .column = lex->column };

    return (struct location_t){ .span = { .data = NULL, .size = 0 } };
}

type_t parse_type(parser_t parser)
{
    lexer_t lex = lexer(parser);

    type_t type = NULL;

    if (is_keyword_bool(lex))
        type = type_bool_new();
    else if (is_keyword_int(lex))
        type = type_int_new();
    else if (is_keyword_char(lex))
        type = type_char_new();
    else if (is_keyword_void(lex))
        type = type_void_new();

    if (type == NULL)
        return NULL;

    while (is_star(lex))
        type = type_pointer_new(type);

    return type;
}

array_t(expression_t) scan_expression_s(parser_t parser)
{
    array_t(expression_t) expression_s = array_empty();

    do
    {
        expression_t expression = parse_expression(parser);
        if (expression == NULL)
            goto leave_null;

        expression_s = array_add(expression_s, expression);
    }
    while (is_comma(lexer(parser)));

    return expression_s;
leave_null:;
    array_cleanup_free(expression_s, (release_t)expression_free);
    return NULL;
}

expression_t scan_constant(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_keyword_true(lex))
        return constant_bool_new(true);
    if (is_keyword_false(lex))
        return constant_bool_new(false);

    if (is_integer_literal(lex))
    {
        char *end = NULL;
        uint64_t u64 = strtoull(lex->span.data, &end, 10);
        return constant_u64_new(u64);
    }

    if (is_char_literal(lex))
        return constant_char_new(lex->char_value);

    return NULL;
}

expression_t finish_call(parser_t parser, expression_t callee)
{
    if (is_close_paren(lexer(parser)))
        return call_new(callee, array_empty());

    array_t(expression_t) argument_s = scan_expression_s(parser);
    if (argument_s == NULL)
        goto leave_null;

    if (!is_close_paren(lexer(parser)))
    {
        display_error(parser, "expected ')'");
        goto leave_argument_s;
    }

    return call_new(callee, argument_s);

leave_argument_s:;
    array_cleanup_free(argument_s, (release_t)expression_free);
leave_null:;
    expression_free(callee);
    return NULL;
}

expression_t finish_assignment(parser_t parser, expression_t lvalue)
{
    expression_t rvalue = parse_expression(parser);
    if (rvalue == NULL)
        goto leave_lvalue;

    return assignment_new(lvalue, rvalue);

leave_lvalue:;
    expression_free(lvalue);
    return NULL;
}

expression_t suffix(parser_t parser, expression_t expression)
{
    lexer_t lex = lexer(parser);
    if (is_open_paren(lex))
    {
        expression = finish_call(parser, expression);
        return suffix(parser, expression);
    }

    return expression;
}

expression_t scan_identifier(parser_t parser)
{
    lexer_t lex = lexer(parser);
    if (!is_identifier(lex))
    {
        display_error(parser, "expected expression");
        return NULL;
    }

    struct location_t location = { .span = lex->span, .source = parser->source, .line = lex->line, .column = lex->column };
    expression_t identifier = identifier_new(location);
    return suffix(parser, identifier);
}

expression_t parse_primary(parser_t parser)
{
    expression_t constant = scan_constant(parser);
    if (constant != NULL)
        return constant;

    expression_t identifier = scan_identifier(parser);
    if (identifier != NULL)
        return identifier;

    return NULL;
}

expression_t parse_unary(parser_t parser)
{
    lexer_t lex = lexer(parser);

    if (is_star(lex))
    {
        expression_t operand = parse_unary(parser);
        if (operand == NULL)
            return NULL;
        return unary_new(UNARY_DEREF, operand);
    }

    if (is_amp(lex))
    {
        expression_t operand = parse_unary(parser);
        if (operand == NULL)
            return NULL;
        return unary_new(UNARY_ADDRESS_OF, operand);
    }

    return parse_primary(parser);
}

expression_t scan_multiplicative(parser_t parser)
{
    expression_t lhs = parse_unary(parser);
    while (true)
    {
        token_kind_t token_kind = match_multiplicative(lexer(parser));
        switch (token_kind)
        {
            case TOKEN_STAR:
            {
                expression_t rhs = parse_unary(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_MUL, rhs);
                break;
            }
            case TOKEN_SLASH:
            {
                expression_t rhs = parse_unary(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_DIV, rhs);
                break;
            }
            case TOKEN_PERCENT:
            {
                expression_t rhs = parse_unary(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_REM, rhs);
                break;
            }
            default:
                return lhs;
        }
    }
}

expression_t scan_additive(parser_t parser)
{
    expression_t lhs = scan_multiplicative(parser);
    while (true)
    {
        token_kind_t token_kind = match_additive(lexer(parser));
        switch (token_kind)
        {
            case TOKEN_PLUS:
            {
                expression_t rhs = scan_multiplicative(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_ADD, rhs);
                break;
            }
            case TOKEN_MINUS:
            {
                expression_t rhs = scan_multiplicative(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_SUB, rhs);
                break;
            }
            default:
                return lhs;
        }
    }
}

expression_t scan_comparition(parser_t parser)
{
    expression_t lhs = scan_additive(parser);
    while (true)
    {
        token_kind_t token_kind = match_comparison(lexer(parser));
        switch (token_kind)
        {
            case TOKEN_LT:
            {
                expression_t rhs = scan_additive(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_LT, rhs);
                break;
            }
            case TOKEN_GT:
            {
                expression_t rhs = scan_additive(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_GT, rhs);
                break;
            }
            case TOKEN_LT_EQ:
            {
                expression_t rhs = scan_additive(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_LE, rhs);
                break;
            }
            case TOKEN_GT_EQ:
            {
                expression_t rhs = scan_additive(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_GE, rhs);
                break;
            }
            default:
                return lhs;
        }
    }
}

expression_t scan_equality(parser_t parser)
{
    expression_t lhs = scan_comparition(parser);
    while (true)
    {
        token_kind_t token_kind = match_equality(lexer(parser));
        switch (token_kind)
        {
            case TOKEN_EQ_EQ:
            {
                expression_t rhs = scan_comparition(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_EQ, rhs);
                break;
            }
            case TOKEN_NO_EQ:
            {
                expression_t rhs = scan_comparition(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = binary_new(lhs, BINARY_NE, rhs);
                break;
            }
            default:
                return lhs;
        }
    }
}

expression_t scan_conditional(parser_t parser)
{
    expression_t lhs = scan_equality(parser);
    while (true)
    {
        token_kind_t token_kind = match_conditional(lexer(parser));
        switch (token_kind)
        {
            case TOKEN_AND_AND:
            {
                expression_t rhs = scan_equality(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = conditional_new(lhs, CONDITIONAL_AND, rhs);
                break;
            }
            case TOKEN_OR_OR:
            {
                expression_t rhs = scan_equality(parser);
                if (rhs == NULL) { expression_free(lhs); return NULL; }
                lhs = conditional_new(lhs, CONDITIONAL_OR, rhs);
                break;
            }
            default:
                return lhs;
        }
    }
}

expression_t parse_expression(parser_t parser)
{
    expression_t lhs = scan_conditional(parser);
    if (lhs == NULL)
        return NULL;

    if (is_eq(lexer(parser)))
        return finish_assignment(parser, lhs);

    return lhs;
}

#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

static
int get_line_size(cursor_t line_start)
{
    int size = 0;
    while (line_start[size] != '\n' && line_start[size] != '\0')
        size++;

    return size;
}

static
void display_error(parser_t parser, const char *msg)
{
    lexer_t lex = lexer(parser);
    fprintf(stderr, BOLDWHITE "%s:%d:%d: ", source_path(parser->source), lex->line + 1, lex->column + 1);
    fprintf(stderr, BOLDRED "error: " BOLDWHITE "%s\n" RESET, msg);

    char line_counter[20] = { 0 };
    int line_counter_len = snprintf(line_counter, sizeof(line_counter), "%5u | ", lex->line + 1);
    fprintf(stderr, "%s", line_counter);

    int line_size = get_line_size(lex->line_start);
    fprintf(stderr, "%.*s\n", line_size, lex->line_start);

    int size = lex->cursor - lex->line_start + line_counter_len;
    for (size_t i = 0; i < size; i++)
        fputc(' ', stderr);
    fprintf(stderr, BOLDGREEN "^\n" RESET);
}
