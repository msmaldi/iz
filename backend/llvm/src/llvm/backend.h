#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "ast/ast.h"

typedef  struct backend_t*  backend_t;

backend_t  backend_codegen(compilation_t compilation);
void       backend_free(backend_t backend);

bool       backend_validate(backend_t backend);
void       backend_optimize(backend_t backend);
void       backend_dump(backend_t backend);

void       backend_emit_object(backend_t backend);
void       backend_emit_assembly(backend_t backend);
void       backend_emit_llvm(backend_t backend);

#endif
