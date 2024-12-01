#include "sema/scope.h"
#include "common/mem.h"

struct scope_t
{
    scope_t parent;
    array_t(declaration_t) declaration_s;
};

scope_t scope_new(scope_t parent)
{
    scope_t scope = mem_alloc(sizeof(struct scope_t));
    scope->parent = parent;
    scope->declaration_s = array_empty();

    return scope;
}

void scope_free(scope_t scope)
{
    array_free(scope->declaration_s);
    mem_free(scope);
}

static
declaration_t find(array_t(declaration_t) declaration_s, span_t name)
{
    size_t size = array_size(declaration_s);
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = declaration_s[i];
        span_t decl_name = declaration_name(declaration);
        if (span_eq(decl_name, name))
            return declaration;
    }
    return NULL;
}

bool scope_add(scope_t scope, declaration_t declaration)
{
    if (find(scope->declaration_s, declaration_name(declaration)))
        return false;

    scope->declaration_s = array_add(scope->declaration_s, declaration);

    return true;
}

declaration_t scope_find(scope_t scope, span_t name)
{
    if (scope == NULL)
        return NULL;

    declaration_t declaration = find(scope->declaration_s, name);
    if (declaration != NULL)
       return declaration;

    return scope_find(scope->parent, name);
}

scope_t scope_parent(scope_t scope)
{
    return scope->parent;
}
