/* many odd sized allocations checked for alignment */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

int main() {
    assert(Mem_Init(4096) == 0);
    void* ptr[9];
    ptr[0] = Mem_Alloc(1);
    ptr[1] = (Mem_Alloc(5));
    ptr[2] = (Mem_Alloc(14));
    ptr[3] = (Mem_Alloc(8));
    ptr[4] = (Mem_Alloc(1));
    ptr[5] = (Mem_Alloc(4));
    ptr[6] = (Mem_Alloc(9));
    ptr[7] = (Mem_Alloc(33));
    ptr[8] = (Mem_Alloc(55));
   
    for (int i = 0; i < 9; i++) {
        assert(((int)ptr[i]) % 8 == 0);
    }
    exit(0);
}
