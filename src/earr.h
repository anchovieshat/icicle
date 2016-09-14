#ifndef DYNARR_H
#define DYNARR_H

#include "common.h"

typedef struct Entity {
	u8 sprite_id;
	glm::vec3 pos;
} Entity;

Entity new_entity(u8 sprite_id, glm::vec3 pos) {
	Entity e;
	e.sprite_id = sprite_id;
	e.pos = pos;

	return e;
}

typedef struct EntityArr {
	Entity *arr;
	u32 len;
	u32 capacity;
} EntityArr;

EntityArr *new_earr() {
	EntityArr *e = (EntityArr *)malloc(sizeof(EntityArr));
	e->arr = (Entity *)malloc(sizeof(Entity *) * 10);
	e->len = 0;
	e->capacity = 10;

	return e;
}

inline Entity ea_get(EntityArr *e, u64 idx) {
	assert(idx < e->len);
	return e->arr[idx];
}

void ea_push(EntityArr *e, Entity entry) {
    if (e->len >= e->capacity) {
		e->arr = (Entity *)realloc(e->arr, sizeof(Entity *) * (e->capacity + 10));
		e->capacity += 10;
	}

	e->arr[e->len] = entry;
	e->len++;
}

#endif
