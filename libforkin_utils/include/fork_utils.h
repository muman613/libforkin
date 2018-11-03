/**
 * fork_utils.h
 *
 * This module contains the functions which are responsible for forking the
 * processes and communicating between the parent and child process.
 *
 * "Stick a fork in it!" - https://www.knowyourphrase.com/stick-a-fork-in-it
 *
 * @author Michael A. Uman
 * @date   August 31, 2018
 */

#ifndef FORKIN_FORK_UTILS_H
#define FORKIN_FORK_UTILS_H

#include <algorithm>
#include <string>
#include <vector>

namespace forkin::process {

    using string_vector = std::vector<std::string>;

/**
 *  Order of sockets in a socket pair (fd[0] = parent, fd[1] = child)
 */
    enum class fd_type {
        fd_parent,
        fd_child,
    };

/**
 *  This structure holds the important information regarding the parent & child
 *  processes.
 */
    typedef struct _process_desc {
        int parent_pid = -1;
        int child_pid = -1;
        int fds[2] = {-1, -1};
    } process_desc_t;


    typedef struct _message {
        uint32_t msg_size;
        uint8_t msg_data[];
    } message_t;

    using process_callback = std::function<int(const process_desc_t *)>;


    process_desc_t *do_fork(const process_callback &child_process_cb);

    bool send_msg_to_child(process_desc_t *proc_desc,
                           const char *data,
                           ssize_t length);

    bool send_msg_to_child(process_desc_t *proc_desc, const std::string &msg);

    message_t *child_get_msg(const process_desc_t *proc_desc);

    bool string_from_msg(const message_t *msg, std::string &str);


    bool split_string(const std::string &str, string_vector &strVec);

}
#endif //FORKIN_FORK_UTILS_H
