#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "ast/declaration.h"

typedef  struct scope_t*  scope_t;

scope_t        scope_new(scope_t parent);
void           scope_free(scope_t scope);

bool           scope_add(scope_t scope, declaration_t declaration);
declaration_t  scope_find(scope_t scope, span_t name);
scope_t        scope_parent(scope_t scope);


#endif
