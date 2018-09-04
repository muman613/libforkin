//
// Created by muman on 9/3/18.
//

#ifndef FORKIN_SERVER_UTILS_H
#define FORKIN_SERVER_UTILS_H

#include <string>

typedef struct _server_struct {
    std::string         server_name;
    int                 server_type;
    int                 server_socket;
    int                 server_port;
    std::string         server_domain_name;
} server_struct_t;

int create_inet_server(server_struct_t * server_desc,
        const std::string & server_name,
        int server_port);
int create_local_server(server_struct_t * server_desc,
        const std::string & server_name,
        const std::string & server_domain);

int open_socket_server(server_struct_t *server_desc);

#endif //FORKIN_SERVER_UTILS_H
