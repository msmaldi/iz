#ifndef _SOURCE_H_
#define _SOURCE_H_

#include "common/span.h"

typedef  struct source_t*  source_t;

source_t source_inline(const char *code, const char *path);

source_t source_load(const char *path);
void     source_free(source_t source);

const char* source_code(source_t source);
int         source_size(source_t source);
const char* source_path(source_t source);

char *source_object_name(source_t source);
char *source_assembly_name(source_t source);

#endif