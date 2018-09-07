//
// Created by muman on 9/3/18.
//

#ifndef FORKIN_SERVER_UTILS_H
#define FORKIN_SERVER_UTILS_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Structure used to represent the server object.
 */
typedef struct _server_struct {
    std::string         server_name;
    int                 server_type         = -1;
    int                 server_socket       = -1;
    int                 server_port         = -1;
    std::string         server_domain_name;
} server_struct_t;

using server_callback = std::function<int(const server_struct_t *, int fd, struct sockaddr *)>;

void create_inet_server(server_struct_t * server_desc,
        const std::string & server_name,
        int server_port);

void create_local_server(server_struct_t * server_desc,
        const std::string & server_name,
        const std::string & server_domain);

int open_socket_server(server_struct_t *server_desc);

int run_socket_server(server_struct_t * server_desc, server_callback cb);

#endif //FORKIN_SERVER_UTILS_H
