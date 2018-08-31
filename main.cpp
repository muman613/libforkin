#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>
#include <wait.h>
#include <assert.h>
#include <cstring>

#include "fork_utils.h"

//#define BUFFER_MAX      32

static int child_process(const process_desc_t * proc_desc) {
    fprintf(stderr, "child process(pid = %d)\n", proc_desc->child_pid);
    char buf[BUFFER_MAX] = {0};
    bool done = false;
    int fd = proc_desc->fds[(int)fd_type::fd_child]; // get the child file descriptor
    fd_set sel_set;
    int nfds = fd + 1;

    while (!done) {
        struct timeval t = { 0, 500000 };
        FD_ZERO(&sel_set);
        FD_SET(fd, &sel_set);

        int selectRes = select(nfds, &sel_set, nullptr, nullptr, &t);
        if (selectRes > 0) {
            if (FD_ISSET(fd, &sel_set)) {
                ssize_t bytes_read = read(proc_desc->fds[(int) fd_type::fd_child], buf, BUFFER_MAX-1);
                buf[bytes_read] = '\0';

                fprintf(stderr, "bytes read = %zu (%s)\n", bytes_read, buf);

                if (strcmp(buf, "cmd:quit") == 0) {
                    fprintf(stderr, "-- quiting child process\n");
                    done = true;
                }
            }
        } else if (selectRes == 0) {
            // timed out
            fprintf(stderr, "-- select loop timed-out\n");
        } else {
            // error
            perror("select : ");
        }


    }

    fprintf(stderr, "child_process exiting\n");

    return 0;
}

bool test_fork_lambda() {
    int status = -1;

    process_desc_t * pDesc = do_fork([](const process_desc_t * pDesc) -> int {
        fprintf(stderr, "From the lamba child process\n");
        return 10;
    });

    waitpid(pDesc->child_pid, &status, 0);
    printf("result = %d\n", WEXITSTATUS(status));

    return true;
}

int main() {
    process_desc_t *  pDesc = do_fork(child_process);
    printf("Parent pid = %d Child pid = %d\n", pDesc->parent_pid, pDesc->child_pid);
    printf("-- waiting for child to exit...\n");

    std::string cmd = "cmd:ls";

    send_msg_to_child(pDesc, cmd);
    sleep(2);

    cmd = "cmd:quit";
    send_msg_to_child(pDesc, cmd);

    int status;
    waitpid(pDesc->child_pid, &status, 0);
    printf("Status = %d\n", WEXITSTATUS(status));

    test_fork_lambda();

    return 0;
}