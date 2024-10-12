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

static
type_t type_new(type_kind_t kind)
{
    type_t type = mem_alloc(sizeof(struct type_t));

    type->kind = kind;

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
    switch (type->kind)
    {
        case TYPE_CALLABLE:
            callable_free(CALLABLE(type));
            break;
        default:
            break;
    }
    mem_free(type);
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

