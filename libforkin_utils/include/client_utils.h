#ifndef FORKIN_CLIENT_UTILS_H
#define FORKIN_CLIENT_UTILS_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <functional>

/**
 * Structure used to represent the client object.
 */
typedef struct _client_struct {
    std::string         client_name;
    std::string         server_name;
    int                 server_type         = -1;
    int                 server_socket       = -1;
    int                 server_port         = -1;
    std::string         server_domain_name;
} client_struct_t;

using client_callback = std::function<int(const client_struct_t *, int fd)>;

void create_inet_client(client_struct_t * client_desc,
                        const std::string & client_name,
                        const std::string & server_name,
                        int server_port);

void create_local_client(client_struct_t * client_desc,
                         const std::string & client_name,
                         const std::string & server_name,
                         const std::string & server_domain);

int open_socket_client(client_struct_t *server_desc);

int run_socket_client(client_struct_t * server_desc, client_callback cb);

bool resolve_host(const std::string & hostname, sockaddr_in * serv_addr);


#endif //FORKIN_CLIENT_UTILS_H
