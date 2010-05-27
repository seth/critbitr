#ifndef CRITBIT_H_
#define CRITBIT_H_

typedef struct {
  void *root;
} critbit_tree;

int critbit_contains(critbit_tree *t, const char *u);

/* insert a key into the tree
   returns 1 for if the key was inserted and not previously present.
   returns 2 if the key was already in the tree.
   returns 0 if a (memory) error occurs.
 */
int critbit_insert(critbit_tree *t, const char *u);
int critbit_delete(critbit_tree *t, const char *u);
void critbit_clear(critbit_tree *t);
int critbit_allprefixed(critbit_tree *t, const char *prefix,
                         int (*handle) (const char *, void *), void *arg);

#endif  /* CRITBIT_H_ */
