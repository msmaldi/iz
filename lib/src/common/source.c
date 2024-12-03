#include "common/mem.h"
#include "common/source.h"
#include "common/span.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct source_t
{
    span_t      code;
    const char *path;
};

struct source_t inline_source_object;

source_t source_inline(const char *code, const char *path)
{
    inline_source_object.code = span_sz(code);
    inline_source_object.path = path;
    return &inline_source_object;
}

source_t source_load(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        goto leave_null;

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1 || size >= __INT32_MAX__) // LCOV_EXCL_LINE
        goto leave_fd;                       // LCOV_EXCL_LINE

    lseek(fd, 0, SEEK_SET);

    char *code = mem_alloc(size + 100);
    read(fd, code, size);

    code[size] = '\0';

    close(fd);

    source_t source = mem_alloc(sizeof(struct source_t));
    source->path = path;
    source->code.data = code;
    source->code.size = size;

    return source;

leave_fd:      // LCOV_EXCL_LINE
    close(fd); // LCOV_EXCL_LINE
leave_null:;
    return NULL;
}

void source_free(source_t source)
{
    if (source == &inline_source_object)
        return;
    mem_free((void *)source->code.data);
    mem_free(source);
}

const char* source_code(source_t source)
{
    return source->code.data;
}

int source_size(source_t source)
{
    return source->code.size;
}

const char* source_path(source_t source)
{
    return source->path;
}

char* source_object_name(source_t source)
{
    size_t len = strlen(source_path(source));
    char *target = mem_alloc(len + 1);

    strcpy(target, source_path(source));

    target[len - 2] = 'o';
    target[len - 1] = '\0';

    return target;
}

char* source_assembly_name(source_t source)
{
    size_t len = strlen(source_path(source));
    char *target = mem_alloc(len + 1);

    strcpy(target, source_path(source));

    target[len - 2] = 's';
    target[len - 1] = '\0';

    return target;
}

char *source_llvm_name(source_t source)
{
    size_t len = strlen(source_path(source));
    char *target = mem_alloc(len + 1);

    strcpy(target, source_path(source));

    target[len - 2] = 'l';
    target[len - 1] = 'l';
    target[len] = '\0';

    return target;
}