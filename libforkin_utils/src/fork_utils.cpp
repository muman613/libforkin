/**
 * fork_utils.cpp
 *
 * This module contains the functions which are responsible for forking the
 * processes and communicating between the parent and child process.
 *
 * @author Michael A. Uman
 * @date   August 31, 2018
 * @date   October 20, 2020
 */

#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <cassert>
#include <cstring>

#include "fork_utils.h"

namespace forkin::process {
/**
 * do a fork process and run the child callback.
 *
 * @param child_process_cb Function to call in child process context.
 * @return
 */

    process_desc_t *do_fork(const process_callback &child_process_cb) {
        auto new_process_desc = new process_desc_t;

        // create a pair of sockets.
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, new_process_desc->fds) < 0) {
            perror("socketpair error : ");
            return nullptr;
        }

        new_process_desc->parent_pid = getpid();
        new_process_desc->child_pid = fork();

        if (new_process_desc->child_pid < 0) {
            perror("fork failed : ");
            return nullptr;
        } else if (new_process_desc->child_pid == 0) {
            // child process
            close(new_process_desc->fds[(int) fd_type::fd_parent]);
            new_process_desc->child_pid = getpid();
            fprintf(stderr, "fds[0] = %d fds[1] = %d\n", new_process_desc->fds[0],
                    new_process_desc->fds[1]);
            int result = child_process_cb(new_process_desc);
            //close(new_process_desc->fds[(int)fd_type::fd_child]);
            exit(result);
        }

        // parent process
        close(new_process_desc->fds[(int) fd_type::fd_child]);

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
    bool send_msg_to_child(process_desc_t *proc_desc, const char *data, ssize_t length) {
        assert(proc_desc != nullptr);
        assert(data != nullptr);

        const int fd = proc_desc->fds[(int) fd_type::fd_parent];
        auto sz = (uint32_t) length;

        if (write(fd, &sz, sizeof(sz)) != sizeof(sz)) {
            perror("write pktlen : ");
            return false;
        }

        if (write(fd, data, (size_t) length) != length) {
            perror("write data : ");
            return false;
        }

        return true;
    }

    bool send_msg_to_child(process_desc_t *proc_desc, const std::string &msg) {
        return send_msg_to_child(proc_desc, msg.c_str(), msg.length());
    }

/***
 * Call when child process wants to read a message sent from the parent.
 *
 * @param proc_desc
 * @return pointer to message_t structure containing the message.
 */
    message_t *child_get_msg(const process_desc_t *proc_desc) {
        assert(proc_desc != nullptr);

        fprintf(stderr, "child_get_msg()\n");

        message_t *pNewMsg = nullptr;
        auto msgSize = (uint32_t) -1;
        int fd = proc_desc->fds[(int) fd_type::fd_child];

        if (read(fd, &msgSize, sizeof(uint32_t)) != sizeof(uint32_t)) {
            perror("read error : ");
            return nullptr;
        }

        pNewMsg = (message_t *) malloc(sizeof(message_t) + msgSize);
        assert(pNewMsg != nullptr);

        pNewMsg->msg_size = msgSize;

        if (read(fd, &pNewMsg->msg_data, msgSize) != (ssize_t) msgSize) {
            perror("read error : ");
            free(pNewMsg);
            return nullptr;
        }

        return pNewMsg;
    }

/**
 * Return a string from a message structure.
 * @param msg
 * @param str
 * @return
 */
    bool string_from_msg(const message_t *msg, std::string &str) {
        str = std::string((const char *) msg->msg_data, msg->msg_size);
        return true;
    }

/**
 * A quick and dirty utility to split a string by ':' (colon) into a vector
 * of strings.
 *
 * @param str String in the form of 'aaaa:bbbb:xxxx'
 * @param strVec Reference to a vector of strings which will be filled with the
 *               items in the input.
 * @return true if there were items found in the input string.
 */
    bool split_string(const std::string &str, string_vector &strVec) {
        std::string copy(str);
        auto *pch = (char *)copy.c_str();

        pch = strtok(pch, ":");
        while (pch != nullptr) {
            strVec.push_back(pch);
            pch = strtok(nullptr, ":");
        }

        return (!strVec.empty());
    }
}
