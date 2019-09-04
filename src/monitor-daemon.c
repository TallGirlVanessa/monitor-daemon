#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signo) {
    switch (signo) {
        case SIGINT:
        case SIGTERM:
            printf("Goodbye world\n");
            exit(128 + signo);
    }
}

int main (int argc, const char * argv[]) {
    printf("Hello world!\n");
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    printf("Signals ready\n");
    fflush(stdout);
    while (1) {
        sleep(1);
        printf("Doing a thing\n");
        fflush(stdout);
    };
}
