#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct Bucket {
	struct Bucket *next;
	char *key;
	void *value;
} Bucket;

typedef struct HashMap {
	Bucket **map;
	int entries;
	int *idx_map;
	int buckets;
} HashMap;

HashMap *init_hashmap() {
    HashMap *hm = (HashMap *)malloc(sizeof(HashMap));
	hm->map = (Bucket **)malloc(sizeof(Bucket *) * 1024);
	hm->idx_map = (int *)malloc(sizeof(int) * 1024);
	memset(hm->map, 0, sizeof(Bucket *) * 1024);
	memset(hm->idx_map, 0, sizeof(int) * 1024);
	hm->entries = 0;
	hm->buckets = 0;

	return hm;
}

int hm_hash(char *key) {
	int hash = 0;
	for (int i = 0; i < strlen(key); i++) {
		hash += key[i] % 1024;
	}

	return hash;
}

void *hm_get_val(HashMap *hm, char *key) {
	Bucket *tmp = hm->map[hm_hash(key)];
	while (tmp != NULL) {
		if (strcmp(tmp->key, key) == 0) {
			return tmp->value;
		} else {
			tmp = tmp->next;
		}
	}
	return NULL;
}

void hm_insert(HashMap *hm, char *key, void *value) {
	int key_hash = hm_hash(key);
	void *check = hm->map[key_hash];

	if (check == NULL) {
		hm->map[key_hash] = (Bucket *)malloc(sizeof(Bucket));
		hm->map[key_hash]->next = NULL;
		hm->map[key_hash]->value = value;
		hm->map[key_hash]->key = key;
		hm->idx_map[hm->buckets] = key_hash;
		hm->buckets += 1;
	} else {
		if (hm_get_val(hm, key) == NULL) {
			Bucket *tmp = (Bucket *)malloc(sizeof(Bucket));
			tmp->next = hm->map[key_hash];
			tmp->value = value;
			tmp->key = key;
			hm->map[key_hash] = tmp;
		} else {
			printf("key %s already filled\n", key);
			return;
		}
	}

	hm->entries += 1;
}

Bucket *hm_remove(HashMap *hm, char *key) {
	Bucket *tmp = hm->map[hm_hash(key)];
	Bucket *old_tmp = NULL;

	int size = 0;
	while (tmp != NULL) {
		if (strcmp(tmp->key, key) == 0) {
			if (old_tmp != NULL && tmp->next != NULL) {
				old_tmp->next = tmp->next;
			} else if (size == 0) {
				hm->map[hm_hash(key)] = NULL;
			}

			hm->entries -= 1;
			return tmp;
		} else {
			old_tmp = tmp;
			tmp = tmp->next;
			size += 1;
		}
	}
	return NULL;
}

void hm_print_bucket(Bucket *b) {
	Bucket *tmp = b;
	while (tmp != NULL) {
		printf("[BUCKET] %s: %p\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
}

#endif
