//
// Created by muman on 9/4/18.
//

#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <cstring>

#include "server_utils.h"

/**
 * Example server main handler.
 *
 * @param server_desc
 * @param fd
 * @param addr
 * @return
 */
int server_cb(const server_struct_t * server_desc, int fd, struct sockaddr * addr) {
    printf("server_cb(%p, %d, %p)\n", server_desc, fd, addr);

    char buffer[128];
    bool done = false;
    int return_code = 0;

    while (!done) {
        ssize_t nbytes = recv(fd, buffer, sizeof(buffer), 0);

        if (nbytes > 1) {
            buffer[nbytes - 1] = '\0';
        }
        printf("buffer %s\n", buffer);


        std::string shutdown_cmd = "shutdown";
        if (strncmp(buffer, shutdown_cmd.c_str(), shutdown_cmd.size()) == 0) {
            done = true;
            return_code = 1;
        }
    }

    close(fd);
    return return_code;
}

/**
 *
 * @param argc
 * @param arg
 * @return
 */
int main(int argc, const char * arg[]) {
    int             result = 1;

    server_struct_t simpleServer;

    create_inet_server(&simpleServer, "simpleServer", 8080);

    if (open_socket_server(&simpleServer) == 0) {
        if (run_socket_server(&simpleServer, server_cb) == 0) {
            printf("Socket server exited...\n");
            result = 0;
        }
    }

    return result;
}
