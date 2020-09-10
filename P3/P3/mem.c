////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      None
// Semester:         CS 354 Spring 2019
//
// Author:           Yuhao Liu
// Email:            liu697@wisc.edu
// CS Login:         yuhao
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          None
//
// Online sources:   None
//
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct block_header {
        int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit 
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    * 
    * End Mark: 
    *  The end of the available memory is indicated using a size_status of 1.
    * 
    * Examples:
    * 
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    * 
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} block_header;         

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

block_header *start_block = NULL;

/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use BEST-FIT PLACEMENT POLICY to find the block closest to the required block size
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic.
 */
void* Alloc_Mem(int size) {
    if (size < 0) {
        fprintf(stderr, "Error: Cannot allocate negative sized block\n");
        return NULL;
    }
    //calculate the actual block size to allocate
    int padSize = 8 - (size % 8);
    int allocSize = size + padSize; //actual size to alloc
    
    //best-fit traversal
    int diff; //tracks difference
    int bestDiff = 2147483647; //int_max
    int curSize; //# bytes inside current block
    int preUsed; //previous block usage
    int curUsed; //current block usage
    char *bestFit = NULL; //pointer to best-fit block
    block_header *curHeader = start_block; //pointer used for traversal
    
    //traverse until reaching the end of the page
    while (curHeader -> size_status != 1) {
        curSize = curHeader -> size_status;
        if (curSize & 1) {//if current block has been allocated
            curUsed = 1;
            curSize -= 1;
        } else {
            curUsed = 0;
        }
        if (curSize & 2) { //if previous block has been allocated
            preUsed = 1;
            curSize -=2;
        } else {
            preUsed = 0;
        }
        
        //we have now erased p-bit and a-bit
        diff = curSize - allocSize;
        if ((curUsed == 0) && (diff >= 0)) {//if this block is sufficiently large
            if (diff < bestDiff) {
                bestDiff = diff; //update global best
                bestFit = (char*) curHeader;
            }
            if (bestDiff == 0) {
                break; //stop if an exact match is found
            }
        }
        curHeader = (block_header*) ((char*) curHeader + curSize);
    }
    //if unable to find such block
    if (bestFit == NULL){
        return NULL;
    }
    //if block is found, check whether to split the block
    block_header *bestHeader = (block_header*) (bestFit); //start of best-fit block
    char *payload = bestFit + sizeof(block_header); // start of best-fit payload
    curSize = bestHeader -> size_status;
    
    //updating p bit
    if (curSize & 2) {
        preUsed = 1;
        curSize -= 2;
    } else {
        preUsed = 0;
    }
    
    bestHeader -> size_status = allocSize + 1; //set a-bit to 1
    if (preUsed == 1) {
        bestHeader -> size_status += 2;
    }
    
    //dump mem
    diff = curSize - allocSize;
    block_header *nextHeader = (block_header*) (bestFit + curSize);
    if (diff == 0) {
        //check next block's p-bit
        if (nextHeader -> size_status & 2) {
            return payload;
        } else {
            nextHeader -> size_status += 2;
            return payload;
        }
    } else { //handling split
        block_header *splitHeader = (block_header*) (bestFit + allocSize);
        splitHeader -> size_status = diff + 2; //mark split block's pbit 1, abit 0
        block_header *splitFooter = (block_header*)((char*) nextHeader - sizeof(block_header));
        splitFooter -> size_status = diff;
        return payload;
    }
}

/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */                    
int Free_Mem(void *ptr) {         
    if (ptr == NULL) {
        fprintf(stderr, "Error: cannot free a NULL pointer\n");
        return -1;
    }
    if ((int) ptr % 8 != 0) {
        fprintf(stderr, "Error: pointer address must be multiples of 8\n");
        return -1;
    }
    
    //check if the given ptr is in the heap
    block_header *end_block = start_block;
    while (end_block -> size_status != 1) {
        end_block = (block_header*)((char*) end_block + (8 * (end_block -> size_status / 8)));
    }
    if ((int) ptr < (int) start_block || (int) ptr >= (int) end_block){
        fprintf(stderr, "Error: pointer address is not in the heap\n");
        return -1;
    }
    
    block_header *curHeader = (block_header*)((char*) ptr - sizeof(block_header));
    int curSize = curHeader -> size_status;
    char* byteCurHeader = (char*)(curHeader); //convert to char pointer for fugure addressing computations
    int preUsed;
    int nextUsed;
    
    //check if the current block is freed and upadate size
    if (curSize & 1) {
        curSize -= 1;
    } else {
        fprintf(stderr, "Error: cannot free a block that is already freed\n");
        return -1;
    }
    
    //update prevBlock's usage info and size
    if (curSize & 2) {
        preUsed = 1;
        curSize -= 2;
    } else {
        preUsed = 0;
    }
    
    //now, curSize reflects acutal block size
    //check status of the next block
    block_header *nextHeader = (block_header*) (byteCurHeader + curSize);
    int nextSize = nextHeader -> size_status;
    int endOfPage;
    
    if (nextSize != 1) {
        endOfPage = 0;
        if (nextSize & 1) {
            nextUsed = 1; //if next block is used, cannot coalesce
        } else { //calculating block size
            nextUsed = 0;
            nextSize -= 2;
        }
    } else { //reaching end of page
        endOfPage = 1;
        nextUsed = 1;
    }
    
    block_header *preHeader;
    block_header *preFooter;
    int preSize;
    
    if (preUsed == 0) { //if prev block is empty BUT WHY?
        preFooter = (block_header*) (byteCurHeader - sizeof(block_header));
        preSize = preFooter -> size_status;
        preHeader = (block_header*) (byteCurHeader - preSize);
    }
    if (preUsed == 1){  //case 1: do not coalesce with previous
        //case 1a: do not coalesce at all
        if (nextUsed == 1) {
            //if next block is not end of heap, then next's update p-bit
            if (endOfPage == 0) {
                nextHeader -> size_status -= 2;
            }
            curHeader -> size_status -= 1;
            block_header *newFooter = (block_header*)(byteCurHeader + curSize - sizeof(block_header));
            newFooter -> size_status = curSize;
        } else { //case 1b: only coal. with next block
            curHeader -> size_status = curHeader -> size_status - 1 + nextSize;
            block_header * newFooter = (block_header*)
            (byteCurHeader + curSize - sizeof(block_header));
            newFooter -> size_status = curSize;
        }
    } else { //case 2: coalesce with previous
        if (nextUsed == 1) { //case 2a: only coal. with prev
            if (endOfPage == 0) { //if next is not end-of-heap
                nextHeader -> size_status -= 2; //update p-bit
            }
            preHeader -> size_status += curSize;
            block_header *newFooter = (block_header*)(byteCurHeader + curSize - sizeof(block_header));
            newFooter -> size_status = curSize + preSize;
        } else { //case 2b: coal. both
            preHeader -> size_status = preHeader -> size_status + curSize + nextSize;
            block_header *newFooter = (block_header*)(byteCurHeader + curSize + nextSize - sizeof(block_header));
            newFooter -> size_status = preSize + curSize + nextSize;
        }
    }
    return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int Init_Mem(int sizeOfRegion) {         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    block_header* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that start block meets 
    // double word alignement requirement
    start_block = (block_header*) space_ptr + 1;
    end_mark = (block_header*)((void*)start_block + alloc_size);
  
    // Setting up the header
    start_block->size_status = alloc_size;

    // Marking the previous block as used
    start_block->size_status += 2;

    // Setting up the end mark and marking it as used
    end_mark->size_status = 1;

    // Setting up the footer
    block_header *footer = (block_header*) ((char*)start_block + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}         
                 
/* 
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void Dump_Mem() {
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    block_header *current = start_block;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (block_header*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}         
