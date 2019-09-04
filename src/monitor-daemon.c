#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <xpc/xpc.h>
#include <notify_keys.h>

void signal_handler(int signo) {
    switch (signo) {
        case SIGINT:
            printf("Goodbye world\n");
            exit(128 + signo);
        case SIGTERM:
            printf("Goodbye world\n");
            exit(0);
    }
}

int main (int argc, const char * argv[]) {
    printf("Hello world!\n");
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    xpc_set_event_stream_handler("com.apple.notifyd.matching", 0, ^(xpc_object_t event) {
                const char *name = xpc_dictionary_get_string(event, XPC_EVENT_KEY_NAME);
                printf("%s\n", name);
            }
            );
    printf("Initialized\n");
    fflush(stdout);
    while (1) {
        sleep(1);
        printf("Doing a thing\n");
        fflush(stdout);
    };
}
