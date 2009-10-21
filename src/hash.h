#ifndef HASH_H_
#define HASH_H_

struct _Entry {
	char *val;
	struct _Entry *next;
};

struct _Hash {
	int table_size;
	struct _Entry *table;
};

typedef struct _Hash HTHash;

struct _Hash *
hash_new(int table_size);

struct _Entry *
hash_insert(struct _Hash *hash, const char *str);

void
hash_free(struct _Hash *hash);


#endif
