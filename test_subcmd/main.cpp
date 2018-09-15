#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <string>

#define SUBCMD_CMD "subcmd/subcmd"


typedef struct _popen_proc_entry {
    FILE *  fp;
    int     fd;
    pid_t   pid;
    char    mode;
} POPEN_PROC_ENTRY;

POPEN_PROC_ENTRY __popen_table[8] = {0};

#define PIPE_READ_END   0
#define PIPE_WRITE_END  1

int lookup_index_by_file(FILE *fp) {
    int index = 0;
    for (auto entry : __popen_table) {
        if (entry.fp == fp) {
            return index;
        }
        index++;
    }

    return -1;
}

void clear_popen_table_entry(int index) {
    assert(index != -1);

    memset(&__popen_table[index], 0, sizeof(POPEN_PROC_ENTRY));

    return;
}


FILE * popen2(const char* command, const char * mode) {
    FILE * fp = nullptr;
    pid_t child_pid;
    const char * const  args[] = {
            command, nullptr,
    };

    int fd[2];

    if (pipe(fd) != 0) {
        perror("pipe failed");
        return nullptr;
    }

    child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        return (FILE*)nullptr;
    } else if (child_pid == 0) {
        // child process
        dup2(fd[PIPE_WRITE_END], STDOUT_FILENO);
        close(fd[PIPE_READ_END]);
        close(fd[PIPE_WRITE_END]);

        setgid(1000);
        setuid(1000);

        execv(command, (char * const *)args);
        perror("execv failed");
    } else {
        close(fd[PIPE_WRITE_END]);
        fp = fdopen(fd[PIPE_READ_END], "r");
        __popen_table[0].fd = fd[PIPE_READ_END];
        __popen_table[0].fp = fp;
        __popen_table[0].mode = 'r';
        __popen_table[0].pid = child_pid;

        printf("Child process pid = %d\n", child_pid);
    }

    return fp;
}


int pclose2(FILE * fp) {
    int popen_table_index = lookup_index_by_file(fp);
    assert(popen_table_index != -1);

    int result;

    waitpid(__popen_table[popen_table_index].pid, &result, 0);

    close(__popen_table[popen_table_index].fd);
    fclose(__popen_table[popen_table_index].fp);

    clear_popen_table_entry(popen_table_index);

    return result;
}

void display_process_info() {
    printf("UID           = %d\n", getuid());
    printf("GID           = %d\n", getgid());
    printf("Effective UID = %d\n", geteuid());
    printf("Effective GID = %d\n", getegid());
    printf("Process ID    = %d\n", getpid());
}


int main(int argc, char * argv[]) {
    display_process_info();

    FILE * read_fp = popen2(SUBCMD_CMD, "r");

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
    return 0;
}