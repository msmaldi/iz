#ifndef _ARR_H_
#define _ARR_H_

#include <stddef.h>

#define array_t(T) T*

typedef void* address_t;
typedef void* array_t;
typedef void (*release_t)(void *);

array_t   array_empty();
array_t   array_new(size_t capacity, size_t object_size);
array_t   array_realloc(array_t array, size_t new_capacity, size_t object_size);

void      array_cleanup_object(array_t array, release_t release, size_t object_size);
#define   array_cleanup(array, release) array_cleanup_object(array, release, sizeof(*array))

void      array_cleanup_free_object(array_t array, release_t release, size_t object_size);
#define   array_cleanup_free(array, release) array_cleanup_free_object(array, release, sizeof(*array))

void      array_free(array_t array);

size_t    array_capacity(array_t array);
size_t    array_size(array_t array);

array_t   array_add_object(array_t array, address_t object, size_t object_size);
#define   array_add(array, object) array_add_object(array, &object, sizeof(*array))

array_t   array_shrink_object(array_t array, size_t object_size);
#define   array_shrink(array) array_shrink_object(array, sizeof(*array))

#endif
