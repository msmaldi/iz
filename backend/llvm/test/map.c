#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "map.h"

static void success_branch(void **arg)
{
    (void) arg;

    {
        map_t map = map_new();

        assert_null(map_get(map, NULL));

        declaration_t declaration = (declaration_t)10;
        LLVMValueRef llvm_value = (LLVMValueRef)100;

        map_at(map, declaration, llvm_value);
        assert_int_equal(map_get(map, declaration), llvm_value);

        assert_null(map_get(map, NULL));

        LLVMValueRef llvm_other_value = (LLVMValueRef)1000;

        map_at(map, declaration, llvm_other_value);
        assert_int_equal(map_get(map, declaration), llvm_other_value);


        declaration_t declaration_other = (declaration_t)20;
        LLVMValueRef llvm_other_value2 = (LLVMValueRef)10000;

        map_at(map, declaration_other, llvm_other_value2);
        assert_int_equal(map_get(map, declaration_other), llvm_other_value2);

        map_free(map);
    }
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(success_branch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}