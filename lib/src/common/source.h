#ifndef _SOURCE_H_
#define _SOURCE_H_

#include "common/span.h"

typedef  struct source_t*   source_t;
typedef  struct location_t* location_t;

struct location_t
{
    span_t   span;
    source_t source;
    int      line;
    int      column;
};

source_t source_inline(const char *code, const char *path);

source_t source_load(const char *path);
void     source_free(source_t source);

const char* source_code(source_t source);
int         source_size(source_t source);
const char* source_path(source_t source);

char *source_object_name(source_t source);
char *source_assembly_name(source_t source);
char *source_llvm_name(source_t source);

location_t location_new(source_t source, span_t span, int line, int column);
void       location_free(location_t location);

source_t location_source(location_t location);
span_t   location_span(location_t location);
int      location_line(location_t location);
int      location_column(location_t location);

#endif