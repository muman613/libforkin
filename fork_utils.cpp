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

/**
 * do a fork process and run the child callback.
 *
 * @param child_process_cb Function to call in child process context.
 * @return
 */

process_desc_t * do_fork(const process_callback & child_process_cb) {
    auto new_process_desc = new process_desc_t;

    // create a pair of sockets.
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, new_process_desc->fds) < 0) {
        perror("socketpair error : ");
        return nullptr;
    }

    new_process_desc->parent_pid = getpid();
    new_process_desc->child_pid  = fork();

    if (new_process_desc->child_pid < 0) {
        perror("fork failed : ");
        return nullptr;
    } else if (new_process_desc->child_pid == 0) {
        // child process
        close(new_process_desc->fds[(int)fd_type::fd_parent]);
        new_process_desc->child_pid = getpid();
        fprintf(stderr, "fds[0] = %d fds[1] = %d\n", new_process_desc->fds[0],
                new_process_desc->fds[1]);
        int result = child_process_cb(new_process_desc);
        exit(result);
    } else {
        // parent process
        close(new_process_desc->fds[(int)fd_type::fd_child]);
    }

    return new_process_desc;
}

/**
 * Send a message from the parent to the child process
 *
 * @param proc_desc pointer to process_desc_t structure
 * @param data pointer to data to send to the child
 * @param length length of the data (must be less than MAX_BUFFER)
 * @return
 */
bool send_msg_to_child(process_desc_t * proc_desc, const char * data, ssize_t length) {
    assert(proc_desc != nullptr);
    assert(data != nullptr);

    int fd = proc_desc->fds[(int)fd_type::fd_parent];

    if (length > (BUFFER_MAX - 1)) {
        fprintf(stderr, "-- error invalid length (must be less than %d)", BUFFER_MAX);
        return false;
    }

    write(fd, data, length);

    return true;
}

bool send_msg_to_child(process_desc_t * proc_desc, const std::string & msg) {
    return send_msg_to_child(proc_desc, msg.c_str(), msg.length());
}
