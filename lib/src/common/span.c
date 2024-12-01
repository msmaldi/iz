#include "common/span.h"

#include <string.h>

span_t span_sz(const char *sz)
{
    return span_ctor(strlen(sz), sz);
}

span_t span_ctor(int size, const char *data)
{
    return (span_t) { .size = size, .data = data };
}

bool span_eq(span_t lhs, span_t rhs)
{
    if (lhs.size != rhs.size)
        return false;

    for (size_t i = 0; i < lhs.size; i++)
        if (lhs.data[i] != rhs.data[i])
            return false;

    return true;
}

int span_cmp(span_t lhs, span_t rhs)
{
    int min_size = lhs.size < rhs.size ? lhs.size : rhs.size;
    int cmp = memcmp(lhs.data, rhs.data, min_size);
    if (cmp != 0)
        return cmp;

    if (lhs.size == rhs.size)
        return 0;
    if (lhs.size < rhs.size)
        return -1;
    else
        return 1;
}
