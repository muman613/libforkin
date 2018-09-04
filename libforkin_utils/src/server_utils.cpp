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

int create_inet_server(server_struct_t * server_desc,
                       const std::string & server_name,
                       int server_port)
{
    assert(server_desc != nullptr);

    server_desc->server_name    = server_name;
    server_desc->server_port    = server_port;
    server_desc->server_type    = AF_INET;
    server_desc->server_socket  = -1;
    return 0;
}

int create_local_server(server_struct_t * server_desc,
                        const std::string & server_name,
                        const std::string & server_domain)
{
    server_desc->server_name        = server_name;
    server_desc->server_port        = -1;
    server_desc->server_type        = AF_LOCAL;
    server_desc->server_domain_name = server_domain;
    server_desc->server_socket      = -1;

    return 0;
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
            return 10;
        }

        // TODO: Handle the case of the abstract namespace.
        struct sockaddr_un addr = {0};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, server_desc->server_domain_name.c_str(), sizeof(addr.sun_path)-1);

        // Bind the socket to the domain name
        if (bind(server_desc->server_socket, (const sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("bind failed");
            close(server_desc->server_socket);
            return 10;
        }

    } else if (server_desc->server_type == AF_INET) {
        /**
         * Here we create the internet domain socket.
         */
        syslog(LOG_USER|LOG_DEBUG, "creating an internet domain socket server");

        server_desc->server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_desc->server_socket == -1) {
            perror("open_socket_server call to socket failed");
            return 10;
        }

        int opt = 1;
        if (setsockopt(server_desc->server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)))
        {
            perror("setsockopt");
            close(server_desc->server_socket);
            return 10;
        }
        struct sockaddr_in addr = { 0 };

        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(server_desc->server_port);

        if (bind(server_desc->server_socket, (const sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("bind failed");
            close(server_desc->server_socket);
            return 10;
        }
    }

    syslog(LOG_USER|LOG_DEBUG, "socket fd = %d", server_desc->server_socket);

    return 0;
}
