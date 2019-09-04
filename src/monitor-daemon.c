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

const int MAX_SSID_LEN = 50;

int main (int argc, const char * argv[]) {
    printf("Hello world!\n");
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    xpc_set_event_stream_handler("com.apple.notifyd.matching", 0, ^(xpc_object_t event) {
            const char *name = xpc_dictionary_get_string(event, XPC_EVENT_KEY_NAME);
            printf("%s\n", name);
            FILE *get_ssid_fp = popen("networksetup -getairportnetwork en0 | awk -F\": \" '{print $2}'", "r");
            char ssid[MAX_SSID_LEN];
            fgets(ssid, MAX_SSID_LEN, get_ssid_fp);
            ssid[strcspn(ssid, "\n")] = 0;
            int status = pclose(get_ssid_fp);
            if (status == -1) {
                perror("pclose error");
            }
            printf("Current SSID: '%s'\n", ssid);
    });
    printf("Initialized\n");
    fflush(stdout);
    while (1) {
        sleep(1);
        printf("Doing a thing\n");
        fflush(stdout);
    };
}
