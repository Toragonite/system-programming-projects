#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#include "types.h"
#include "bitmap.h"
#include "debug.h"
#include "hash.h"
#include "hex_dump.h"
#include "limits.h"
#include "list.h"
#include "store.h"
#include "parser.h"

int main(void)
{
    char command[100];
    
    store_init();
    
    while (1)
    {
        fflush(stdout);
        if (!fgets(command, sizeof(command), stdin))
        {
            break;
        }

       command[strcspn(command, "\n")] = 0;
//	fprintf("%s", command);
        // global function
        if (strcmp(command, "quit") == 0){
            //free all datastructures;
            clear_list_store();
           
            break;
        }
        // create command handler
        else if (strncmp(command, "create", 6)==0)
        {
            DataStructType type = parse_create_command(command);
            
            switch (type) {
                case DATATYPE_LIST:{
                    //list
                    struct list *new_list = malloc(sizeof(struct list));
                    if(new_list == NULL)continue;
                    
                    
                    size_t listIdx;
                    parse_list_global(command, &listIdx);

                    // struct list *new_list =get_list(listIdx);//
                    // if(new_list == NULL){//dbg
                    //     printf("new_list error");
                    //     return 1;
                    // }
                    list_init(new_list);

                    store_list(new_list, listIdx);
                    // struct list *new_list = list_store[listIdx];
                    // list_init(new_list);

                    continue;}
                case DATATYPE_BITMAP:{
                    //bitmap
                    //create bitmap
		            
                    
                    size_t bit_cnt, bitmapIdx;
                    char dummy[100];

                    parse_create_bitmap(command, dummy, &bitmapIdx, &bit_cnt);
                    struct bitmap *new_bitmap = bitmap_create(bit_cnt);
                    if (new_bitmap == NULL) {
                        fprintf(stderr, "Failed to create a bitmap.\n");
                        return EXIT_FAILURE;
                    }

                    store_bitmap(new_bitmap, bitmapIdx);

                    continue;}
                case DATATYPE_HASHTABLE:{
                    //hashtable
                    //create hashtable
                    struct hash *new_hash = malloc(sizeof(struct hash));
                    if(new_hash == NULL)continue;

                    size_t hashIdx;
                    parse_hash_create(command, &hashIdx);
                    hash_init(new_hash, hash_int_wrapper, hash_int_less, NULL);

                    store_hash(new_hash, hashIdx);

                    continue;}
                default:
                    //error handler
                    printf("Unknown data type.\n");
            }
        }
        // dumpdata command handler
        else if (strncmp(command, "dumpdata", 8)==0)
        {
            DataStructType type = parse_dumpdata_command(command);

            switch (type) {
                case DATATYPE_LIST:{
                    //list
                    size_t listIdx;
                    sscanf(command, "dumpdata list%zu", &listIdx);
                    struct list *list = get_list(listIdx);
                    // printf("%zu", listIdx);
                    list_dump_data(list);

                   continue;}
                case DATATYPE_BITMAP:{
                    //bitmap
                    char dummy[100];
                    size_t bitmapIdx;
                    sscanf(command, "%s bm%zu",dummy, &bitmapIdx);
                    struct bitmap *bitmap = get_bitmap(bitmapIdx);
                    bitmap_binomical_dump(bitmap);
                    
                    continue;}
                case DATATYPE_HASHTABLE:{
                    //hashtable
                    size_t hashIdx;
                    parse_hash_create(command, &hashIdx);

                    struct hash *hash = get_hash(hashIdx);

                    hash_dump(hash);
                    continue;}
                default:{
                    //error handler
                    printf("Unknown data type.\n");
                }   continue;
            }
        }
        // delete command handler
        else if (strncmp(command, "delete", 6)==0)
        {
            DataStructType type = parse_delete_command(command);

            switch (type) {
                case DATATYPE_LIST:{
                    //list
                    size_t listIdx;
                    parse_list_global(command, &listIdx);
		            struct list *list = get_list(listIdx);
                    list_clear(list);
                    list_store[listIdx]=NULL;

                    continue;}
                case DATATYPE_BITMAP:{
                    //bitmap
                    char dummy[100];
                    size_t bitmapIdx;
                    sscanf(command, "%s bm%zu",dummy, &bitmapIdx);
                    struct bitmap *bitmap = get_bitmap(bitmapIdx);
                    bitmap_destroy(bitmap);
                    
                    continue;}
                case DATATYPE_HASHTABLE:{
                    //hashtable
                    // printf("delete hashtable\n");
                    size_t hashIdx;
                    char cmd[100];
                    parse_hash_single(command, cmd, &hashIdx);
                    struct hash *hash = get_hash(hashIdx);
                    hash_destroy(hash, hash_elem_destructor);
                    free(hash);
                    
                    continue;}
                default:{
                    //error handler
                    printf("Unknown data type.\n");}
            }
        }
        // data structure fork
        else
        {
            // list functions
            if (strncmp(command, "list", 4)==0)
            {
                // list modules
                // struct list *list;
                //get list pointer from store
//                printf("list modules");
                list_modules(command);
            }
            // hashtable functions
            else if (strncmp(command, "hash", 4)==0)
            {
                // hashtable modules
                // printf("hashtable");
                hash_modules(command);
            }
            // bitmap functions
            else if (strncmp(command, "bitm", 4)==0)
            {
                // bitmap modules
                bitmap_modules(command);
            }
            else
            {
                printf("MAIN: no match function");
                printf("%s", command);

            }
        }
    }
    printf("\n");
    return 0;
}
