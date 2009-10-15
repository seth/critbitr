#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>
#include "critbit.h"

SEXP cbt_make();
SEXP cbt_insert(SEXP xp, SEXP cv);
SEXP cbt_contains(SEXP xp, SEXP cv);

static void cbt_finalizer(SEXP xp);

SEXP cbt_make() {
    critbit0_tree *tree = (critbit0_tree *) Calloc(1, critbit0_tree);
    SEXP xp = R_MakeExternalPtr(tree, install("critbit_tree"), R_NilValue);
    PROTECT(xp);
    R_RegisterCFinalizerEx(xp, cbt_finalizer, TRUE);
    UNPROTECT(1);
    return xp;
}

static void cbt_finalizer(SEXP xp) {
    void *p = R_ExternalPtrAddr(xp);
    if (p) {
        Free(p);
        R_ClearExternalPtr(xp);
    }
}

SEXP cbt_insert(SEXP xp, SEXP cv) {
    const char *u = CHAR(STRING_ELT(cv, 0));
    critbit0_tree *tree = R_ExternalPtrAddr(xp);
    return ScalarInteger(critbit0_insert(tree, u));
}

SEXP cbt_contains(SEXP xp, SEXP cv) {
    const char *u = CHAR(STRING_ELT(cv, 0));
    critbit0_tree *tree = R_ExternalPtrAddr(xp);
    return ScalarInteger(critbit0_contains(tree, u));
}
