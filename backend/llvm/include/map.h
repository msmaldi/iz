#ifndef _MAP_H_
#define _MAP_H_

#include "ast/ast.h"
#include <llvm-c/Core.h>

typedef  struct map_t*  map_t;

map_t         map_new();
void          map_free(map_t map);

LLVMValueRef  map_get(map_t map, declaration_t declaration);
void          map_at(map_t map, declaration_t declaration, LLVMValueRef llvm_value);

#endif
