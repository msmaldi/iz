#ifndef _TYPE_H_
#define _TYPE_H_

#include "ast/ast.h"

enum type_kind_t
{
    TYPE_INT,
    TYPE_CALLABLE,
};

type_t   type_int_new();
type_t   type_callable_new(type_t return_type, array_t(type_t) param_s);

void     type_free(type_t type);

type_kind_t type_kind(type_t type);

type_t           callable_return_type(callable_t callable);
array_t(type_t)  callable_param_s(callable_t callable);

callable_t CALLABLE(type_t type);

#endif
