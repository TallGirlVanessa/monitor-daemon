#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <xpc/xpc.h>
#include <notify_keys.h>
#include <time.h>

size_t DATETIME_LENGTH = strlen("2019-01-01 12:12:12");

void time_for_log(char* out) {
    time_t epoch_seconds;
    struct tm calendar_time;
    time(&epoch_seconds);
    struct tm *result = gmtime_r(&epoch_seconds, &calendar_time);
    if (result == 0) {
        perror("gmtime_r error");
        exit(1);
    }
    strftime(out, DATETIME_LENGTH, "%F %T", &calendar_time);
}

void signal_handler(int signo) {
    char formatted_time[DATETIME_LENGTH + 1];
    formatted_time[DATETIME_LENGTH] = 0;
    switch (signo) {
        case SIGINT:
            time_for_log(formatted_time);
            printf("%s SIGINT, shutting down\n", formatted_time);
            exit(128 + signo);
        case SIGTERM:
            time_for_log(formatted_time);
            printf("%s SIGTERM, shutting down\n", formatted_time);
            exit(0);
    }
}

const int MAX_SSID_LEN = 50;
const int MAX_IP_LEN = 50;

char *STARRY_WIFI = "Bill Wi The Science Fi_5";

bool ssid_match = false;
bool connected = false;
bool monitoring = false;

void get_SSID() {
    char formatted_time[DATETIME_LENGTH + 1];
    formatted_time[DATETIME_LENGTH] = 0;
    time_for_log(formatted_time);
    FILE *get_ssid_fp = popen("networksetup -getairportnetwork en0 | awk -F\": \" '{print $2}'", "r");
    if (get_ssid_fp == 0) {
        perror("popen error");
        exit(1);
    }
    char ssid[MAX_SSID_LEN];
    ssid[0] = 0;
    fgets(ssid, MAX_SSID_LEN, get_ssid_fp);
    ssid[strcspn(ssid, "\n")] = 0;
    int status = pclose(get_ssid_fp);
    if (status == -1) {
        perror("pclose error");
        exit(1);
    }
    printf("%s Current SSID: '%s'\n", formatted_time, ssid);
    if (strcmp(ssid, STARRY_WIFI) != 0) {
        printf("%s Not Starry Wi-Fi\n", formatted_time);
        ssid_match = false;
    } else {
        printf("%s Starry Wi-Fi\n", formatted_time);
        ssid_match = true;
    }
}

void get_network_info() {
    char formatted_time[DATETIME_LENGTH + 1];
    formatted_time[DATETIME_LENGTH] = 0;
    time_for_log(formatted_time);
    FILE *get_ip_fp = popen("networksetup -getinfo 'Wi-Fi' | grep 'IP address' | awk -F\": \" '{print $2}'", "r");
    if (get_ip_fp == 0) {
        perror("popen error");
        exit(1);
    }
    char ip[MAX_IP_LEN];
    ip[0] = 0;
    fgets(ip, MAX_IP_LEN, get_ip_fp);
    ip[strcspn(ip, "\n")] = 0;
    int status = pclose(get_ip_fp);
    if (status == -1) {
        perror("pclose error");
    }
    if (strlen(ip) > 0) {
        printf("%s Connected. IP: '%s'\n", formatted_time, ip);
        connected = true;
    } else {
        printf("%s Disconnected\n", formatted_time);
        connected = false;
    }

}

int main (int argc, const char * argv[]) {
    char formatted_time[DATETIME_LENGTH + 1];
    formatted_time[DATETIME_LENGTH] = 0;
    time_for_log(formatted_time);
    printf("%s Starting\n", formatted_time);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    dispatch_queue_t queue = dispatch_get_main_queue();
    xpc_set_event_stream_handler("com.apple.notifyd.matching", queue, ^(xpc_object_t event) {
            char handler_formatted_time[DATETIME_LENGTH + 1];
            handler_formatted_time[DATETIME_LENGTH] = 0;
            time_for_log(handler_formatted_time);
            const char *name = xpc_dictionary_get_string(event, XPC_EVENT_KEY_NAME);
            printf("%s %s\n", handler_formatted_time, name);
            get_SSID();
            get_network_info();
            time_for_log(handler_formatted_time);
            if (ssid_match && connected) {
                printf("%s SSID match & connected. ", handler_formatted_time);
                if (monitoring) {
                    printf("Monitoring already enabled\n");
                } else {
                    printf("Enable monitoring\n");
                    monitoring = true;
                }
            } else {
                printf("%s SSID mismatch or not connected. ", handler_formatted_time);
                if (monitoring) {
                    printf("Disable monitoring\n");
                    monitoring = false;
                } else {
                    printf("Monitoring already disabled\n");
                }
            }
            fflush(stdout);
            });
    time_for_log(formatted_time);
    printf("%s Initialized\n", formatted_time);
    fflush(stdout);
    dispatch_main();
}
