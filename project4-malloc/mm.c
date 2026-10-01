/*
 * Simple, 32-bit and 64-bit clean allocator based on implicit free
 * lists, first-fit placement, and boundary tag coalescing, as described
 * in the CS:APP3e text. Blocks must be aligned to doubleword (8 byte)
 * boundaries. Minimum block size is 16 bytes.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your information in the following struct.
 ********************************************************/
team_t team = {
    /* Your student ID */
    "-",
    /* Your full name*/
    "Jongkyoung Han",
    /* Your email address */
    "toragonite@gmail.com",
};

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */            // line:vm:mm:beginconst
#define DSIZE 8                                                      /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */ // line:vm:mm:endconst
#define ALIGNMENT 8                                                  /* single word (4) or double word (8) alignment */
#define PLACE_THRESHOLD ALIGNMENT << 3                               /* Threshold for place function: modify to find changes */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc)) // line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))              // line:vm:mm:get
#define PUT(p, val) (*(unsigned int *)(p) = (val)) // line:vm:mm:put

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7) // line:vm:mm:getsize
#define GET_ALLOC(p) (GET(p) & 0x1) // line:vm:mm:getalloc

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)                      // line:vm:mm:hdrp
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // line:vm:mm:ftrp

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // line:vm:mm:nextblkp
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // line:vm:mm:prevblkp

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define NEXT_BLK(ptr) (*(char **)(ptr))
#define PREV_BLK(ptr) (*(char **)(ptr + WSIZE))
#define BLOCK_SIZE(ptr) (GET_SIZE(HDRP(ptr)))
#define SET(p, ptr) (*(uintptr_t *)(p) = (uintptr_t)(ptr))

#define PACK_MACRO(bp, size, alloc)       \
    do                                    \
    {                                     \
        PUT(HDRP(bp), PACK(size, alloc)); \
        PUT(FTRP(bp), PACK(size, alloc)); \
    } while (0)

#define SPLIT_MACRO(bp, size, free)         \
    do                                      \
    {                                       \
        PACK_MACRO(bp, size, 1);            \
        PACK_MACRO(NEXT_BLKP(bp), free, 0); \
        insert(free, NEXT_BLKP(bp));        \
    } while (0)

#define NEXT_NOT_ALLOC(ptr) (!GET_ALLOC(HDRP(NEXT_BLKP(ptr))) || !BLOCK_SIZE(NEXT_BLKP(ptr)))

/* Number of segregated lists */
#define LISTS 10

/* Array of segregated free lists, stored within the heap space */
#define SEG_LIST(i) (*(char **)((char *)segregated_listp + (i * WSIZE)))

/* Adjusted macros */
#define GET_LIST_INDEX(size)             \
    ((size) <= 16 ? 0 : (size) <= 32 ? 1 \
                    : (size) <= 64   ? 2 \
                    : (size) <= 128  ? 3 \
                    : (size) <= 256  ? 4 \
                    : (size) <= 512  ? 5 \
                    : (size) <= 768  ? 6 \
                    : (size) <= 1024 ? 7 \
                    : (size) <= 1536 ? 8 \
                                     : 9)

/* $end mallocmacros */

/* Global variables */
static void *heap_listp = NULL;       // Empty list
static void *segregated_listp = NULL; // Segregated list

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static char *dyn_extend_heap(char *bp, size_t asize, int *remain);

static char *fit_block(size_t asize);
static void *place(void *bp, size_t asize); // Modified return type
static void *coalesce(void *bp);
static void insert(size_t size, char *ptr);
static void pop(char *ptr);

int mm_init(void)
{
    if ((heap_listp = mem_sbrk((LISTS + 4) * WSIZE)) == (void *)-1)
        return -1;

    segregated_listp = heap_listp;

    for (int i = 0; i < LISTS; i++)
    {
        SEG_LIST(i) = NULL;
    }

    heap_listp += (LISTS * WSIZE);
    PUT(heap_listp, 0);                            // Alignment padding
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); // Prologue header
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); // Prologue footer
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     // Epilogue header
    heap_listp += (2 * WSIZE);

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

// Allocate a block with at least size bytes of payload
void *mm_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    size_t asize = MAX(ALIGN(size + DSIZE), DSIZE << 1);
    char *bp = fit_block(asize);
    bp = dyn_extend_heap(bp, asize, NULL);

    if (bp == NULL)
        return NULL;

    return place(bp, asize);
}

// Free a block
void mm_free(void *bp)
{
    size_t size = BLOCK_SIZE(bp);
    PACK_MACRO(bp, size, 0);
    insert(size, bp);
    coalesce(bp);
}

// Reallocate a block
void *mm_realloc(void *ptr, size_t size)
{
    if (!size)
        return NULL;

    size_t asize = (size <= DSIZE) ? (DSIZE << 1) + (1 << 7) : ALIGN(size + DSIZE) + (1 << 7);

    if (BLOCK_SIZE(ptr) >= asize)
        return ptr;

    if (NEXT_NOT_ALLOC(ptr))
    {
        int remain = BLOCK_SIZE(ptr) + BLOCK_SIZE(NEXT_BLKP(ptr)) - asize;
        ptr = dyn_extend_heap(ptr, asize, &remain);

        if (ptr == NULL)
            return NULL;

        pop(NEXT_BLKP(ptr));
        PACK_MACRO(ptr, asize + remain, 1);
        return ptr;
    }

    size_t old_size = BLOCK_SIZE(ptr);
    size_t copy_size = MIN(old_size, size);

    // Temporarily save the data
    char *temp_data = mm_malloc(copy_size); // can change to malloc
    memcpy(temp_data, ptr, copy_size);

    // Free the old block
    mm_free(ptr);

    // Allocate new block
    void *newptr = mm_malloc(size);

    if (newptr != NULL)
    {
        // Copy the data to the new block
        memcpy(newptr, temp_data, copy_size);
    }

    // Free the temp buffer
    mm_free(temp_data);

    return newptr;
}

// Extend the heap with a new free block
static void *extend_heap(size_t words)
{
    void *bp;

    if ((bp = mem_sbrk(ALIGN(words))) == ((void *)-1))
        return NULL;

    PACK_MACRO(bp, ALIGN(words), 0);      // Initialize free block header/footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // New epilogue header
    insert(ALIGN(words), bp);             // Insert the new block into the free list

    return coalesce(bp); // Coalesce if the previous block was free
}

// Dynamically extend the heap
static char *dyn_extend_heap(char *bp, size_t asize, int *remain)
{
    if (bp == NULL)
    {
        size_t extend_size = MAX(asize, CHUNKSIZE);
        bp = extend_heap(extend_size);
    }
    else if (remain && *remain < 0)
    {
        int extendsize = MAX(CHUNKSIZE, -(*remain));
        if (extend_heap(extendsize) == NULL)
            return NULL;
        *remain += extendsize;
    }
    return bp;
}

// Find a fit for a block with asize bytes
static char *fit_block(size_t asize)
{
    int index = GET_LIST_INDEX(asize);
    char *list = SEG_LIST(index);

    while (index < LISTS)
    {
        while (list != NULL)
        {
            if (!GET_ALLOC(HDRP(list)) && (asize <= GET_SIZE(HDRP(list))))
                return list;
            list = NEXT_BLK(list);
        }
        index++;
        list = SEG_LIST(index);
    }
    return NULL;
}

// Place a block of asize bytes at start of free block bp
static void *place(void *bp, size_t asize)
{
    pop(bp);

    size_t csize = BLOCK_SIZE(bp);
    size_t free_remain = csize - asize;
    size_t min_block_size = DSIZE << 1;

    if (min_block_size >= free_remain)
    {
        asize = csize;
    }
    else if (asize < PLACE_THRESHOLD)
    {
        SPLIT_MACRO(bp, asize, free_remain);
    }
    else
    {
        PACK_MACRO(bp, free_remain, 0);
        insert(free_remain, bp);
        bp = NEXT_BLKP(bp);
    }

    PACK_MACRO(bp, asize, 1);
    return bp;
}

// Insert a block into the free list
static void insert(size_t size, char *ptr)
{
    int index = GET_LIST_INDEX(size);
    char *list = SEG_LIST(index);

    if (list != NULL)
    {
        SET(ptr, list);
        SET(list + WSIZE, ptr);
    }
    else
    {
        SET(ptr, NULL);
    }
    SEG_LIST(index) = ptr;
    SET(ptr + WSIZE, NULL);
}

// Remove a block from the free list
static void pop(char *ptr)
{
    int index = GET_LIST_INDEX(BLOCK_SIZE(ptr));
    if (PREV_BLK(ptr) != NULL)
    {
        SET(PREV_BLK(ptr), NEXT_BLK(ptr));
        if (NEXT_BLK(ptr))
            SET(NEXT_BLK(ptr) + WSIZE, PREV_BLK(ptr));
    }
    else
    {
        SEG_LIST(index) = NEXT_BLK(ptr);
        if (SEG_LIST(index))
            SET(SEG_LIST(index) + WSIZE, NULL);
    }
}

// Coalesce free blocks
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = BLOCK_SIZE(bp);

    if (prev_alloc && next_alloc)
    { /* Case 1 */
        return bp;
    }
    else if (prev_alloc && !next_alloc)
    { /* Case 2 */
        pop(bp);
        pop(NEXT_BLKP(bp));
        size += BLOCK_SIZE(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc)
    { /* Case 3 */
        pop(bp);
        pop(PREV_BLKP(bp));
        size += BLOCK_SIZE(PREV_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else
    { /* Case 4 */
        pop(bp);
        pop(PREV_BLKP(bp));
        pop(NEXT_BLKP(bp));
        size += BLOCK_SIZE(PREV_BLKP(bp)) + BLOCK_SIZE(NEXT_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    insert(size, bp);

    return bp;
}