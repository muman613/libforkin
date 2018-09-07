/**
 * server_utils.cpp
 *
 * @author Michael A. Uman
 * @date   August 31, 2018
 */

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <wait.h>
#include <cassert>
#include <cstring>
#include <netinet/in.h>
#include <sys/un.h>
#include <syslog.h>

#include "server_utils.h"

/**
 * Initialize the server_desc with values required to run an internet
 * domain server.
 *
 * @param server_desc
 * @param server_name
 * @param server_port
 * @return
 */
void create_inet_server(server_struct_t * server_desc,
                       const std::string & server_name,
                       int server_port)
{
    assert(server_desc != nullptr);

    server_desc->server_name = server_name;
    server_desc->server_port = server_port;
    server_desc->server_type = AF_INET;
    server_desc->server_socket = -1;
}

void create_local_server(server_struct_t * server_desc,
                        const std::string & server_name,
                        const std::string & server_domain)
{
    server_desc->server_name        = server_name;
    server_desc->server_port        = -1;
    server_desc->server_type        = AF_LOCAL;
    server_desc->server_domain_name = server_domain;
    server_desc->server_socket      = -1;
}

/**
 * Open a socket server
 *
 * @param server_desc pointer to server struct to open.
 *
 * @return
 */
int open_socket_server(server_struct_t *server_desc) {
    assert(server_desc != nullptr);
    syslog(LOG_USER|LOG_DEBUG, "open_socket_server(%p) server_name = %s", (void *)server_desc,
            server_desc->server_name.c_str());

    server_desc->server_socket = -1;

    if (server_desc->server_type == AF_LOCAL) {
        /**
         * Here we create the socket using UNIX/local domain sockets
         */
        syslog(LOG_USER|LOG_DEBUG, "creating a local domain socket server");

        server_desc->server_socket = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (server_desc->server_socket == -1) {
            perror("open_socket_server call to socket failed");
            return -1;
        }

        // TODO: Handle the case of the abstract namespace.
        struct sockaddr_un addr = {0};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, server_desc->server_domain_name.c_str(), sizeof(addr.sun_path)-1);

        // Bind the socket to the domain name
        if (bind(server_desc->server_socket, (const sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("bind failed");
            close(server_desc->server_socket);
            return -1;
        }

    } else if (server_desc->server_type == AF_INET) {
        /**
         * Here we create the internet domain socket.
         */
        syslog(LOG_USER|LOG_DEBUG, "creating an internet domain socket server");

        server_desc->server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_desc->server_socket == -1) {
            perror("open_socket_server call to socket failed");
            return -1;
        }

        int opt = 1;
        if (setsockopt(server_desc->server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)))
        {
            perror("setsockopt");
            close(server_desc->server_socket);
            return -1;
        }
        struct sockaddr_in addr = { 0 };

        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(server_desc->server_port);

        if (bind(server_desc->server_socket, (const sockaddr *)&addr, sizeof(addr)) != 0) {
            perror("bind failed");
            close(server_desc->server_socket);
            return -1;
        }

        if (listen(server_desc->server_socket, 5) != 0) {
            perror("listen failed");
            close(server_desc->server_socket);
            return -1;
        }
    }

    syslog(LOG_USER|LOG_DEBUG, "socket fd = %d", server_desc->server_socket);

    return 0;
}

/**
 * Run the socket server, call callback when a connection is received.
 *
 * @param server_desc
 * @param cb
 * @return
 */
int run_socket_server(server_struct_t * server_desc, server_callback cb) {
    assert(server_desc != nullptr);
    int         server_fd = server_desc->server_socket;
    fd_set      read_set;
    int         nfds = server_fd + 1;
    bool        done = false;

    syslog(LOG_USER|LOG_DEBUG, "run_socket_server(%p, ...)", server_desc);

    while (!done) {
        struct timeval t = { 0, 500000 };

        FD_ZERO(&read_set);
        FD_SET(server_fd, &read_set);

        int select_res = select(nfds, &read_set, nullptr, nullptr, &t);
        if (select_res > 0) {
            if (FD_ISSET(server_fd, &read_set)) {
                struct sockaddr_in client_addr = {0};
                socklen_t client_addr_len = sizeof(client_addr);

                // TODO: Adapt to work with a domain socket server
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                printf("accept connection on socket %d\n",client_fd);

                if (cb(server_desc, client_fd, (struct sockaddr *)&client_addr) != 0) {
                    done = true;
                }
            }
        }
    }

    syslog(LOG_USER|LOG_DEBUG, "run_socket_server exiting");

    return 0;
}
