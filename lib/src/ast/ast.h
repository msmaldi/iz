#ifndef _AST_H_
#define _AST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common/array.h"
#include "common/span.h"
#include "common/source.h"

typedef  struct compilation_t*         compilation_t;
typedef  struct unit_t*                unit_t;

typedef  enum   type_kind_t            type_kind_t;
typedef  struct type_t*                type_t;
typedef  struct callable_t*            callable_t;

typedef  enum   declaration_kind_t     declaration_kind_t;
typedef  struct declaration_t*         declaration_t;
typedef  struct function_t*            function_t;
typedef  struct argument_t*            argument_t;
typedef  struct variable_t*            variable_t;


typedef  enum   statement_kind_t       statement_kind_t;
typedef  struct statement_t*           statement_t;
typedef  struct block_t*               block_t;
typedef  struct return_t*              return_t;
typedef  struct if_t*                  if_t;
typedef  struct var_t*                 var_t;
typedef  struct act_t*                 act_t;


typedef  enum   expression_kind_t      expression_kind_t;
typedef  struct expression_t*          expression_t;

typedef  enum   constant_kind_t        constant_kind_t;
typedef  struct constant_t*            constant_t;

typedef  struct identifier_t*          identifier_t;

typedef  enum   binary_kind_t          binary_kind_t;
typedef  struct binary_t*              binary_t;

typedef  struct call_t*                call_t;

typedef  struct assignment_t*          assignment_t;

typedef  enum   implicit_cast_kind_t   implicit_cast_kind_t;
typedef  struct implicit_cast_t*       implicit_cast_t;

typedef  enum   conditional_kind_t     conditional_kind_t;
typedef  struct conditional_t*         conditional_t;


#include "ast/expression.h"
#include "ast/statement.h"
#include "ast/type.h"
#include "ast/declaration.h"
#include "ast/unit.h"
#include "ast/compilation.h"


#endif
