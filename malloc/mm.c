/* 
 * mm-implicit.c -  Simple allocator based on implicit free lists, 
 *                  first fit placement, and boundary tag coalescing. 
 *
 * Each block has header and footer of the form:
 * 
 *      31                     3  2  1  0 
 *      -----------------------------------
 *     | s  s  s  s  ... s  s  s  0  0  a/f
 *      ----------------------------------- 
 * 
 * where s are the meaningful size bits and a/f is set 
 * iff the block is allocated. The list has the following form:
 *
 * begin                                                          end
 * heap                                                           heap  
 *  -----------------------------------------------------------------   
 * |  pad   | hdr(8:a) | ftr(8:a) | zero or more usr blks | hdr(8:a) |
 *  -----------------------------------------------------------------
 *          |       prologue      |                       | epilogue |
 *          |         block       |                       | block    |
 *
 * The allocated prologue and epilogue blocks are overhead that
 * eliminate edge conditions during coalescing.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
  /* Team name */
  "Awesome",
  /* First member's full name */
  "Chakrya Ros",
  /* First member's email address */
  "chakrya.ros@colorado.edu",
  /* Second member's full name (leave blank if none) */
  "Muntaha Pasha",
  /* Second member's email address (leave blank if none) */
  "Muntaha.Pasha@colorado.edu"
};


/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define checkheap mm_checkheap
#endif /* def DRIVER */


/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
/* #define DEBUG  */
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif


/////////////////////////////////////////////////////////////////////////////
// Constants and macros
//
// These correspond to the material in Figure 9.43 of the text
// The macros have been turned into C++ inline functions to
// make debugging code easier.
//
/////////////////////////////////////////////////////////////////////////////
#define WSIZE       4       /* word size (bytes) */  
#define DSIZE       8       /* doubleword size (bytes) */
#define CHUNKSIZE  (1<<12)  /* initial heap size (bytes) */
#define OVERHEAD    8       /* overhead of header and footer (bytes) */


/* alignment  must be equal to integer size */
#define ALIGNMENT 8 //signle word(4) or double word (8)
#define MINIMUM   24 
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Given free list bp, find address of next and previous free list ptrs */
#define NEXT_FREEP(bp)  (*(char **)((char *)(bp) + DSIZE))
#define PREV_FREEP(bp)  (*(char **)((char *)(bp)))

static inline  uint32_t  MAX(int x, int y) {
  return x > y ? x : y;
}

//
// Pack a size and allocated bit into a word
// We mask of the "alloc" field to insure only
// the lower bit is used
//
static inline uint32_t PACK(uint32_t size, int alloc) {
  return ((size) | (alloc & 0x1));
}

//
// Read and write a word at address p
//
static inline uint32_t GET(void *p) { return  *(uint32_t *)p; }
static inline void PUT( void *p, uint32_t val)
{
  *((uint32_t *)p) = val;
}

//
// Read the size and allocated fields from address p
//
static inline uint32_t GET_SIZE( void *p )  { 
  return GET(p) & ~0x7;
}

static inline int GET_ALLOC( void *p  ) {
  return GET(p) & 0x1;
}

//
// Given block ptr bp, compute address of its header and footer
//
static inline void *HDRP(void *bp) {

  return ( (char *)bp) - WSIZE;
}
static inline void *FTRP(void *bp) {
  return ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE);
}

//
// Given block ptr bp, compute address of next and previous blocks
//
static inline void *NEXT_BLKP(void *bp) {
  return  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)));
}

static inline void* PREV_BLKP(void *bp){
  return  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)));
}

    
/////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
static char *heap_listp = 0;  /* pointer to first block */ 
static char *free_listp = 0;   /* Pointer to list of free blocks */ 


//
// function prototypes for internal helper routines
//
static void *extend_heap(uint32_t words);
static void place(void *bp, uint32_t asize);
static void *find_fit(uint32_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);

/*My own function helper*/
static void InsertFreeBlock(void *bp);
static void RemoveFreeBlock(void *bp);
static void FreeList(void *bp);


//
// mm_init - Initialize the memory manager 
//mm_init function gets four words from the memeory system
//and initializes them to create empty free list
// return -1 on error, 0 on success
int mm_init(void) 
{
  //mem_sbrk -extends the heap by incr bytes and 
   //return the start address of new area
    //in this case just check, if sbrk fail, ran out of memory
    
    /*create the initial empty heap(free list) */
    if((heap_listp = mem_sbrk(2*MINIMUM))== (void *)-1)
        return -1;
    PUT(heap_listp, 0);  //Alignment padding
    PUT(heap_listp + (1*WSIZE), PACK(MINIMUM, 1)); //prologue header
    PUT(heap_listp + (2*WSIZE), 0); /* Previous pointer */
    PUT(heap_listp + (3*WSIZE), 0);    /* Next Pointer */

    PUT(heap_listp + MINIMUM, PACK(MINIMUM, 1));      /* Prologue footer */
    PUT(heap_listp + MINIMUM + WSIZE, PACK(0, 1));    /* Epilogue Header */
     free_listp = heap_listp + DSIZE;
    //It call the extend_heap function that extend the heap by CHUNKSIZE bytes and
    //and creates initial free block
    /*Extend the empty heap with a free block of CHUNKSIZE byes*/
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
    {
        return -1;
    }
    
  return 0;
}


//
// extend_heap - Extend heap with free block and return its block pointer
//
//
static void *extend_heap(uint32_t words) 
{
    char *bp;
    uint32_t size;
    /*Allocate an even number of words to maintain alignment */
    //the extend_head function is invoked in two different circumstacnce
    //(1) when the heap is initialize
    //(2) when mm_malloc is unable to find a suitable fit
    //to maintain alignment, extend_head rounds up the requested size to 
    //the nearest multiple of 2 words (8bytes) and then request the additional
    //heap space from the memeory system (218-222)
    size = (words % 2)? (words + 1) * WSIZE : words * WSIZE;
    if (size < MINIMUM)
       size = MINIMUM;
    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    
    //the heap begins on a double word aligned boundary
    //every call to extend_head return a block whose size is an intgral number
    //of double word
    //the header become the header of new free block
    PUT(HDRP(bp), PACK(size, 0));   //Free block header
    PUT(FTRP(bp), PACK(size, 0));   //Free block footer
    
    //the last word of the chunk become the new epilogue block header
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); //new epilogue header
    
    //in the case that previous heap was terminated by a free block
    //we call Coaliesece function to merge the two free block
    //and return the block pointer of the merged block
    //Coaliesce if the previous block was free
  return coalesce(bp);
}
//
// find_fit - Find a fit for a block with asize bytes 
//
static void *find_fit(uint32_t asize)
{
  //First-fit search
    void *bp;
    
    //Iterate over free list till we find the first block that fits
    for(bp = free_listp; GET_SIZE(HDRP(bp))> 0; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
            return bp;
        }
    }
    return NULL; /* no fit */
}

// 
// mm_free - Free a block 
//
void mm_free(void *bp)
{
  
    if(bp == 0)
        return;
    
    uint32_t size = GET_SIZE(HDRP(bp));
    if(heap_listp == 0)
    {
        mm_init();
    }
    
    //Set header, footer alloc bits to zero
    PUT(HDRP(bp), PACK(size, 0)); //Free block header
    PUT(FTRP(bp), PACK(size, 0)); //Free block footer
    
    coalesce(bp);
    
}

//
// coalesce - boundary tag coalescing. Return ptr to coalesced block
//
static void *coalesce(void *bp) 
{
    uint32_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    /* When at front, prev block is same as curr block :( Nasty bug it was */
    if(PREV_BLKP(bp) == bp)
    {
        prev_alloc = 1;
    }
    
    uint32_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    uint32_t size = GET_SIZE(HDRP(bp));
    
    //case1
    
    /* case2 */
    if(prev_alloc && !next_alloc){
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        
        RemoveFreeBlock(NEXT_BLKP(bp)); //remove next block
        
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    //case 3
    else if(!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        RemoveFreeBlock(PREV_BLKP(bp)); //remove previous block
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    
    //CASE 4
    else if(!prev_alloc && !next_alloc) {
         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        RemoveFreeBlock(NEXT_BLKP(bp)); //remove next block
        RemoveFreeBlock(PREV_BLKP(bp)); //remove previous block
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    
    /*Insert Coalesced block in free list */
    InsertFreeBlock(bp);
    return bp;
}

//
// mm_malloc - Allocate a block with at least size bytes of payload 
//
void *mm_malloc(uint32_t size) 
{
  //
  // You need to provide this
  //
    uint32_t asize;    //adjusted block size
    uint32_t extend_size;   //to expend heap if no fit
    char *bp;
    
    if(heap_listp == 0)
    {
        mm_init();
    }
    
    //Ignore NON-positive value
    if(size == 0)
    {
        return NULL;
    }
    
    //adjust block size to include overhead and alignment reqs
    asize = MAX(ALIGN(size) + DSIZE, MINIMUM);
    
    //Search the free list for a fit
    if((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    
    //No fit found. Get more memory and place the block
    extend_size = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extend_size/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    
    return bp;
  
} 

//
//
// Practice problem 9.9
//
// place - Place block of asize bytes at start of free block bp 
//         and split if remainder would be at least minimum block size
//
static void place(void *bp, uint32_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    
    if((csize - asize) >= (MINIMUM))
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        coalesce(bp);
    }
    else
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}


//
// mm_realloc -- implemented for you
//
void *mm_realloc(void *ptr, uint32_t size)
{

  void *newp;
  uint32_t copySize;

  newp = mm_malloc(size);
  if (newp == NULL) {
    printf("ERROR: mm_malloc failed in mm_realloc\n");
    exit(1);
  }
  copySize = GET_SIZE(HDRP(ptr));
  if (size < copySize) {
    copySize = size;
  }
  memcpy(newp, ptr, copySize);
  mm_free(ptr);
  return newp;
    /*
    void *newbp;
    uint32_t oldsize, asize;
 
    asize = MAX(ALIGN(size) + DSIZE, MINIMUM);
    //if size is 0, it's free and we return NULL
    if(size == 0)
    {
        mm_free(ptr);
        return 0;
    }
    
    //if old pointer is null, it's malloc
    if(ptr == NULL)
    {
        return malloc(size);
    }
    
    //get original block size
    oldsize = GET_SIZE(HDRP(ptr));
    
    //if new size is same as old, just return
    if(oldsize == asize)
    {
        return ptr;
    }
    newbp = mm_malloc(size);
    
    //if realloc funtion fail, the original block is left untouched
    if(!newbp)
        return 0;
    
    //copy the old data
    if(size < oldsize)
    {
         oldsize = size;
    }
       
    memcpy(newbp, ptr, oldsize);
    
    //free the old block
    mm_free(ptr);
    return newbp;
    */
}

//
// mm_checkheap - Check the heap for consistency 
//
void mm_checkheap(int verbose) 
{
  //
  // This provided implementation assumes you're using the structure
  // of the sample solution in the text. If not, omit this code
  // and provide your own mm_checkheap
  //
  void *bp = heap_listp;
  
  if (verbose) {
    printf("Heap (%p):\n", heap_listp);
  }

  if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp))) {
	printf("Bad prologue header\n");
  }
  checkblock(heap_listp);

  for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if (verbose)  {
      printblock(bp);
    }
    checkblock(bp);
  }
     
  if (verbose) {
    printblock(bp);
  }

  if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp)))) {
    printf("Bad epilogue header\n");
  }
}

static void printblock(void *bp) 
{
  uint32_t hsize, halloc, fsize, falloc;

  hsize = GET_SIZE(HDRP(bp));
  halloc = GET_ALLOC(HDRP(bp));  
  fsize = GET_SIZE(FTRP(bp));
  falloc = GET_ALLOC(FTRP(bp));  
    
  if (hsize == 0) {
    printf("%p: EOL\n", bp);
    return;
  }

  printf("%p: header: [%d:%c] footer: [%d:%c]\n",
	 bp, 
	 (int) hsize, (halloc ? 'a' : 'f'), 
	 (int) fsize, (falloc ? 'a' : 'f')); 
}

static void checkblock(void *bp) 
{
  if ((uintptr_t)bp % 8) {
    printf("Error: %p is not doubleword aligned\n", bp);
  }
  if (GET(HDRP(bp)) != GET(FTRP(bp))) {
    printf("Error: header does not match footer\n");
  }
}

/* 
 * insertfreeblock - Append free block to the front of list
 *                   Link new block to current free list and
 *                   set new block as top of current list. 
 */
static void InsertFreeBlock(void *bp)
{
    //if free list is null, set it to block point
    if(free_listp == NULL)
    {
        NEXT_FREEP(bp)=NULL;
        PREV_FREEP(bp)=NULL;
        free_listp = bp;
        return;
    }
    
    //set curr next to head of list
     PREV_FREEP(bp) = NULL;
     NEXT_FREEP(bp) = free_listp;
     PREV_FREEP(free_listp)= bp;
    
     free_listp = bp;
}
/* 
 * removefreeblock - Remove freed block from the list. 
 *      Coded to reflect all possible cases to avoid ambiguity
 *      CASE 1: Remove block with only one block in the list
 *      CASE 2: Block is top of list: Set next block as new top. 
 *      CASE 3: Block is end of free list: Set prev block as new end
 *      CASE 4: Freed block is a middle one: Link Prev block to next block
 *
 */
static void RemoveFreeBlock(void *bp)
{
    /* Case when we have nothing in list */
    if (free_listp == 0)
        return;


    /* Case 1 */
    if ((PREV_FREEP(bp) == NULL) && (NEXT_FREEP(bp) == NULL)) {
        free_listp = 0;
    }

    /* Case 2 */
    else if ((PREV_FREEP(bp) == NULL) && (NEXT_FREEP(bp) != NULL)) {
        free_listp = NEXT_FREEP(bp);
        PREV_FREEP(free_listp) = NULL; 
         
    }

    /* Case 3 */
    else if ((PREV_FREEP(bp) != NULL) && (NEXT_FREEP(bp) == NULL)) {
        /* Last one now points to NULL */
        NEXT_FREEP(PREV_FREEP(bp)) = NULL; 
      
    }

    /* Case 4 */
    else if ((PREV_FREEP(bp) != NULL) && (NEXT_FREEP(bp) != NULL)) {
        PREV_FREEP(NEXT_FREEP(bp)) = PREV_FREEP(bp);  
        NEXT_FREEP(PREV_FREEP(bp)) = NEXT_FREEP(bp); 
    }
}

