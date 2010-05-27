#include <stdio.h>
#include <stdlib.h>
#include "CuTest.h"
#include "critbit.h"

void Test_critbit_basics(CuTest *tc)
{
    critbit_tree *t = (critbit_tree *) malloc(sizeof(critbit_tree));

    CuAssertIntEquals(tc, 0, critbit_contains(t, "hello"));
    CuAssertIntEquals(tc, 2, critbit_insert(t, "hello"));

    CuAssertIntEquals(tc, 1, critbit_contains(t, "hello"));
    CuAssertIntEquals(tc, 0, critbit_contains(t, "hell"));

    CuAssertIntEquals(tc, 1, critbit_insert(t, "hello"));
    CuAssertIntEquals(tc, 2, critbit_insert(t, "hello there"));

    CuAssertIntEquals(tc, 1, critbit_contains(t, "hello there"));
    CuAssertIntEquals(tc, 1, critbit_contains(t, "hello"));
    CuAssertIntEquals(tc, 0, critbit_contains(t, "hello "));

    CuAssertIntEquals(tc, 0, critbit_delete(t, "notthere"));
    CuAssertIntEquals(tc, 1, critbit_delete(t, "hello"));
    CuAssertIntEquals(tc, 0, critbit_contains(t, "hello"));

    critbit_clear(t);
    free(t);
}
