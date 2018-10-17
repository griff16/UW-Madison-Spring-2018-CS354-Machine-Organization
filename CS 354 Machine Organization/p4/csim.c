/* 
 * Main File: cachelab
 * This file: csim.c
 * Other Files: 
 * Semester: CS354 Srping 2018
 *
 * Name: Griff Zhang
 * Eamil: xzhang953@wisc.edu 
 * CS login:griff
 * Section(s):
 *
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/****************************************************************************/
/***** DO NOT MODIFY THESE VARIABLE NAMES ***********************************/

/* Globals set by command line args */
int s = 0; /* set index bits */
int E = 0; /* associativity */
int b = 0; /* block offset bits */
int verbosity = 0; /* print trace if set */
char* trace_file = NULL;

/* Derived from command line args */
int B; /* block size (bytes) B = 2^b */
int S; /* number of sets S = 2^s In C, you can use the left shift operator */

/* Counters used to record cache statistics */
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;
/*****************************************************************************/


/* Type: Memory address 
 * Use this type whenever dealing with addresses or address masks
 */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
 * TODO 
 * 
 * NOTE: 
 * You might (not necessarily though) want to add an extra field to this struct
 * depending on your implementation
 * 
 * For example, to use a linked list based LRU,
 * you might want to have a field "struct cache_line * next" in the struct 
 */
typedef struct cache_line {                     
    char valid;
    mem_addr_t tag;
    struct cache_line * next;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;
cache_line_t** heads;  // array of pointers each poinst to the head of the list
cache_line_t** tails;  // array of pointers each points to the tail of the list


/* The cache we are simulating */
cache_t cache;  

/* TODO - COMPLETE THIS FUNCTION
 * initCache - 
 * Allocate data structures to hold info regrading the sets and cache lines
 * use struct "cache_line_t" here
 * Initialize valid and tag field with 0s.
 * use S (= 2^s) and E while allocating the data structures here
 */
void initCache(){
    int i, j;                      
    S = 1 << s;
    B = 1 << b;
    
    cache = malloc(S*sizeof(cache_set_t));  // creating S number of sets
    if (!cache){ exit (0); }  // if fail exit
    
    heads = malloc(S*sizeof(cache_line_t*));
    if (!heads){ exit (0); }  // if fail exit
    
    tails = malloc(S*sizeof(cache_line_t*));
    if (!tails){ exit (0); }  // if fail exit
    
    for (i = 0; i < S; i++){  // creating E number of lines
        *(cache+i) = malloc(E*sizeof(cache_line_t));
        for (j = 0; j < E; j++){  // initializing each set's vaild and tag bits
            (*(cache + i) + j)->valid = 0;
            (*(cache + i) + j)->tag = 0;
            (*(cache + i) + j)->next = *(cache + i) + j + 1;
        }
        heads[i] = *(cache + i) + 0;
        tails[i] = *(cache + i) + E - 1;
        tails[i]->next = NULL;
    }
}


/* TODO - COMPLETE THIS FUNCTION 
 * freeCache - free each piece of memory you allocated using malloc 
 * inside initCache() function
 */
void freeCache() {                     
    int i;
    for (i = 0; i < S; i++){
        free(*(cache+i));  // freeing each set's content first
    }
    free(heads);
    free(tails);    
    free(cache);
}

/* TODO - COMPLETE THIS FUNCTION 
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increase hit_cnt
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase evict_cnt if a line is evicted.
 *   you will manipulate data structures allocated in initCache() here
 */
void accessData(mem_addr_t addr) {
    // extracting s, b, and tag bits
    mem_addr_t bbits = 1; 
    mem_addr_t sbits = 1; 
    mem_addr_t tagbits = 1;
    int i;
    for (i = 1; i < b; i++){  // finding the right value to extract b bits
        bbits = (bbits << 1) + 1;
    }
    bbits = addr & bbits;
    addr = addr >> b;  // getting rid of b bits

    for (i = 1; i < s; i++){  // finding the right value to extract s bits
        sbits = (sbits << 1) + 1;
    }
    sbits = addr & sbits;
    addr = addr >> s;  // getting rid of s bits
    
    tagbits = addr;  // extracting tag bits

    // checking if there is the requested set
    int flag = 1;  // checking if there is a hit

    for (i = 0; i < E; i++){  // go through cache to find out if there is a hit
        // checking if there is a hiti
        // if run, then tagbits are the same, and next check valid bits
        if ((*(cache + sbits) + i)->tag == tagbits){
            // checking if v bit, if 1 then it is a hit
            if ((*(cache + sbits) + i)->valid == 1){
                // chechking if the hit line is the head of the list
                if ( heads[sbits] == (*(cache + sbits) + i) ) {  
                    if (heads[sbits]->next == NULL){
                        // do nothing
                    } else {
                        // updating head[sbits] to hit line's next
                        heads[sbits] = heads[sbits]->next;
                        // updating old tail[sbits]'s next
                        tails[sbits]->next = (*(cache + sbits) + i);  
                        // updating tail[sbits] to the new one
                        tails[sbits] = (*(cache + sbits) + i);  
                        tails[sbits]->next = NULL;
                    }
                } else if ( tails[sbits] == (*(cache + sbits) + i) ){
                    /*if the hit line is the tail of the list do nothing*/
                } else {  // the case when the hit is in the middle of the list
                    // connecting the one before hit line and the one 
                    // after hit line
                    (*(cache + sbits) + i - B)->next = 
                        (*(cache + sbits) + i)->next;
                    tails[sbits]->next = *(cache + sbits) + i;
                    tails[sbits] = *(cache + sbits) + i;
                    tails[sbits]->next = NULL;
                }
                hit_cnt++;
                if (verbosity){printf("hit");}
                flag = 0;
                break;
            }
        }  // ending chekcing hit case
    }

    if (flag){  // if run, then there is a miss
        if (heads[sbits]->valid == 1){  // miss eviction case
            miss_cnt++;
            evict_cnt++;
            if (verbosity){printf("miss eviction");}
        } else {  // miss case
            miss_cnt++;
            if (verbosity){printf("miss");}
        }
    
        heads[sbits]->valid = 1;  // changing valid bits
        heads[sbits]->tag = tagbits;  // changing tag bits 
        tails[sbits]->next = heads[sbits];  // manupilating the list
        tails[sbits] = heads[sbits];
        heads[sbits] = heads[sbits]->next;
        tails[sbits]->next = NULL;
    }
}

/* TODO - FILL IN THE MISSING CODE
 * replayTrace - replays the given trace file against the cache 
 * reads the input trace file line by line
 * extracts the type of each memory access : L/S/M
 * YOU MUST TRANSLATE one "L" as a load i.e. 1 memory access
 * YOU MUST TRANSLATE one "S" as a store i.e. 1 memory access
 * YOU MUST TRANSLATE one "M" as a load followed by a store i.e. 2 memory accesses 
 */
void replayTrace(char* trace_fn) {                      
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if (!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // now you have: 
            // 1. address accessed in variable - addr 
            // 2. type of acccess(S/L/M)  in variable - buf[1] 
            // call accessData function here depending on type of acces
            if (buf[1] == 'S' || buf[1] == 'L'){
                accessData(addr);   
            } else if (buf[1] == 'M')  {
                accessData(addr);
                if (verbosity){
                    printf(" ");
                }
                accessData(addr);
            } else {  // ignoring I instruction
            }

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */
void printSummary(int hits, int misses, int evictions) {                        
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[]) {                      
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                printUsage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                printUsage(argv);
                exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Initialize cache */
    initCache();

    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_cnt, miss_cnt, evict_cnt);
    return 0;
}
