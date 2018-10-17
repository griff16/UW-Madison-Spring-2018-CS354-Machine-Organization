#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

// fields
int count = 0;

void signal_handler() {
    printf("a division by 0 operation was attempted.\n");
    printf("Total number of operations completed successfully: %d\n", count);
    printf("The program will be terminated.\n");
    exit(0);
}

int main() {
    int quotient;
    int remainder;
    int first;
    int second;
    char buffer[100];

    // setting up handler
    struct sigaction sa;
    if (memset(&sa, 0, sizeof(sa))== 0) {
        printf("error with memset");
        exit(0);
    }
    sa.sa_handler = signal_handler;
    if (sigaction(8, &sa, NULL) != 0) {
        printf("error with sigaction");
        exit(0);
    }

    while (1) {
        // getting the first number  
        printf("Enter first integer:");
        fgets(buffer, 100, stdin);
        first = atoi(buffer);

        printf("Enter second integer:");  // getting the second number
        fgets(buffer, 100, stdin);
        second = atoi(buffer);

        quotient = first / second;
        remainder = first % second;
        count++;
        printf("%d / %d is %d with a remainder of %d\n", 
            first, second, quotient, remainder);
    }

    return 0;
}
