#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>
#include "critbit.h"

SEXP cbt_make();
SEXP cbt_insert(SEXP xp, SEXP cv);
SEXP cbt_contains(SEXP xp, SEXP cv);
SEXP cbt_prefix(SEXP xp, SEXP prefix);
SEXP cbt_delete(SEXP xp, SEXP cv);

SEXP cbt_load_file(SEXP xp, SEXP cv);

SEXP ht_load_file(SEXP iv, SEXP cv);

static void cbt_finalizer(SEXP xp);
static void ht_finalizer(SEXP xp);

SEXP cbt_make() {
    critbit_tree *tree = (critbit_tree *) Calloc(1, critbit_tree);
    tree->root = NULL;
    SEXP xp = R_MakeExternalPtr(tree, install("critbit_tree"), R_NilValue);
    PROTECT(xp);
    R_RegisterCFinalizerEx(xp, cbt_finalizer, TRUE);
    UNPROTECT(1);
    return xp;
}

static void cbt_finalizer(SEXP xp) {
    void *p = R_ExternalPtrAddr(xp);
    if (p) {
        critbit_clear(p);
        Free(p);
        R_ClearExternalPtr(xp);
    }
}

SEXP cbt_insert(SEXP xp, SEXP cv) {
    const char *u = CHAR(STRING_ELT(cv, 0));
    critbit_tree *tree = R_ExternalPtrAddr(xp);
    return ScalarInteger(critbit_insert(tree, u));
}

SEXP cbt_contains(SEXP xp, SEXP cv) {
    const char *u = CHAR(STRING_ELT(cv, 0));
    critbit_tree *tree = R_ExternalPtrAddr(xp);
    return ScalarLogical(critbit_contains(tree, u));
}

typedef struct simple_node {
    const char *value;
    struct simple_node *next;
} simple_node;

typedef struct {
    simple_node *head;
    int count;
} simple_list;

static int handle1(const char *v, void *arg)
{
    simple_list *list = (simple_list *)arg;
    list->count += 1;
    simple_node *node = Calloc(1, simple_node);
    node->value = v;
    node->next = list->head;
    list->head = node;
    return 1;
}

SEXP cbt_prefix(SEXP xp, SEXP prefix) {
    simple_list ans_list;
    ans_list.head = NULL;
    ans_list.count = 0;
    const char *p = CHAR(STRING_ELT(prefix, 0));
    critbit_allprefixed(R_ExternalPtrAddr(xp), p, handle1, &ans_list);
    SEXP ans;
    PROTECT(ans = NEW_CHARACTER(ans_list.count));
    simple_node *cur_node = ans_list.head;
    simple_node *prev_node = NULL;
    for (int i = 0; i < ans_list.count; ++i) {
        SET_STRING_ELT(ans, i, mkChar(cur_node->value));
        prev_node = cur_node;
        cur_node = cur_node->next;
        Free(prev_node);
    }
    UNPROTECT(1);
    return ans;
}

SEXP cbt_delete(SEXP xp, SEXP cv)
{
    const char *p = CHAR(STRING_ELT(cv, 0));
    return ScalarInteger(critbit_delete(R_ExternalPtrAddr(xp), p));
}

SEXP cbt_load_file(SEXP xp, SEXP cv) {
    critbit_tree *tree = R_ExternalPtrAddr(xp);
    const char *fname = CHAR(STRING_ELT(cv, 0));
    FILE *in = fopen(fname, "rb");
    if (in == 0) {
        error("unable to open file: %s", fname);
    }
    char line[100];
    while (fscanf(in, "%100s", &line) != EOF) {
        critbit_insert(tree, line);
    }
    fclose(in);
    return ScalarLogical(1);
}
