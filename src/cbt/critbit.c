#define _POSIX_C_SOURCE 200112
#define uint8 uint8_t
#define uint32 uint32_t

#include <stdint.h> 
#include <string.h> 
#include <stdlib.h> 

#include <sys/types.h> 
#include <errno.h> 

typedef struct {
    void *child[2];
    uint32 byte;
    uint8 otherbits;
} critbit_node;

typedef struct {
    void *root;
} critbit_tree;


int critbit_contains(critbit_tree *t, const char *u)
{
    const uint8 *ubytes = (void*)u;
    const size_t ulen = strlen(u);
    uint8 *p = t->root;

    if (!p) return 0;

    while (1 & (intptr_t)p) {
        critbit_node *q = (void*)(p-1);

        uint8 c = 0;
        if (q->byte<ulen) c = ubytes[q->byte];
        const int direction = (1 + (q->otherbits | c)) >> 8;
        p = q->child[direction];
    }
    return 0 == strcmp(u, (const char*)p);
}


int critbit_insert(critbit_tree *t, const char *u)
{
    const uint8 *const ubytes = (void*)u;
    const size_t ulen = strlen(u);
    uint8 *p = t->root;

    if (!p) {
        char *x;
        int a = posix_memalign((void**)&x, sizeof(void*), ulen + 1);
        if (a) return 0;
        memcpy(x, u, ulen + 1);
        t->root = x;
        return 2;
    }

    while (1 & (intptr_t)p) {
        critbit_node *q = (void*)(p-1);
        uint8 c = 0;
        if (q->byte < ulen) c = ubytes[q->byte];
        const int direction = (1 + (q->otherbits | c)) >> 8;
        p = q->child[direction];
    }
    /* p now points to a leaf node (a string) */

    uint32 newbyte;
    uint32 newotherbits;

    for (newbyte = 0; newbyte < ulen; ++newbyte) {
        if (p[newbyte] != ubytes[newbyte]) {
            newotherbits = p[newbyte] ^ ubytes[newbyte];
            goto different_byte_found;
        }
    }

    if (p[newbyte] != 0){
        newotherbits = p[newbyte];
        goto different_byte_found;
    }
    return 1;

 different_byte_found:
    /* we've just done newotherbits = ours ^ theirs, or
       we had a length mismatch.  So the leftmost 1 is the first
       bit that differs and we need to create a mask for it.

       We do this by setting the rightmost 1 bit to zero repeatedly
       until the next iteration would be all zeros.
     */

    /* x & (x - 1) turns off the rightmost 1 bit */

    while (newotherbits & (newotherbits-1)) newotherbits &= newotherbits - 1;
    newotherbits ^= 255;
    uint8 c = p[newbyte];
    /* newdirection will be 1 if critbit matches, and 0 otherwise.
       I'm confused about this test.  Is this more efficient than:
          (newotherbits | c) != newotherbits
       which seems easier to read...  examine asm?
     */
    int newdirection = (1 + (newotherbits | c)) >> 8;

    critbit_node*newnode;
    if (posix_memalign((void**)&newnode, sizeof(void*), sizeof(critbit_node)))
        return 0;

    char*x;
    if (posix_memalign((void**)&x, sizeof(void*), ulen + 1)) {
        free(newnode);
        return 0;
    }
    memcpy(x, ubytes, ulen + 1);

    newnode->byte = newbyte;
    newnode->otherbits = newotherbits;
    newnode->child[1-newdirection] = x;

    void **wherep = &t->root;
    for ( ; ; ) {
        uint8 *p = *wherep;
        if (!(1 & (intptr_t)p)) break;
        critbit_node *q = (void*)(p-1);
        if (q->byte > newbyte) break;
        if (q->byte == newbyte && q->otherbits > newotherbits) break;
        /* at this point newnode is greater or equal to q */
        uint8 c = 0;
        if (q->byte < ulen) c = ubytes[q->byte];
        const int direction = (1 + (q->otherbits | c)) >> 8;
        wherep = q->child + direction;
    }

    newnode->child[newdirection] = *wherep;
    *wherep = (void*)(1 + (char*)newnode);
    return 2;
}


int critbit_delete(critbit_tree*t, const char*u)
{
    const uint8 *ubytes = (void*)u;
    const size_t ulen = strlen(u);
    uint8 *p = t->root;
    void **wherep = &t->root;
    void **whereq = 0;
    critbit_node *q = 0;
    int direction = 0;

    if (!p) return 0;

    while (1 & (intptr_t)p){
        whereq = wherep;
        q = (void*)(p - 1);
        uint8 c = 0;
        if (q->byte < ulen) c = ubytes[q->byte];
        direction = (1 + (q->otherbits | c)) >> 8;
        wherep = q->child + direction;
        p = *wherep;
    }

    if (0 != strcmp(u, (const char*)p)) return 0;
    free(p);

    if (!whereq) {
        t->root = 0;
        return 1;
    }

    *whereq = q->child[1 - direction];
    free(q);
    return 1;
}


static void traverse(void*top)
{
    uint8 *p = top;
    if (1 & (intptr_t)p) {
        critbit_node *q = (void*)(p - 1);
        traverse(q->child[0]);
        traverse(q->child[1]);
        free(q);
    } else {
        free(p);
    }
}

void critbit_clear(critbit_tree*t)
{
    if (t->root) traverse(t->root);
    t->root = NULL;
}


static int allprefixed_traverse(uint8 *top,
                                int(*handle)(const char*, void*), void *arg)
{

    if (1 & (intptr_t)top) {
        critbit_node *q = (void*)(top - 1);
        int direction;
        for (direction = 0; direction < 2; ++direction)
            switch (allprefixed_traverse(q->child[direction], handle, arg)) {
            case 1: break;
            case 0: return 0;
            default: return -1;
            }
        return 1;
    }
    return handle((const char*)top, arg);
}

int critbit_allprefixed(critbit_tree*t, const char*prefix,
                         int(*handle)(const char*, void*), void*arg)
{
    const uint8*ubytes = (void*)prefix;
    const size_t ulen = strlen(prefix);
    uint8 *p = t->root;
    uint8 *top = p;

    if (!p) return 1;

    while (1 & (intptr_t)p) {
        critbit_node *q = (void*)(p-1);
        uint8 c = 0;
        if (q->byte < ulen) c = ubytes[q->byte];
        const int direction = (1 + (q->otherbits | c)) >> 8;
        p = q->child[direction];
        if (q->byte < ulen) top = p;
    }
    size_t i;
    for (i = 0;i < ulen; ++i) {
        if (p[i] != ubytes[i]) return 1;
    }

    return allprefixed_traverse(top, handle, arg);
}
