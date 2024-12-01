expression = executable(
    'expression',
    'ast/expression.c',
    dependencies: [ iz_dep_test, cmocka ])
test('expression', expression)

statement = executable(
    'statement',
    'ast/statement.c',
    dependencies: [ iz_dep_test, cmocka ])
test('statement', statement)

type = executable(
    'type',
    'ast/type.c',
    dependencies: [ iz_dep_test, cmocka ])
test('type', type)

declaration = executable(
    'declaration',
    'ast/declaration.c',
    dependencies: [ iz_dep_test, cmocka ])
test('declaration', declaration)

unit = executable(
    'unit',
    'ast/unit.c',
    dependencies: [ iz_dep_test, cmocka ])
test('unit', unit)

span = executable(
    'span',
    'common/span.c',
    dependencies: [ iz_dep_test, cmocka ])
test('span', span)

source = executable(
    'source',
    'common/source.c',
    dependencies: [ iz_dep_test, cmocka ])
test('source', source)

array = executable(
    'array',
    'common/array.c',
    dependencies: [ iz_dep_test, cmocka ])
test('array', array)

lexer = executable(
    'lexer',
    'parser/lexer.c',
    dependencies: [ iz_dep_test, cmocka ])
test('lexer', lexer)

parser = executable(
    'parser',
    'parser/parser.c',
    dependencies: [ iz_dep_test, cmocka ])
test('parser', parser)

scope = executable(
    'scope',
    'sema/scope.c',
    dependencies: [ iz_dep_test, cmocka ])
test('scope', scope)

sema = executable(
    'sema',
    'sema/sema.c',
    dependencies: [ iz_dep_test, cmocka ])
test('sema', sema)

