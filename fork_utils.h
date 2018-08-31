//
// Created by muman on 8/31/18.
//

#ifndef FORKIN_FORK_UTILS_H
#define FORKIN_FORK_UTILS_H

#include <algorithm>

//#define BUFFER_MAX      32

/**
 *
 */
enum class fd_type {
    fd_parent,
    fd_child,
};

/**
 *
 */
typedef struct _process_desc {
    int parent_pid = -1;
    int child_pid  = -1;
    int fds[2] = { -1, -1};
} process_desc_t;


typedef struct _message {
    uint32_t    msg_size;
    uint8_t     msg_data[];
} message_t;

using process_callback = std::function<int(const process_desc_t *)>;


process_desc_t *    do_fork(const process_callback & child_process_cb);
bool                send_msg_to_child(process_desc_t * proc_desc,
                                      const char * data,
                                      ssize_t length);
bool                send_msg_to_child(process_desc_t * proc_desc, const std::string & msg);
message_t *         child_get_msg(const process_desc_t * proc_desc);

bool                string_from_msg(const message_t * msg, std::string & str);

#endif //FORKIN_FORK_UTILS_H
