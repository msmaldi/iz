#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "common/array.h"
#include "common/mem.h"

static void test_data(void **arg)
{
    (void) arg;

    {
        array_free(NULL);
    }

    {
        array_t empty = array_empty();

        assert_int_equal(array_capacity(empty), 0);
        assert_int_equal(array_size(empty), 0);

        array_free(empty);
    }

    {
        array_t(int) arr = array_new(1, sizeof(int));

        assert_int_equal(array_capacity(arr), 1);
        assert_int_equal(array_size(arr), 0);

        array_free(arr);
    }

    {
        array_t(int) arr = array_new(1, sizeof(int));

        size_t new_capacity = 2;
        arr = array_realloc(arr, new_capacity, sizeof(int));
        assert_int_equal(array_capacity(arr), new_capacity);
        assert_int_equal(array_size(arr), 0);

        array_free(arr);
    }

    {
        array_t(int) ints = NULL;

        int one = 1;
        ints = array_add(ints, one);

        assert_int_equal(one, ints[0]);

        array_free(ints);
    }

    {
        array_t(int) ints = array_new(1, sizeof(int));
        for (int i = 0; i < 2; i++)
            ints = array_add(ints, i);

        array_free(ints);
    }

    {
        array_t(int) ints = NULL;

        int one = 1;
        ints = array_add(ints, one);

        ints = array_shrink(ints);

        assert_int_equal(array_capacity(ints), 1);
        assert_int_equal(array_size(ints), 1);

        array_free(ints);
    }

    {
        array_t(int) ints = array_new(1, sizeof(int));

        int one = 1;
        ints = array_add(ints, one);

        ints = array_shrink(ints);

        assert_int_equal(array_capacity(ints), 1);
        assert_int_equal(array_size(ints), 1);

        array_free(ints);
    }

    {
        array_t(int) ints = array_new(1, sizeof(int));

        int one = 1;
        ints = array_add(ints, one);
        ints = array_add(ints, one);

        assert_int_equal(ints[0], 1);
        assert_int_equal(ints[1], 1);

        ints = array_realloc(ints, 1, sizeof(int));

        array_free(ints);
    }

    {
        assert_int_equal(array_new(0, 0), array_empty());
        assert_int_equal(array_realloc(NULL, 0, 0), array_empty());

        array_t(int) ints = array_new(1, sizeof(int));
        assert_int_equal(array_shrink(ints), array_empty());
    }

    {
        array_t(int*) numbers_ptr = array_empty();

        int *zero_ptr = mem_alloc(sizeof(int));
        *zero_ptr = 0;
        numbers_ptr = array_add(numbers_ptr, zero_ptr);

        assert_int_equal(numbers_ptr[0], zero_ptr);


        int *one_ptr = mem_alloc(sizeof(int));
        *one_ptr = 1;
        numbers_ptr = array_add(numbers_ptr, one_ptr);
        for (int i = 0; i < 1; i++)
        {
            int *ptr = mem_alloc(sizeof(int));
            numbers_ptr = array_add(numbers_ptr, ptr);
        }

        array_cleanup_free(numbers_ptr, mem_free);
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