#include "stdbool.h"
#include "string.h"
#include "common/array.h"
#include "common/mem.h"

#define DEFAULT_NEW_CAPACITY  16
#define REALLOC_MULTIPLICATOR  2

typedef  struct array_header_t*        array_header_t;
struct array_header_t
{
    size_t capacity;
    size_t size;
};

struct array_header_t array_empty_header = { 0 };

static inline
array_header_t array_header(array_t array)
{
    array_header_t header = array;
    return header - 1;
}

static inline
array_t array_data(array_header_t header)
{
    return header + 1;
}

array_t array_empty()
{
    return array_data(&array_empty_header);
}

array_t array_new(size_t capacity, size_t object_size)
{
    if (capacity == 0)
        return array_empty();

    size_t allocated_size = sizeof(struct array_header_t) + object_size * capacity;

    array_header_t header = mem_alloc(allocated_size);
    //if (header == NULL)
    //    return NULL;
    header->capacity = capacity;
    header->size = 0;
    return array_data(header);
}

void* array_at(void *array, size_t index, const size_t obj_size)
{
    return (char *)array + (index * obj_size);
}

void array_cleanup_object(array_t array, release_t release, size_t object_size)
{
    size_t size = array_size(array);
    for (size_t i = 0; i < size; i++)
    {
        void **at = array_at(array, i, object_size);
        release(*at);
    }
}

void array_cleanup_free_object(array_t array, release_t release, size_t object_size)
{
    array_cleanup_object(array, release, object_size);
    array_free(array);
}

static inline
bool is_empty(array_t array)
{
    return array_header(array) == &array_empty_header;
}

static inline
bool is_null_or_empty(array_t array)
{
    return array == NULL || is_empty(array);
}

void array_free(array_t array)
{
    if (is_null_or_empty(array))
        return;
    mem_free(array_header(array));
}

size_t array_capacity(array_t array)
{
    return array_header(array)->capacity;
}

size_t array_size(array_t array)
{
    return array_header(array)->size;
}

array_t array_realloc(array_t array, size_t new_capacity, size_t object_size)
{
    if (new_capacity == 0)
        return array_empty();

    size_t allocated_size = sizeof(struct array_header_t) + object_size * new_capacity;

    array_header_t header = mem_realloc(array_header(array), allocated_size);
    //if (header == NULL)
    //    return NULL;
    header->capacity = new_capacity;
    if (header->capacity < header->size)
        header->size = header->capacity;
    return array_data(header);
}

static inline
bool is_full(array_header_t header)
{
    return header->capacity == header->size;
}

array_t array_add_object(array_t array, address_t object, size_t object_size)
{
    if (is_null_or_empty(array))
    {
        array = array_new(DEFAULT_NEW_CAPACITY, object_size);
    }
    else if (is_full(array_header(array)))
    {
        size_t new_capacity = array_capacity(array) * REALLOC_MULTIPLICATOR;
        array = array_realloc(array, new_capacity, object_size);
    }
    //if (array == NULL)
    //    return NULL;

    array_header_t header = array_header(array);
    address_t target = (char *)array + array_size(array) * object_size;
    memcpy(target, object, object_size);
    header->size++;
    return array;
}

array_t array_shrink_object(array_t array, size_t object_size)
{
    if (array_size(array) == 0)
    {
        array_free(array);
        return array_empty();
    }

    if (is_full(array_header(array)))
        return array;

    return array_realloc(array, array_size(array), object_size);
}
