#ifndef _COMPILATION_UNIT_H_
#define _COMPILATION_UNIT_H_

#include "ast/ast.h"

compilation_t   compilation_new(array_t(unit_t) unit_s);
void            compilation_free(compilation_t compilation);

array_t(unit_t) compilation_unit_s(compilation_t compilation);

#endif
