#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>

#ifdef UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#define  mem_alloc      test_malloc
#define  mem_realloc    test_realloc
#define  mem_free       test_free

#else

#define  mem_alloc      malloc
#define  mem_realloc    realloc
#define  mem_free       free

#endif

#endif