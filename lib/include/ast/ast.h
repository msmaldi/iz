#ifndef _AST_H_
#define _AST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common/array.h"

typedef  struct unit_t*                unit_t;

typedef  enum   type_kind_t            type_kind_t;
typedef  struct type_t*                type_t;
typedef  struct callable_t*            callable_t;

typedef  enum   declaration_kind_t     declaration_kind_t;
typedef  struct declaration_t*         declaration_t;
typedef  struct function_t*            function_t;
typedef  struct argument_t*            argument_t;


typedef  enum   statement_kind_t       statement_kind_t;
typedef  struct statement_t*           statement_t;
typedef  struct return_t*              return_t;


typedef  enum   expression_kind_t      expression_kind_t;
typedef  struct expression_t*          expression_t;

typedef  enum   constant_kind_t        constant_kind_t;
typedef  struct constant_t*            constant_t;

typedef  struct identifier_t*          identifier_t;

#include "ast/expression.h"
#include "ast/statement.h"
#include "ast/type.h"
#include "ast/declaration.h"
#include "ast/unit.h"

#endif
