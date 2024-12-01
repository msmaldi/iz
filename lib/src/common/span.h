#ifndef _SPAN_H_
#define _SPAN_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct span span_t;
struct span
{
    int         size;
    const char  *data;
};

span_t span_sz(const char *sz);
span_t span_ctor(int size, const char *data);

bool   span_eq(span_t lhs, span_t rhs);
int    span_cmp(span_t lhs, span_t rhs);

#endif