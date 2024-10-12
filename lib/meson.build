

includes = include_directories([ 'include' ])

sources = files(
    'src/ast/declaration.c',
    'src/ast/expression.c',
    'src/ast/statement.c',
    'src/ast/type.c',
    'src/ast/unit.c',
    'src/common/array.c',
    'src/common/span.c',
    'src/parser/lexer.c',
    'src/parser/parser.c',
    'src/sema/scope.c',
    'src/sema/sema.c',
)

iz = library(
    'iz',
    sources,
    include_directories: includes
)

iz_dep = declare_dependency(
    link_with: iz,
    include_directories: includes,
)

iz_test = static_library(
    'izl_test',
    sources,
    c_args : ['-DUNIT_TESTING' ],
    include_directories: includes
)

iz_dep_test = declare_dependency(
    link_with: iz_test,
    include_directories: includes,
)

subdir('test')
