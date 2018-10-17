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
typedef struct blk_hdr {                         
        int size_status;
  
    /*
    * Size of the block is always a multiple of 8
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
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 2f
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {                      
	// Your code goes in here
	blk_hdr* store = first_blk;//points to the header of best block 
	blk_hdr* current = first_blk;
	int blk_size = sizeof(blk_hdr*);
	if(size<=0) return NULL;
	if((size+blk_size)%8!=0) size = (8-(size+blk_size)%8)+size;//rounding(size now includes the size of head and the size of the payload)
	size += blk_size; // let the size account for the header block size
	int fit_exist = 0;//checking if the store has been updated or not	
	//start finding the best block
	while((current->size_status)!=1){//as long as dont hit end mark, then run
		//finding the closest fit block
		if((current->size_status&1)==0){//checking if the block is free
			if((current->size_status&(-4))>=size){//checking if the block is greater than the requested size
				if(store == first_blk || (current->size_status&(-4))<(store->size_status&(-4))){//checking if the block is better than the current best block
					store = current;//update the best block pointer
					fit_exist = 1;
				}	
			}
		}
		current = (blk_hdr*)((void*)current+(current->size_status&(-4)));//update the current pointer to the next on
	}
	if (!fit_exist) return NULL;
	
	if((store->size_status&(-4))<size)return NULL;//checking if the requested size but this is NOT WROKING!!!!
	//splitting
	if((store->size_status&(-4))>size){
		blk_hdr* split_header = ((void*)store+size);// address of the header of splitted block
		split_header->size_status = ((store->size_status&(-4))-size)|2;
		store->size_status = (store->size_status & 2) |size|1;
		((blk_hdr*)((void*)split_header+(split_header->size_status&(-4))-blk_size))->size_status = split_header->size_status&(-4);		
	}
	return ((void*)store)+blk_size;
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr) {                        
    // Your code goes in here 
	int blk_size = sizeof(blk_hdr*);
	if(ptr==NULL)return -1;
	blk_hdr* cur = ptr - blk_size; //moving the address to the header
	//printf("\n%p, %p\n", ptr, cur);  
	if((int)ptr%8!=0 || (cur->size_status&1)==0) return -1; // if the ptr is not 8 bytes aligned or block is already freed	

	//changing the status of the current block
	int flag=0;//1 means the next block is endmark
	blk_hdr* footer = (blk_hdr*)((cur->size_status&(-4))+(void*)cur-blk_size);//adding the footer
	footer->size_status = cur->size_status&(-4);//changing the footer size	
	cur->size_status = cur->size_status&(-2);//changing the LSB to 0
	if((footer+1)->size_status==1)flag=1;//next block is end mark situation 
	else (footer+1)->size_status=((footer+1)->size_status)-2;//chaning the next block's p bits situation
	
	//coalesce here
	//checking the previous is free or not
	if((cur->size_status&2)==0) {
		if( (((blk_hdr*)((void*)cur-((cur-1)->size_status)))->size_status&2)==2){//if the prev block's LSB is 1	
			((blk_hdr*)((void*)cur-((cur-1)->size_status)))->size_status = ((cur-1)->size_status) +(footer->size_status)+2;
		} else{//if the prev block's SLB is 0
			((blk_hdr*)((void*)cur-((cur-1)->size_status)))->size_status = ((cur-1)->size_status) +(footer->size_status);
		}
		int old_footer_size = footer->size_status;//storing the old footer size 
		footer->size_status = (((blk_hdr*)((void*)cur-((cur-1)->size_status)))->size_status)&(-4);// updating footer size	
		cur = (blk_hdr*)((void*)cur - (footer->size_status-old_footer_size));//updating cur to the new cur
	}

	//coalescing the block that is next to the current
	if(flag==1){}//end mark situation
	else if((((footer+1)->size_status)&1)==0){//checking the next block is free or not
		int temp = ((footer+1)->size_status)&(-4);//the size of the next block
		int temp2 = footer->size_status;//the size of the footer before updating
		footer = (blk_hdr*)((void*)footer+temp);//to the beginning of the footer
		footer->size_status = temp + temp2;
		int pbit = cur->size_status&3;
		cur->size_status = footer->size_status+pbit; 
	} 	
	return 0;
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Mem_Init has allocated space during a previous call\n");
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

    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */ 
void Mem_Dump() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
