#include "ast/compilation.h"
#include "common/mem.h"

extern void unit_free(unit_t unit);

struct compilation_t
{
    array_t(unit_t) unit_s;
};

compilation_t compilation_new(array_t(unit_t) unit_s)
{
    compilation_t compilation = mem_alloc(sizeof(struct compilation_t));
    compilation->unit_s = unit_s;
    return compilation;
}

void compilation_free(compilation_t compilation)
{
    array_cleanup_free(compilation->unit_s, (release_t)unit_free);
    mem_free(compilation);
}

array_t(unit_t) compilation_unit_s(compilation_t compilation)
{
    return compilation->unit_s;
}
