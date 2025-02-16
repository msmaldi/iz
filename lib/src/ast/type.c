#include "ast/type.h"
#include "common/mem.h"

struct callable_t
{
    type_t          return_type;
    array_t(type_t) param_s;
};

struct type_t
{
    type_kind_t kind;
    union
    {
        struct callable_t callable;
    };
};

struct type_t struct_type_bool = { .kind = TYPE_BOOL };
type_t type_bool()
{
    return &struct_type_bool;
}

struct type_t struct_type_int = { .kind = TYPE_INT };
type_t type_int()
{
    return &struct_type_int;
}

static
type_t type_new(type_kind_t kind)
{
    type_t type = mem_alloc(sizeof(struct type_t));

    type->kind = kind;

    return type;
}

type_t type_bool_new()
{
    type_t type = type_new(TYPE_BOOL);

    return type;
}

type_t type_int_new()
{
    type_t type = type_new(TYPE_INT);

    return type;
}

type_t type_callable_new(type_t return_type, array_t(type_t) param_s)
{
    type_t type = type_new(TYPE_CALLABLE);
    callable_t callable = CALLABLE(type);

    callable->return_type = return_type;
    callable->param_s = param_s;

    return type;
}

void callable_free(callable_t callable)
{
    type_free(callable->return_type);
    array_cleanup_free(callable->param_s, (release_t)type_free);
}

void type_free(type_t type)
{
    switch (type->kind) // LCOV_EXCL_LINE
    {
        case TYPE_BOOL:
        case TYPE_INT:
            break;
        case TYPE_CALLABLE:
            callable_free(CALLABLE(type));
            break;
    }
    mem_free(type);
}

static
type_t callable_clone(callable_t callable)
{
    array_t(type_t) param_s = array_empty();
    for (size_t i = 0; i < array_size(callable->param_s); ++i)
    {
        type_t param = type_clone(callable->param_s[i]);
        param_s = array_add(param_s, param);
    }

    return type_callable_new(type_clone(callable->return_type), param_s);
}

type_t type_clone(type_t type)
{
    switch (type->kind) // LCOV_EXCL_LINE
    {
        case TYPE_BOOL:
            return type_bool_new();
        case TYPE_INT:
            return type_int_new();
        case TYPE_CALLABLE:
            return callable_clone(CALLABLE(type));
    }

    __builtin_unreachable();
}

bool type_eq(type_t lhs, type_t rhs)
{
    if (lhs == NULL || rhs == NULL)
        return false;

    return lhs->kind == rhs->kind;
}

type_t callable_return_type(callable_t callable)
{
    return callable->return_type;
}

array_t(type_t) callable_param_s(callable_t callable)
{
    return callable->param_s;
}

type_kind_t type_kind(type_t type)
{
    return type->kind;
}

callable_t CALLABLE(type_t type)
{
    return &type->callable;
}

