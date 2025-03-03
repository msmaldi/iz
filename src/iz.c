#include <stdio.h>

#include "ast/ast.h"
#include "ast/ast_print.h"
#include "common/source.h"
#include "parser/parser.h"
#include "sema/sema.h"
#include "llvm/backend.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    source_t source = source_load(argv[1]);
    if (source == NULL)
        return 1;

    unit_t unit = syntax_analysis(source);
    if (unit == NULL)
        return 1;

    compilation_t compilation = semantic_analysis(array_add(array_empty(), unit));
    if (compilation == NULL)
        return 1;

    compilation_print(compilation, stdout);

    backend_t backend = backend_codegen(compilation);

    backend_optimize(backend);
    backend_emit_object(backend);
    backend_emit_assembly(backend);
    backend_emit_llvm(backend);

    backend_free(backend);

    return 0;
}