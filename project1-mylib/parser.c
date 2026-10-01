#include "parser.h"


//global
DataStructType parse_create_command(const char* command) {
    char type[10]; 

    if (sscanf(command, "create %s", type) == 1) {
        if (strcmp(type, "list") == 0) {
            return DATATYPE_LIST;
        } else if (strcmp(type, "bitmap") == 0) {
            return DATATYPE_BITMAP;
        } else if (strcmp(type, "hashtable") == 0) {
            return DATATYPE_HASHTABLE;
        }
    }
    return DATATYPE_UNKNOWN;
}

DataStructType parse_dumpdata_command(const char* command) {
    char type[10]; 

    if (sscanf(command, "dumpdata %s", type) == 1) {
        if (strncmp(type, "list", 4) == 0) {
            return DATATYPE_LIST;
        } else if (strncmp(type, "bm", 2) == 0) {
            return DATATYPE_BITMAP;
        } else if (strncmp(type, "hash", 4) == 0) {
            return DATATYPE_HASHTABLE;
        }
    }
    return DATATYPE_UNKNOWN;
}

DataStructType parse_delete_command(const char* command) {
    char type[10]; 

    if (sscanf(command, "delete %s", type) == 1) {
        if (strncmp(type, "list", 4) == 0) {
            return DATATYPE_LIST;
        } else if (strncmp(type, "bm", 2) == 0) {
            return DATATYPE_BITMAP;
        } else if (strncmp(type, "hash", 4) == 0) {
            return DATATYPE_HASHTABLE;
        }
    }
    return DATATYPE_UNKNOWN;
}



/*****
 * 
            .__  .__          __   
            |  | |__| _______/  |_ 
            |  | |  |/  ___/\   __\
            |  |_|  |\___ \  |  |  
            |____/__/_____ > |__|                
*/

/*

*/

void parse_list_global(const char* command, size_t* listIdx){
    char dummy[100];
    sscanf(command, "%s list list%zu", dummy, listIdx);
}


/**
 * instruction
 * const char* command = "list_insert list0 1 2"; // 입력 명령어
    int listIdx, before, elem;
    
    // 함수 호출: 명령어 파싱
    parseCommand(command, &listIdx, &before, &elem);
*/
void parse_list_insert(const char* command, size_t* listIdx, size_t* before, size_t* elem) {
    
    sscanf(command, "list_insert list%zu %zu %zu", listIdx, before, elem);
    
}


/**
 * instruction
 * 
 * int targetListIdx, objectListIdx, targetIdx, start, end;
 * 
 * targetListIdx: splice된 자료구조를 삽입할 idx
 * objectListIdx: splice를 할 리스트
 * targetIdx: targetList의 삽입될 idx
 * start: obejectList에서 splice를 할 시작 idx
 * end: end point
*/
void parse_list_splice(const char* command, size_t* targetListIdx, size_t* objectListIdx, size_t* targetIdx, size_t* start, size_t* end){
    //list_splice list0 2 list1 1 4
    sscanf(command, "list_splice list%zu %zu list%zu %zu %zu", targetListIdx, targetIdx, objectListIdx, start, end);
}

/**
 * instruction
 * parser for list_pust functions
 * listIdx: target list's idx
 * data: data to push
*/
void parse_list_push(const char* command, size_t *listIdx, int *data){
    
    char dummy[100];

    //input type list_push_front list0 1
    //input type list_push_back list0 1
    sscanf(command, "%s list%zu %d", dummy, listIdx, data);

    
}

/**
 * instruction
 * parser for single param input
 * 
 * only gets listIdx
*/
void parse_single_param(const char* command, size_t *listIdx){
    char listName[100];
    char dummy[100];

    sscanf(command, "%s %s", dummy, listName);

    sscanf(listName, "list%zu", listIdx);
}

/**
 * instruction
 * parser for list_insert_ordered
 * 
 * listIdx: destination list
 * data: data for insert
*/
void parse_list_insert_ordered(const char* command, size_t *listIdx, int *data){

    sscanf(command, "list_insert_ordered list%zu %d", listIdx, data);
}

/**
 * instruction
 * parser for list_swap
 * 
 * listIdx: destination list
 * a: swap list a
 * b: swap list b
*/
void parse_list_swap(const char* command, size_t* listIdx, size_t* a, size_t*b){
    char listName[100];

    sscanf(command, "list_swap %s %zu %zu", listName, a, b);

    sscanf(listName, "list%zu", listIdx);

}

void parse_list_remove(const char* command, size_t* listIdx, size_t* targetIdx){
    char listName[100];

    sscanf(command, "list_remove %s %zu", listName, targetIdx);
    sscanf(listName, "list%zu", listIdx);
}

void parse_list_unique(const char* command, size_t *listIdx, size_t *duplicateIdx){
    char listName[100];
    char duplicateName[100];

    sscanf(command, "list_unique %s %s", listName, duplicateName);

    sscanf(listName, "list%zu", listIdx);
    sscanf(duplicateName, "list%zu", duplicateIdx);
}


/**
 *
            .__                  .__        
            |  |__ _____    _____|  |__     
            |  |  \\__  \  /  ___/  |  \    
            |   Y  \/ __ \_\___ \|   Y  \   
            |___|  (____  /____  >___|  /   
                \/     \/     \/     \/    
            __        ___.   .__          
            _/  |______ \_ |__ |  |   ____  
            \   __\__  \ | __ \|  | _/ __ \ 
            |  |  / __ \| \_\ \  |_\  ___/ 
            |__| (____  /___  /____/\___  >
                    \/    \/          \/ 
*/

/*

*/

void parse_hash_global(const char* command, size_t* hashIdx){
    char hashName[100];
    char dummy[100];
    sscanf(command, "%s hash %s", dummy, hashName);

    sscanf(hashName, "hash%zu", hashIdx);
}

/*cmd examples
//double
hash_insert hash0 1
hash_apply hash0 square
hash_apply hash0 triple
hash_delete hash0 10
hash_find hash0 10
hash_replace hash0 10
*/
void parse_hash_double(char *command, char *cmd , size_t *hashIdx, char *secondArg){
    char firstArg[100];
    sscanf(command, "%s %s %s", cmd, firstArg, secondArg);
    sscanf(firstArg, "hash%zu", hashIdx);
}
/*
//single
hash_empty hash0
hash_size hash0
hash_clear hash0
*/
void parse_hash_single(char *command, char *cmd, size_t *hashIdx){
    char firstArg[100];
    sscanf(command, "%s %s", cmd, firstArg);
    sscanf(firstArg, "hash%zu", hashIdx);
}

/*
create hashtable hash0
*/
void parse_hash_create(char *command, size_t *hashIdx){
    char firstArg[100];
    sscanf(command, "create hashtable %s", firstArg);
    sscanf(firstArg, "hash%zu", hashIdx);
}




/**
 * 
        ___.   .__  __                         
        \_ |__ |__|/  |_  _____ _____  ______  
        | __ \|  \   __\/     \\__  \ \____ \ 
        | \_\ \  ||  | |  Y Y  \/ __ \|  |_> >
        |___  /__||__| |__|_|  (____  /   __/ 
            \/               \/     \/|__|    
*/

/**
 * example commands
 * 
 * create bitmap bm0 16
 * 
 * //none param
 * bitmap_dump bm0
 * bitmap_size bm3
 */
void parse_bitmap_noparam(char *command, char *cmd, size_t *bmIdx){
    char firstArg[100];
    sscanf(command, "%s %s", cmd, firstArg);
    sscanf(firstArg, "bm%zu", bmIdx);
}
 
/** //single param
 * bitmap_mark bm0 0 
 * bitmap_expand bm0 2
 * bitmap_set_all bm0 false
 * bitmap_flip bm0 4
 * bitmap_reset bm0 7 
 * bitmap_test bm0 8
 */ 
void parse_bitmap_single(char *command, char *cmd, size_t *bmIdx, char *a){
    char firstArg[100];
    sscanf(command, "%s %s %s", cmd, firstArg, a);
    sscanf(firstArg, "bm%zu", bmIdx);
}


/** //double param
 * bitmap_all bm0 0 1
 * bitmap_any bm0 0 1
 * bitmap_none bm0 0 1
 * bitmap_set bm0 0 true
 */
void parse_bitmap_double(char *command, char *cmd, size_t *bmIdx, char *a, char *b){
    char firstArg[100];
    sscanf(command, "%s %s %s %s", cmd, firstArg, a, b);
    sscanf(firstArg, "bm%zu", bmIdx);
}


/* //triple param
 * bitmap_contains bm0 0 2 true
 * bitmap_count bm0 0 8 true
 * bitmap_scan_and_flip bm0 0 1 true
 * bitmap_scan bm0 0 3 true 
 * bitmap_set_multiple bm0 0 4 true
*/
void parse_bitmap_triple(char *command, char *cmd, size_t *bmIdx, char *a, char *b, char *TF){
    char firstArg[100];

    sscanf(command, "%s %s %s %s %s", cmd, firstArg, a, b, TF);
    sscanf(firstArg, "bm%zu", bmIdx);
}


void parse_create_bitmap(char *command, char *cmd, size_t *bmIdx, size_t *bit_cnt){
    sscanf(command, "%s bitmap bm%zu %zu", cmd, bmIdx, bit_cnt);
}