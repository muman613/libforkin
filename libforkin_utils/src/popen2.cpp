#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <sys/wait.h>
#include <string>
#include <errno.h>
#include "popen2.h"

static struct _popen_proc_entry {
    FILE *              fp;
    pid_t               pid;
    _popen_proc_entry * next;
} * _popen_proc_list = nullptr;


/**
 * Custom popen function. Does not invoke the shell to run the command.
 *
 * @param command
 * @param type
 * @return
 */

#if 0
FILE * popen2(const char* command, const char * type) {
    FILE * fp = nullptr;
    pid_t child_pid;
    _popen_proc_entry * cur = nullptr;

    const char * const  args[] = {
            command, nullptr,
    };

    if ((*type != 'r') && (*type != 'w') && (type[1] != '\0')) {
        errno = EINVAL;
        return (FILE*)nullptr;
    }

    int fd[2];

    if (pipe(fd) != 0) {
        perror("pipe failed");
        return (FILE*)nullptr;
    }

    if ((cur = (_popen_proc_entry *)malloc(sizeof(_popen_proc_entry))) == nullptr) {
        (void)close(fd[PIPE_READ_END]);
        (void)close(fd[PIPE_WRITE_END]);
        return (FILE*)nullptr;
    }

    child_pid = fork();
    if (child_pid == -1) {
        // This happens if the fork() call failed.
        perror("fork failed");
        (void)close(fd[PIPE_READ_END]);
        (void)close(fd[PIPE_WRITE_END]);
        return (FILE*)nullptr;
    } else if (child_pid == 0) {
        // child process
        if (*type == 'r') {
            dup2(fd[PIPE_WRITE_END], STDOUT_FILENO);
        } else {
            dup2(fd[PIPE_READ_END], STDIN_FILENO);
        }
        close(fd[PIPE_READ_END]);
        close(fd[PIPE_WRITE_END]);

        setgid(1000);
        setuid(1000);

        execv(command, (char * const *)args);
        perror("execv failed");
        _exit(-127);
    } else {
        if (*type == 'r') {
            fp = fdopen(fd[PIPE_READ_END], "r");
            (void)close(fd[PIPE_WRITE_END]);
        } else {
            fp = fdopen(fd[PIPE_WRITE_END], "w");
            (void)close(fd[PIPE_READ_END]);
        }

        cur->pid    = child_pid;
        cur->fp     = fp;
        cur->next   = nullptr;

        if (_popen_proc_list == nullptr) {
            _popen_proc_list = cur;
        } else {
            cur->next = _popen_proc_list;
            _popen_proc_list = cur;
        }

        printf("Child process pid = %d\n", child_pid);
    }

    return fp;
}
#else

FILE * popen2(const char* const args[], const char * type) {
    FILE * fp = nullptr;
    pid_t child_pid;
    _popen_proc_entry * cur = nullptr;

//    const char * const  args[] = {
//            command, nullptr,
//    };

    if ((*type != 'r') && (*type != 'w') && (type[1] != '\0')) {
        errno = EINVAL;
        return (FILE*)nullptr;
    }

    int fd[2];

    if (pipe(fd) != 0) {
        perror("pipe failed");
        return (FILE*)nullptr;
    }

    if ((cur = (_popen_proc_entry *)malloc(sizeof(_popen_proc_entry))) == nullptr) {
        (void)close(fd[PIPE_READ_END]);
        (void)close(fd[PIPE_WRITE_END]);
        return (FILE*)nullptr;
    }

    child_pid = fork();
    if (child_pid == -1) {
        // This happens if the fork() call failed.
        perror("fork failed");
        (void)close(fd[PIPE_READ_END]);
        (void)close(fd[PIPE_WRITE_END]);
        return (FILE*)nullptr;
    } else if (child_pid == 0) {
        // child process
        if (*type == 'r') {
            dup2(fd[PIPE_WRITE_END], STDOUT_FILENO);
        } else {
            dup2(fd[PIPE_READ_END], STDIN_FILENO);
        }
        close(fd[PIPE_WRITE_END]);
        close(fd[PIPE_READ_END]);

        setgid(1000);
        setuid(1000);

        execv(args[0], (char * const *)args);
        perror("execv failed");
        _exit(-127);
    } else {
        if (*type == 'r') {
            fp = fdopen(fd[PIPE_READ_END], "r");
            (void)close(fd[PIPE_WRITE_END]);
        } else {
            fp = fdopen(dup(fd[PIPE_WRITE_END]), "w");
            (void)close(fd[PIPE_READ_END]);
        }

        cur->pid    = child_pid;
        cur->fp     = fp;
        cur->next   = nullptr;

        if (_popen_proc_list == nullptr) {
            _popen_proc_list = cur;
        } else {
            cur->next = _popen_proc_list;
            _popen_proc_list = cur;
        }

        printf("Child process pid = %d\n", child_pid);
    }

    return fp;
}
#endif


/**
 * Wait for process to exit and return result code.
 *
 * @param fp
 * @return
 */

int pclose2(FILE * fp) {
    pid_t   child_pid = -1;
    int     result = -1;
    _popen_proc_entry * p_entry = _popen_proc_list,
            * p_last = nullptr;

    while (p_entry != nullptr) {
        if (p_entry->fp == fp) {
            child_pid = p_entry->pid;
            fp        = p_entry->fp;
            break;
        }
        p_last = p_entry;
        p_entry = p_entry->next;
    }

    if (p_entry == nullptr) {
        errno = EINVAL;
        return -1;
    }

    fclose(fp);

    if (waitpid(child_pid, &result, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }


    // Remove the node from the list...
    if (p_last == nullptr) {
        _popen_proc_list = p_entry->next;
    } else {
        p_last->next = p_entry->next;
    }

    free(p_entry);

    return result;
}
