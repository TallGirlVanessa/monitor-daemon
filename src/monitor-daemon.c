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
const int MAX_IP_LEN = 50;

void get_SSID() {
    FILE *get_ssid_fp = popen("networksetup -getairportnetwork en0 | awk -F\": \" '{print $2}'", "r");
    char ssid[MAX_SSID_LEN];
    ssid[0] = 0;
    fgets(ssid, MAX_SSID_LEN, get_ssid_fp);
    ssid[strcspn(ssid, "\n")] = 0;
    int status = pclose(get_ssid_fp);
    if (status == -1) {
        perror("pclose error");
    }
    printf("Current SSID: '%s'\n", ssid);
}

void get_network_info() {
    FILE *get_ip_fp = popen("networksetup -getinfo 'Wi-Fi' | grep 'IP address' | awk -F\": \" '{print $2}'", "r");
    char ip[MAX_IP_LEN];
    ip[0] = 0;
    fgets(ip, MAX_IP_LEN, get_ip_fp);
    ip[strcspn(ip, "\n")] = 0;
    int status = pclose(get_ip_fp);
    if (status == -1) {
        perror("pclose error");
    }
    if (strlen(ip) > 0) {
        printf("Connected. IP: '%s'\n", ip);
    } else {
        printf("Disconnected\n");
    }

}

int main (int argc, const char * argv[]) {
    printf("Hello world!\n");
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    xpc_set_event_stream_handler("com.apple.notifyd.matching", 0, ^(xpc_object_t event) {
            const char *name = xpc_dictionary_get_string(event, XPC_EVENT_KEY_NAME);
            printf("%s\n", name);
            get_SSID();
            get_network_info();
            });
    printf("Initialized\n");
    fflush(stdout);
    while (1) {
        sleep(1);
        printf("Doing a thing\n");
        fflush(stdout);
    };
}
