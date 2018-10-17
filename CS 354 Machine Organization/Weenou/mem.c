#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct block_tag{

  int size_status;
  
 /*
  * Size of the block is always a multiple of 4
  * => last two bits are always zero - can be used to store other information
  *
  * LSB -> Least Significant Bit (Last Bit)
  * SLB -> Second Last Bit 
  * LSB = 0 => free block
  * LSB = 1 => allocated/busy block
  * SLB = 0 => previous block is free
  * SLB = 1 => previous block is allocated/busy
  * 
  * When used as the footer the last two bits should be zero
  */

 /*
  * Examples:
  * 
  * For a busy block with a payload of 24 bytes (i.e. 24 bytes data + an additional 4 bytes for header)
  * Header:
  * If the previous block is allocated, size_status should be set to 31
  * If the previous block is free, size_status should be set to 29
  * 
  * For a free block of size 28 bytes (including 4 bytes for header + 4 bytes for footer)
  * Header:
  * If the previous block is allocated, size_status should be set to 30
  * If the previous block is free, size_status should be set to 28
  * Footer:
  * size_status should be 28
  * 
  */

} block_tag;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
block_tag *first_block = NULL;

/* Global variable - Total available memory */
int total_mem_size = 0;

/* 
 * Function for allocating 'size' bytes
 * Returns address of the payload in the allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - If size is less than equal to 0 - Return NULL
 * - Round up size to a multiple of 4 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks when possible 
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size){
	/* Your code goes in here */

	if(size <= 0){
		return NULL;
	} 

	// If size is not multiple of 4, round up	
	while(size % 4 != 0){
		size++;
	}

	size += 4; // Add four to requested size to account for header

	//block_tag type

	// Temporary pointers
	block_tag * currBlock = first_block;
	block_tag * bestBlock = NULL;
	int bestSizeDiff = -1;
	int currSizeDiff = -1;
	int predAllocd = 2;
	int predToBestAllocd = NULL;

	// Iterate through list until out of scope
	while(currBlock - (first_block+total_mem_size/sizeof(block_tag)) < 0){	

		//Find real size without bit offset from status
		int realSize = currBlock->size_status & -4;

		//Test to see if block already allocated, if it is set predAllocd flag to true
		if(!(currBlock->size_status & 1)){
			currSizeDiff = realSize - size;

			if(bestBlock == NULL && (currSizeDiff >= 0)){
				bestSizeDiff = currSizeDiff;
				bestBlock = currBlock;
				predToBestAllocd = predAllocd;
			}

			else if(currSizeDiff >= 0 && (currSizeDiff < bestSizeDiff)){
				bestSizeDiff = currSizeDiff;
				bestBlock = currBlock;
				predToBestAllocd = predAllocd;
			}
	
			predAllocd = 0;	
		}
		else{
			predAllocd = 2; //bin value of 10b which can be used later with bitwise functions
		}
	
		currBlock += realSize/sizeof(block_tag);
	} 

	// Set block to allocated status if valid block
	if(bestBlock != NULL){
		bestBlock -> size_status = size | 1;
		bestBlock -> size_status = (bestBlock -> size_status) | predToBestAllocd;

		// Be sure to set the next blocks status to prev allocated if it exists
		currBlock = bestBlock + size/sizeof(block_tag);

		if(currBlock - (first_block+total_mem_size/sizeof(block_tag)) < 0){
			currBlock -> size_status = currBlock -> size_status | 2;
		}
	}		

	//Split remaining section of the block
	if(bestSizeDiff != 0 && bestBlock != NULL){
		currBlock = bestBlock + size/sizeof(block_tag);
		
		if(currBlock - (first_block+total_mem_size/sizeof(block_tag)) < 0){
			currBlock -> size_status = bestSizeDiff;			//set size
			currBlock -> size_status = currBlock -> size_status | 2;
		}
		//Establish footers
		if(bestSizeDiff > 0 && bestBlock != NULL){
			currBlock = currBlock + bestSizeDiff/sizeof(block_tag) - 4;
			
			if(currBlock - (first_block+total_mem_size/sizeof(block_tag)) < 0){
				currBlock -> size_status = bestSizeDiff;
				currBlock -> size_status = currBlock -> size_status + 2;
			}
		}

	}

	// Nullify pointers going out of scope
	currBlock = NULL;

	if(bestBlock == NULL){
		return NULL;
	}

	else{
		return bestBlock + 1;
	}
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the payload of the allocated block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not within the range of memory allocated by Mem_Init()
 * - Return -1 if ptr is not 4 byte aligned
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr){
	/* Your code goes in here */

	if(ptr == NULL){
		return -1;
	}

	// If not a multiple of 4, return -1
	if((int)ptr % 4 != 0){
		return -1;
	}
	
	// If out of range, return -1
	if(((block_tag*)ptr - first_block < 0) || ((block_tag*)ptr - (total_mem_size + first_block) > 0)){
		return -1;
	}

	// Temporaries to be used
	int hasCoaledBack = 0;					// flag for whether coalescing back has occurred
	int hasCoaledNext = 0;					// flag for whether coalescing into successor has occurred
	int predAllocd;						// flag if block before the full new free block is allocd
	int sizeTotal;						// Total size of block after coalescing occurred
	int realSizeCurr;					// real size of current working block
	int realSizeOrig;					// size of the original block freed
	block_tag * ptrHead = ((block_tag*)ptr) - 1;		// pointer to the header
	block_tag * newPtrHead;					// head of new block created after coalescing
	block_tag * newPtrFoot;					// head of new footer created after coalescing
	block_tag * backCoalPtr = ptrHead - 1;			// points to footer of predecessor block, then header of that block
	block_tag * nextCoalPtr;				// points to header of next block

	// Find actual size
	realSizeOrig = ptrHead -> size_status & -4;
	sizeTotal = realSizeOrig;
	predAllocd = (ptrHead -> size_status)&2;
	nextCoalPtr = ptrHead + realSizeOrig/sizeof(block_tag); // find header of next block

	// Set block as unallocated
	ptrHead -> size_status = ptrHead -> size_status & -2;

	// Look for block behind to coalesce with, test if inside heap AND backCoalPtr is a footer
	if((backCoalPtr - first_block > 0) && !(ptrHead->size_status & 2)){
		
		// Find real size of block preceding current block
		realSizeCurr = backCoalPtr -> size_status & -4;		
		predAllocd = (backCoalPtr -> size_status) & 2;
		sizeTotal += realSizeCurr;
		backCoalPtr = backCoalPtr - realSizeCurr/sizeof(block_tag) + 1;
		hasCoaledBack = 1;
	}

	// Look for block ahead to coalesce with
	if(nextCoalPtr - (first_block+total_mem_size/sizeof(block_tag)) < 0){

		if(!(nextCoalPtr->size_status & 1)){
			// Find real size of block succeeding current block
			realSizeCurr = nextCoalPtr -> size_status & -4;	
			sizeTotal += realSizeCurr;
			hasCoaledNext = 1;
		}

	}

	//Check if coalesced back to see where to place beginning of new block
	if(hasCoaledBack){
		newPtrHead = backCoalPtr;
	}
	
	else{
		newPtrHead = ptrHead;
	}

	// Define new header block
	newPtrHead -> size_status = sizeTotal;
	newPtrHead -> size_status += predAllocd;

	// Check if coalesced forward to see where to place end of new block
	if(hasCoaledNext){
		newPtrFoot = ptrHead + realSizeOrig/sizeof(block_tag);
		newPtrFoot = newPtrFoot + realSizeCurr/sizeof(block_tag) - 1;
	}

	else{
		newPtrFoot = ptrHead + realSizeOrig/sizeof(block_tag)  - 1;
	}
	
	// Finally define footer block
	newPtrFoot -> size_status = sizeTotal;
	newPtrFoot -> size_status = (newPtrFoot -> size_status)&-2;
	newPtrFoot -> size_status += predAllocd;

	// Check next block, set header to have previous block alloc to false
	newPtrFoot++;
	if((newPtrFoot - first_block > 0) && (newPtrFoot - (first_block + total_mem_size/sizeof(block_tag)) < 0 )){
		newPtrFoot -> size_status = (newPtrFoot -> size_status) & -3;
	}

	// Type safety: Nullify used pointers
	ptrHead = NULL; backCoalPtr = NULL; nextCoalPtr = NULL; newPtrHead = NULL;
	newPtrFoot = NULL;

	return 0;		
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion){
  int pagesize;
  int padsize;
  int fd;
  int alloc_size;
  void* space_ptr;
  static int allocated_once = 0;
  
  if(0 != allocated_once){
    fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
    return -1;
  }
  if(sizeOfRegion <= 0){
    fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
    return -1;
  }

  // Get the pagesize
  pagesize = getpagesize();

  // Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize
  padsize = sizeOfRegion % pagesize;
  padsize = (pagesize - padsize) % pagesize;

  alloc_size = sizeOfRegion + padsize;

  // Using mmap to allocate memory
  fd = open("/dev/zero", O_RDWR);
  if(-1 == fd){
    fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
    return -1;
  }
  space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (MAP_FAILED == space_ptr){
    fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
    allocated_once = 0;
    return -1;
  }
  
  allocated_once = 1;
  
  // Intialising total available memory size
  total_mem_size = alloc_size;

  // To begin with there is only one big free block
  first_block = (block_tag*) space_ptr;
  
  // Setting up the header
  first_block->size_status = alloc_size;
  // Marking the previous block as busy
  first_block->size_status += 2;

  // Setting up the footer
  block_tag *footer = (block_tag*)((char*)first_block + alloc_size - 4);
  footer->size_status = alloc_size;
  
  return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header)(including the header/footer)
 */ 
void Mem_Dump() {
  int counter;
  char status[5];
  char p_status[5];
  char *t_begin = NULL;
  char *t_end = NULL;
  int t_size;

  block_tag *current = first_block;
  counter = 1;

  int busy_size = 0;
  int free_size = 0;
  int is_busy = -1;

  fprintf(stdout,"************************************Block list***********************************\n");
  fprintf(stdout,"No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  
  while(current < (block_tag*)((char*)first_block + total_mem_size)){

    t_begin = (char*)current;
    
    t_size = current->size_status;
    
    if(t_size & 1){
      // LSB = 1 => busy block
      strcpy(status,"Busy");
      is_busy = 1;
      t_size = t_size - 1;
    }
    else{
      strcpy(status,"Free");
      is_busy = 0;
    }

    if(t_size & 2){
      strcpy(p_status,"Busy");
      t_size = t_size - 2;
    }
    else strcpy(p_status,"Free");

    if (is_busy) busy_size += t_size;
    else free_size += t_size;

    t_end = t_begin + t_size - 1;
    
    fprintf(stdout,"%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n",counter,status,p_status,
                    (unsigned long int)t_begin,(unsigned long int)t_end,t_size);
    
    current = (block_tag*)((char*)current + t_size);
    counter = counter + 1;
  }
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  fprintf(stdout,"*********************************************************************************\n");

  fprintf(stdout,"Total busy size = %d\n",busy_size);
  fprintf(stdout,"Total free size = %d\n",free_size);
  fprintf(stdout,"Total size = %d\n",busy_size+free_size);
  fprintf(stdout,"*********************************************************************************\n");
  fflush(stdout);
  return;
}
