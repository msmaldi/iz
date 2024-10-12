#include "ast/unit.h"
#include "common/mem.h"

struct unit_t
{
    char *code;
    array_t(declaration_t) declaration_s;
};

unit_t unit_new(char *code, array_t(declaration_t) declaration_s)
{
    unit_t unit = mem_alloc(sizeof(struct unit_t));
    unit->code = code;
    unit->declaration_s = declaration_s;

    return unit;
}

void unit_free(unit_t unit)
{
    array_cleanup_free(unit->declaration_s, (release_t)declaration_free);
    mem_free(unit);
}

array_t(declaration_t) unit_declaration_s(unit_t unit)
{
    return unit->declaration_s;
}
