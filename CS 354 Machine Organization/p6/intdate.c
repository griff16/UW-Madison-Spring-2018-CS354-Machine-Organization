#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

// fields
int count;  // counts SIGUSR1

// the function that handles the signal
void signal_handler() {
    time_t cur_time;
    time(&cur_time);
    printf("PID: %d | Current Time: %s", getpid(), ctime(&cur_time));
    alarm(3);
}

// the SIGUSR1 handler
void userone_handler() {
    count++;
    printf("SIGUSR1 caught!\n");
}

// sigInt handler
void sigint_handler() {
    printf("SIGINT recieved\n");
    printf("SIGUSR1 was recieved %d times. Exiting now\n", count);
    exit(0);
}

// the main function
int main() {
    struct sigaction sa;  // setting up sig alarm
    if (memset(&sa, 0, sizeof(sa)) == 0) {
       printf("error with memset");
       exit(0);
    }
    sa.sa_handler = signal_handler;

    struct sigaction sa2;  // setting up user1 signal 
    if (memset(&sa2, 0, sizeof(sa2)) == 0) {
        printf("error with memset");
        exit(0);
    }
    sa2.sa_handler = userone_handler;

    struct sigaction sa3;  // setting up sigint signal
    if (memset(&sa3, 0, sizeof(sa3)) == 0) {
        printf("error with memset");
        exit(0);
    }
    sa3.sa_handler = sigint_handler;

    
    // triggering the alarm
    if (alarm(3) != 0) {
        printf("error with alarm");
        exit(0);
    }
    if (sigaction(14, &sa, NULL) != 0) {  // sig alarm
        printf("error with sigacgtion");
        exit(0);
    }
    if (sigaction(10, &sa2, NULL) != 0) {  // sig user1 
        printf("error with aigaction");
        exit(0);
    }
    if (sigaction(2, &sa3, NULL) != 0) {  // sigint
        printf("error with aigaction");
        exit(0);
    }

    while (1) {}

    return 0;
}
