#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void display_process_info() {
    printf("UID           = %d\n", getuid());
    printf("GID           = %d\n", getgid());
    printf("Effective UID = %d\n", geteuid());
    printf("Effective GID = %d\n", getegid());
    printf("Process ID    = %d\n", getpid());
}


int main(int argc, char * argv[]) {
    if (argc == 1) {
        printf("Hello World, this is a test...\n");
        display_process_info();
    } else {
        char buffer[32];

        fgets(buffer, 32, stdin);
        printf(">> %s", buffer);
        fgets(buffer, 32, stdin);
        printf(">> %s", buffer);

    }

    return 10;
}