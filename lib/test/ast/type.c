#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "ast/type.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        assert_int_equal(TYPE_BOOL, type_kind(type_bool()));
        assert_int_equal(TYPE_INT, type_kind(type_int()));
        assert_int_equal(TYPE_CHAR, type_kind(type_char()));
        assert_int_equal(TYPE_VOID, type_kind(type_void()));
    }

    {
        type_t bool_ty = type_bool_new();

        assert_int_equal(type_kind(bool_ty), TYPE_BOOL);

        type_free(bool_ty);
    }

    {
        type_t int_ty = type_int_new();

        assert_int_equal(type_kind(int_ty), TYPE_INT);

        type_free(int_ty);
    }

    {
        type_t char_ty = type_char_new();

        assert_int_equal(type_kind(char_ty), TYPE_CHAR);

        type_free(char_ty);
    }

    {
        type_t void_ty = type_void_new();

        assert_int_equal(type_kind(void_ty), TYPE_VOID);

        type_free(void_ty);
    }

    {
        type_t int_ty = type_int_new();
        array_t(type_t) param_s = array_empty();

        type_t type_callable = type_callable_new(int_ty, param_s);

        assert_int_equal(type_kind(type_callable), TYPE_CALLABLE);

        type_free(type_callable);
    }

    {
        type_t int_ty = type_int_new();
        type_t pointer_ty = type_pointer_new(int_ty);

        assert_int_equal(type_kind(pointer_ty), TYPE_POINTER);
        assert_int_equal(pointer_pointee(POINTER(pointer_ty)), int_ty);

        type_free(pointer_ty);
    }

    {
        assert_false(type_eq(type_bool(), type_int()));
        assert_false(type_eq(NULL, type_int()));
        assert_false(type_eq(type_bool(), NULL));
        assert_false(type_eq(type_char(), type_int()));
        assert_true(type_eq(type_char(), type_char()));
    }

    {
        type_t int_ptr = type_pointer_new(type_int_new());
        type_t bool_ptr = type_pointer_new(type_bool_new());
        type_t another_int_ptr = type_pointer_new(type_int_new());

        assert_false(type_eq(int_ptr, bool_ptr));
        assert_true(type_eq(int_ptr, another_int_ptr));

        type_free(int_ptr);
        type_free(bool_ptr);
        type_free(another_int_ptr);
    }

    {
        type_t int_ty = type_int_new();
        type_t cloned = type_clone(int_ty);

        assert_int_equal(type_kind(cloned), TYPE_INT);
        assert_true(type_eq(int_ty, cloned));

        type_free(int_ty);
        type_free(cloned);
    }

    {
        type_t bool_ty = type_bool_new();
        type_t cloned = type_clone(bool_ty);

        assert_int_equal(type_kind(cloned), TYPE_BOOL);
        assert_true(type_eq(bool_ty, cloned));

        type_free(bool_ty);
        type_free(cloned);
    }

    {
        type_t char_ty = type_char_new();
        type_t cloned = type_clone(char_ty);

        assert_int_equal(type_kind(cloned), TYPE_CHAR);
        assert_true(type_eq(char_ty, cloned));

        type_free(char_ty);
        type_free(cloned);
    }

    {
        type_t void_ty = type_void_new();
        type_t cloned = type_clone(void_ty);

        assert_int_equal(type_kind(cloned), TYPE_VOID);
        assert_true(type_eq(void_ty, cloned));

        type_free(void_ty);
        type_free(cloned);
    }

    {
        type_t pointer_ty = type_pointer_new(type_int_new());
        type_t cloned = type_clone(pointer_ty);

        assert_int_equal(type_kind(cloned), TYPE_POINTER);
        assert_true(type_eq(pointer_ty, cloned));
        assert_int_not_equal(pointer_pointee(POINTER(pointer_ty)), pointer_pointee(POINTER(cloned)));

        type_free(pointer_ty);
        type_free(cloned);
    }

    {
        type_t int_ty = type_int_new();
        array_t(type_t) param_s = array_empty();

        type_t arg_ty = type_int_new();
        param_s = array_add(param_s, arg_ty);

        type_t type_callable = type_callable_new(int_ty, param_s);

        type_t cloned = type_clone(type_callable);

        assert_int_equal(type_kind(callable_return_type(CALLABLE(cloned))), TYPE_INT);
        assert_int_equal(array_size(callable_param_s(CALLABLE(cloned))), 1);

        type_free(type_callable);
        type_free(cloned);
    }
}

int main()
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_data)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}