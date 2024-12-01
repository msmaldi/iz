includes = include_directories([ 'src' ])

sources = files(
    'src/ast/compilation.c',
    'src/ast/declaration.c',
    'src/ast/expression.c',
    'src/ast/statement.c',
    'src/ast/type.c',
    'src/ast/unit.c',
    'src/common/array.c',
    'src/common/source.c',
    'src/common/span.c',
    'src/parser/lexer.c',
    'src/parser/parser.c',
    'src/sema/scope.c',
    'src/sema/sema.c',
)

iz = static_library(
    'iz',
    sources,
    include_directories: includes
)

iz_dep = declare_dependency(
    link_with: iz,
    include_directories: includes,
)

iz_c_args = []
if get_option('b_coverage')
    iz_c_args = [ '-DUNIT_TESTING' ]
endif

iz_test = static_library(
    'izl_test',
    sources,
    c_args : [ iz_c_args ],
    include_directories: includes
)

iz_dep_test = declare_dependency(
    link_with: iz_test,
    include_directories: includes,
)

subdir('test')
