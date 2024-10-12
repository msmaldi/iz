includes = include_directories([ 'include' ])

llvm_iz_sources = files(
    'src/codegen.c',
    'src/map.c'
)

llvm_iz = static_library('llvmiz', llvm_iz_sources,
    dependencies: [ iz_dep, llvm ],
    include_directories: includes
)

llvm_iz_dep = declare_dependency(
    dependencies: [ iz_dep, llvm ],
    link_with: llvm_iz,
    include_directories: includes
)


llvm_iz_test = static_library('llvmiz_test', 
    llvm_iz_sources,
    c_args : ['-DUNIT_TESTING' ],
    dependencies: [ iz_dep, llvm ],
    include_directories: includes
)

llvm_iz_dep_test = declare_dependency(
    dependencies: [ iz_dep, llvm ],
    link_with: llvm_iz_test,
    include_directories: includes
)

subdir('test')
