#ifndef STORE_H
#define STORE_H

#include "list.h"
#include "hash.h"
#include "bitmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> 

#define MAX_STORE_SIZE 11

struct list_elem;
extern struct list *list_store[MAX_STORE_SIZE];
extern struct hash *hash_store[MAX_STORE_SIZE];
extern struct bitmap *bitmap_store[MAX_STORE_SIZE];

extern size_t list_store_size;
extern size_t hash_store_size;
extern size_t bitmap_store_size;

void store_init();
void store_list(struct list *new_list, size_t listIdx);
void store_hash(struct hash *new_hash, size_t hashIdx);
void store_bitmap(struct bitmap *new_bitmap, size_t bitmapIdx);

struct list *get_list(size_t index);
struct hash *get_hash(size_t index);
struct bitmap *get_bitmap(size_t index);

void list_clear(struct list *list);
void clear_list_store();

#endif
