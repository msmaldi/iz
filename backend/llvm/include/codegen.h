#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "ast/ast.h"

typedef  struct codegen_t*  codegen_t;

codegen_t codegen_new(array_t(unit_t) unit_s);

void codegen_free(codegen_t codegen);

void codegen_generate(codegen_t codegen);

bool codegen_validate(codegen_t codegen);

void codegen_dump(codegen_t codegen);

bool codegen_emit_object(codegen_t codegen, const char *filename);

#endif
