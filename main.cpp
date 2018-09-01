/**
 * main.cpp
 *
 * Example code for forking a process and communicating between the parent
 * and child processes.
 *
 */

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
#include <cassert>
#include <cstring>
#include <netinet/in.h>

#include "fork_utils.h"
#include "process_utils.h"

/**
 * This is the child process function. Currently it waits for a command on the
 * childs socket and when it arrives parse it and handle the request.
 *
 * @param proc_desc
 * @return
 */
static int child_process(const process_desc_t * proc_desc) {
    fprintf(stderr, "child process(pid = %d)\n", proc_desc->child_pid);
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
                message_t * pMsg = child_get_msg(proc_desc);
                assert(pMsg != nullptr);

                std::string cmd;
                string_from_msg(pMsg, cmd);
                string_vector  strVec;
                if (split_string(cmd, strVec)) {
                    if (strVec.size() == 2) {
                        fprintf(stderr, "cmd type = [%s]\n"
                                        "cmd func = [%s]\n",
                                        strVec[0].c_str(),
                                        strVec[1].c_str());

                        if (strVec[1] == "quit") {
                            done = true;
                        }
                    }
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

#if 0
static int socket_server(const process_desc_t * proc_desc, int port) {
    int sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket_server call to socket failed");
        return 10;
    }
    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return 10;
    }

    struct sockaddr_in address = { 0 };

//        .sin_family = AF_INET,
//        .sin_addr = {
//                .s_addr = INADDR_ANY,
//        },
//        .sin_port = port,
    //};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    return 0;
}
#endif

bool test_fork_lambda() {
    int status = -1;

    process_desc_t * pDesc = do_fork([](const process_desc_t * pDesc) -> int {
        fprintf(stderr, "From the lambda child process\n");
        sleep(2);
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

    close(pDesc->fds[(int)fd_type::fd_parent]);

    test_fork_lambda();

    return 0;
}