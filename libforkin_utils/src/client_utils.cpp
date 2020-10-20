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
#include <sys/wait.h>
#include <cassert>
#include <cstring>
#include <netinet/in.h>
#include <sys/un.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

#include "client_utils.h"

void create_inet_client(client_struct_t * client_desc,
                        const std::string & client_name,
                        const std::string & server_name,
                        int server_port)
{
    assert(client_desc != nullptr);

    client_desc->client_name = client_name;
    client_desc->server_name = server_name;
    client_desc->server_port = server_port;
    client_desc->server_type = AF_INET;
}

/**
 * Create a client to connect to a local domain socket.
 *
 * @param client_desc
 * @param client_name
 * @param server_name
 * @param server_domain
 */
void create_local_client(client_struct_t * client_desc,
                         const std::string & client_name,
                         const std::string & server_name,
                         const std::string & server_domain)
{
    assert(client_desc != nullptr);

    client_desc->client_name        = client_name;
    client_desc->server_name        = server_name;
    client_desc->server_domain_name = server_domain;
    client_desc->server_type        = AF_LOCAL;
}

/**
 * Create a client connection to the host specified in the client_desc.
 *
 * @param client_desc
 * @return
 */
int open_socket_client(client_struct_t *client_desc) {
    assert(client_desc != nullptr);

    if (client_desc->server_type == AF_LOCAL) {
        // TODO: Implement local domain client
    } else if (client_desc->server_type == AF_INET) {
        sockaddr_in addr = {};
        if (resolve_host(client_desc->server_name, &addr)) {
            client_desc->server_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (client_desc->server_socket == -1) {
                perror("socket failed ");
                return -1;
            }

            addr.sin_family = AF_INET;
            addr.sin_port   = htons(client_desc->server_port);

            if (connect(client_desc->server_socket, (sockaddr *)&addr, sizeof(addr)) != 0) {
                perror("connect fauked ");
                close(client_desc->server_socket);
                return -1;
            }
        } else {
            return -1;
        }
    }

    return 0;
}

/**
 *
 * @param hostname Hostname or IP to resolve to address.
 * @param serv_addr pointer to sockaddr_t address
 * @return true if successful, false otherwise.
 */
bool resolve_host(const std::string & hostname, sockaddr_in * serv_addr) {
    assert(serv_addr != nullptr);

    hostent * server_ent = gethostbyname(hostname.c_str());
    if (server_ent == nullptr) {
        perror("gethostbyname failed : ");
        return false;
    }

    bzero((char *)serv_addr, sizeof(serv_addr));
    bcopy((char *)server_ent->h_addr, (char *)&serv_addr->sin_addr.s_addr,
          server_ent->h_length);

    return true;
}