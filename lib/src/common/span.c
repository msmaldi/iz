#include "common/span.h"

#include <string.h>

span_t span_sz(const char *sz)
{
    return span_ctor(strlen(sz), sz);
}

span_t span_ctor(size_t size, const char *data)
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
