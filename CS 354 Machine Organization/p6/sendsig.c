#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "-i") == 0) {  // sigint
        kill(atoi(argv[2]), 2);
    } else if (strcmp(argv[1], "-u") == 0) {  // siguser1
        kill(atoi(argv[2]), 10);
    } else {  // other cases
        printf("wrong parameter\n");
        kill(atoi(argv[2]), 9);
    }

    return 0;
}
