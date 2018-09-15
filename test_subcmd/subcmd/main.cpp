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
    printf("Hello World, this is a test...\n");

    display_process_info();

    return 10;
}