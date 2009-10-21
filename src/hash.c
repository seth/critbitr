#include "hash.h"
#include <R.h>
#include <Rdefines.h>

const int HASH_SCALE = 10;	/* target # collisions */

int 
hashpjw(const char *s)
/* from  R-devel/src/main/envir.c:188, 18 October, 2009*/
{
    char *p;
    unsigned h = 0, g;
    for (p = (char *) s; *p; p++) {
		h = (h << 4) + (*p);
		if ((g = h & 0xf0000000) != 0) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
    }
    return h;
}

struct _Hash *
hash_new(int table_size)
{
	struct _Hash *hash = Calloc(1, struct _Hash);
	struct _Entry *entries = Calloc(table_size, struct _Entry);
	for (int i = 0; i < table_size; ++i) {
		entries[i].val = NULL;
		entries[i].next = NULL;
	}
	hash->table_size = table_size;
	hash->table = entries;
	return hash;
}

struct _Entry *
hash_insert(struct _Hash *hash, const char *str)
{
	int key = hashpjw(str) % hash->table_size;
	struct _Entry *entry = &hash->table[key], *kv;
	if (entry->val != NULL) {
		kv = entry;
		while (kv->next != NULL) {
			kv = kv->next;
			if (strcmp(kv->val, str) == 0)
				return kv;
		};
		kv = kv->next = Calloc(1, struct _Entry);
	} else {
		kv = &hash->table[key];
	}
	const int len = strlen(str);
	kv->val = Calloc(len + 1, char);
	strcpy(kv->val, str);
	kv->next = NULL;
	return kv;
}

void
_hash_stats(struct _Hash *hash)
{
	struct _Entry *kv;
	int i, max = 0;
	for (i = 0; i < hash->table_size; ++i) {
		int n = 0;
		for (kv = &hash->table[i]; kv != NULL && kv->val != NULL; 
			 kv = kv->next)
			n += 1;
		max = n > max ? n : max;
	}

	int *cnt = Calloc(max + 1, int);
	for (i = 0; i <= max; ++i)
		cnt[i] = 0;
	for (i = 0; i < hash->table_size; ++i) {
		int n = 0;
		for (kv = &hash->table[i]; kv != NULL && kv->val != NULL;
			 kv = kv->next)
			n += 1;
		cnt[n] += 1;
	}
	int tot = 0;
	for (i = 0; i <= max; ++i)
		if (cnt[i] != 0) {
			tot += cnt[i] * i;
			if (i <= 25)
				Rprintf("%d\t%d\n", i, cnt[i]);
		}
	Rprintf("max: %d\n", max);
	Rprintf("tot: %d\n", tot);
	Free(cnt);
}

#ifdef USE_ENCODE_FUNC
void
hash_encode(struct _Hash *hash, ENCODE_FUNC encode)
{
	struct _Entry *kv;
	char *c;
	for (int i = 0; i < hash->table_size; ++i)
		for (kv = &hash->table[i]; kv != NULL; kv = kv->next)
			if ((c = kv->val) != NULL)
				while (*c != '\0') { *c = encode(*c); ++c; }
}
#endif  /* USE_ENCODE_FUNC */

void
hash_free(struct _Hash *hash)
{
	struct _Entry *kv;
	for (int i = 0; i < hash->table_size; ++i) {
		kv = hash->table[i].next;
		while (kv != NULL) {
			struct _Entry *tmp = kv;
			kv = kv->next;
			Free(tmp->val);
			Free(tmp);
		}
	}
	Free(hash->table);
}
