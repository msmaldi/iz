map = executable('map', 'map.c', dependencies: [ llvm_iz_dep_test, cmocka ])
test('map', map)

backend = executable('backend', 'llvm/backend.c', dependencies: [ llvm_iz_dep_test, cmocka ])
test('backend', backend)