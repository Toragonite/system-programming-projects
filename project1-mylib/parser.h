#ifndef __MYLIB_PARSER_H
#define __MYLIB_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    DATATYPE_LIST,
    DATATYPE_BITMAP,
    DATATYPE_HASHTABLE,
    DATATYPE_UNKNOWN
} DataStructType;


DataStructType parse_create_command(const char* command);
DataStructType parse_dumpdata_command(const char* command);
DataStructType parse_delete_command(const char* command);

void parse_list_global(const char* command, size_t* listIdx);
void parse_list_insert(const char* command, size_t* listIdx, size_t* before, size_t* elem);
void parse_list_splice(const char* command, size_t* targetListIdx, size_t* objectListIdx, size_t* targetIdx, size_t* start, size_t* end);
void parse_list_push(const char* command, size_t *listIdx, int *data);
void parse_single_param(const char* command, size_t *listIdx);
void parse_list_insert_ordered(const char* command, size_t *listIdx, int *data);
void parse_list_remove(const char* command, size_t* listIdx, size_t* targetIdx);
void parse_list_unique(const char* command, size_t *listIdx, size_t *duplicateIdx);
void parse_list_swap(const char* command, size_t* listIdx, size_t* a, size_t*b);


void parse_hash_global(const char* command, size_t* hashIdx);
void parse_hash_double(char *command, char *cmd , size_t *hashIdx, char *secondArg);
void parse_hash_single(char *command, char *cmd, size_t *hashIdx);
void parse_hash_create(char *command, size_t *hashIdx);

void parse_bitmap_noparam(char *command, char *cmd, size_t *bmIdx);
void parse_bitmap_single(char *command, char *cmd, size_t *bmIdx, char *a);
void parse_bitmap_double(char *command, char *cmd, size_t *bmIdx, char *a, char *b);
void parse_bitmap_triple(char *command, char *cmd, size_t *bmIdx, char *a, char *b, char *TF);
void parse_create_bitmap(char *command, char *cmd, size_t *bmIdx, size_t *bit_cnt);

#endif //__MYLIB_PARSER_H
