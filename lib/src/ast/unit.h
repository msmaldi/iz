#ifndef _UNIT_H_
#define _UNIT_H_

#include "ast/ast.h"

unit_t unit_new(source_t source, array_t(declaration_t) declaration_s);
void   unit_free(unit_t unit);

array_t(declaration_t)  unit_declaration_s(unit_t unit);
source_t                unit_source(unit_t unit);

#endif
