#include <stdio.h>
#include <stdlib.h>
#include "CuTest.h"
#include "critbit.h"

void Test_critbit_basics(CuTest *tc)
{
    critbit0_tree *t = (critbit0_tree *) malloc(sizeof(critbit0_tree));

    CuAssertIntEquals(tc, 0, critbit0_contains(t, "hello"));
    CuAssertIntEquals(tc, 2, critbit0_insert(t, "hello"));

    CuAssertIntEquals(tc, 1, critbit0_contains(t, "hello"));
    CuAssertIntEquals(tc, 0, critbit0_contains(t, "hell"));

    CuAssertIntEquals(tc, 1, critbit0_insert(t, "hello"));
    CuAssertIntEquals(tc, 2, critbit0_insert(t, "hello there"));

    CuAssertIntEquals(tc, 1, critbit0_contains(t, "hello there"));
    CuAssertIntEquals(tc, 1, critbit0_contains(t, "hello"));
    CuAssertIntEquals(tc, 0, critbit0_contains(t, "hello "));

    CuAssertIntEquals(tc, 0, critbit0_delete(t, "notthere"));
    CuAssertIntEquals(tc, 1, critbit0_delete(t, "hello"));
    CuAssertIntEquals(tc, 0, critbit0_contains(t, "hello"));

    critbit0_clear(t);
    free(t);
}
