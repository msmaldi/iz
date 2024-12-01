#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast/ast.h"
#include "parser/lexer.h"
#include "common/source.h"

unit_t syntax_analysis(source_t source);

#endif
