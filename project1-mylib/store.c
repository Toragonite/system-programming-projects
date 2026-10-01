#include "store.h"


size_t list_store_size = 0;
size_t hash_store_size = 0;
size_t bitmap_store_size = 0;

struct list *list_store[MAX_STORE_SIZE];
struct hash *hash_store[MAX_STORE_SIZE];
struct bitmap *bitmap_store[MAX_STORE_SIZE];

void store_init(){
    
    for(int i=0 ; i<MAX_STORE_SIZE;i++){
        list_store[i]=NULL;
        hash_store[i]=NULL;
        bitmap_store[i]=NULL;
    }
    
    // for(int i=0 ; )
    list_store_size = 0;
    hash_store_size = 0;
    bitmap_store_size = 0;
}

/* List */
void store_list(struct list *new_list, size_t listIdx) {
    if (listIdx < MAX_STORE_SIZE) {
        list_store[listIdx] = new_list;
        // printf("save list %zu\n", listIdx);
        // printf("%p\n",list_store[listIdx]);
        list_store_size++;
    } else {
        printf("unvalid index\n");
    }
}

/* Hash */
void store_hash(struct hash *new_hash, size_t hashIdx) {
    if (hashIdx < MAX_STORE_SIZE) {
        hash_store[hashIdx] = new_hash;
        hash_store_size++;
    } else {
        printf("Hash store is full.\n");
    }
}

/* Bitmap */
void store_bitmap(struct bitmap *new_bitmap, size_t bitmapIdx) {
    if (bitmapIdx < MAX_STORE_SIZE) {
        if(bitmap_store[bitmapIdx] != NULL){
            bitmap_destroy(bitmap_store[bitmapIdx]);
        }
        bitmap_store[bitmapIdx] = new_bitmap;
        bitmap_store_size++;
    } else {
        printf("Bitmap store is full.\n");
    }
}

/* get functions */
struct list *get_list(size_t index) {
    if (index < MAX_STORE_SIZE) {
        return list_store[index];
    } else {
        printf("Invalid list index.\n");
        return NULL;
    }
}

struct hash *get_hash(size_t index) {
    if (index < MAX_STORE_SIZE) {
        return hash_store[index];
    } else {
        printf("Invalid hash index.\n");
        return NULL;
    }
}

struct bitmap *get_bitmap(size_t index) {
    if (index < MAX_STORE_SIZE) {
        return bitmap_store[index];
    } else {
        printf("Invalid bitmap index.\n");
        return NULL;
    }
}

/* delete function */

/* List */
void list_clear(struct list *list) {
    if (list == NULL) return; 
    if(list_empty(list)){
        free(list);
        return;
    }

    struct list_elem *e = list_begin(list);
    
    while (e != list_end(list)) {
        struct list_elem *next = list_next(e);
        struct list_item *item = list_entry(e, struct list_item, elem);
        if (item != NULL) { 
            free(item);
        }
        e = next;
    }
    free(list);
}

void clear_list_store() {
    for (size_t i = 0; i < list_store_size; i++) {
        struct list *current_list = list_store[i];
        if (current_list != NULL) {
            list_clear(current_list);
            list_store[i] = NULL;
        }
    }
    list_store_size = 0; // 리스트 스토어 크기 0으로 초기화
}
