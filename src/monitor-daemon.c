#include <stdio.h>
#include <unistd.h>

int main (int argc, const char * argv[]) {
    printf("Hello world!\n");
    fflush(stdout);
    while (1) {
        sleep(1);
        printf("Doing a thing\n");
        fflush(stdout);
    };
    printf("Goodbye world\n");
    return 0;
}
