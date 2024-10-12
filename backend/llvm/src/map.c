#include "common/mem.h"

#include "map.h"

struct key_value_t
{
    declaration_t key;
    LLVMValueRef value;
};

struct map_t
{
    array_t(struct key_value_t) data;
};

map_t map_new()
{
    map_t map = mem_alloc(sizeof(struct map_t));
    map->data = array_empty();
    return map;
}

void map_free(map_t map)
{
    array_free(map->data);
    mem_free(map);
}

LLVMValueRef map_get(map_t map, declaration_t declaration)
{
    size_t size = array_size(map->data);
    for (int i = 0; i < size; i++)
    {
        struct key_value_t key_value = map->data[i];
        if (key_value.key == declaration)
            return key_value.value;
    }

    return NULL;
}

void map_at(map_t map, declaration_t declaration, LLVMValueRef llvm_value)
{
    size_t size = array_size(map->data);
    for (int i = 0; i < size; i++)
    {
        struct key_value_t key_value = map->data[i];
        if (key_value.key == declaration)
            map->data[i].value = llvm_value;
    }
    struct key_value_t key_value = {declaration, llvm_value};
    map->data = array_add(map->data, key_value);
}
