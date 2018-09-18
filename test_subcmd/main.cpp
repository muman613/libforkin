#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <string>

#include "popen2.h"

#define SUBCMD_CMD "subcmd/subcmd"


void display_process_info() {
    printf("UID           = %d\n", getuid());
    printf("GID           = %d\n", getgid());
    printf("Effective UID = %d\n", geteuid());
    printf("Effective GID = %d\n", getegid());
    printf("Process ID    = %d\n", getpid());
}

char ** get_command_args(const char * full_command) {
    char * command = strdup(full_command);
    char * tok = nullptr;
    int arg_count = 0;
    char * pcmd = command;

    tok = strtok(command, " ");
    while (tok != nullptr) {
        printf("tok = %s\n", tok);
        tok = strtok(NULL, " ");
        arg_count++;
    }
    free(pcmd);

    printf("found %d args\n", arg_count);

    char ** arg_array = nullptr;

    arg_array = (char **)malloc((arg_count + 1) * sizeof(char *));
    memset(arg_array, 0, (arg_count + 1) * sizeof(char *));

    command = strdup(full_command);
    pcmd = command;
    tok = strtok(command, " ");
    arg_array[0] = strdup(tok);
    tok = strtok(NULL, " ");
    int i = 0;

    while (tok != nullptr) {
        arg_array[1 + i] = strdup(tok);
        tok = strtok(NULL, " ");
        i++;
    }
    arg_array[1 + i] = nullptr;

    free(pcmd);

    return arg_array;
}

void display_args(char * args[]) {
    assert(args != nullptr);

    for (size_t index = 0 ; args[index] != nullptr ; index++) {
        printf("arg %zu [%s]\n", index, args[index]);
    }

    return;
}


int main(int argc, char * argv[]) {
//    char * command_str = strdup("/bin/cat > /tmp/boo");
//    char ** args = get_command_args(command_str);
//    printf("args = %p\n", args);
//    display_args(args);

    display_process_info();

    char ** args = get_command_args(SUBCMD_CMD);
    FILE * read_fp = popen2(args, "r");

    if (read_fp != nullptr) {

        std::string str;

        while (!feof(read_fp)) {
            char ch = fgetc(read_fp);
            if (ch != EOF)
                str += ch;
        }

        int result = pclose2(read_fp);
        if (WIFEXITED(result)) {
            printf("Result returned = %d\n", WEXITSTATUS(result));
            printf("Std Out = %s\n", str.c_str());
        } else {

        }
    }

    args = get_command_args("subcmd/subcmd write");
    FILE * write_fp = popen2(args, "w");

    if (write_fp != nullptr) {
        fprintf(write_fp, "This is a test!");

        int result = pclose2(write_fp);
        if (WIFEXITED(result)) {
            printf("Result returned = %d\n", WEXITSTATUS(result));
        } else {

        }

    }

    return 0;
}