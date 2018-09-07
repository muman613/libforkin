//
// Created by muman on 9/6/18.
//

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
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client_utils.h"

int main(int argc, char * argv[]) {
    std::string hostname;

    if (argc == 2) {
        hostname = argv[1];

        printf("Resolving hostname %s\n", hostname.c_str());

        sockaddr_in addr;

        if (resolve_host(hostname, &addr)) {
            printf( "Resolved IP address as %s\n", inet_ntoa(addr.sin_addr));

        }

        client_struct_t babyClient;

        create_inet_client(&babyClient, "babyClient", "google.com", 80);

        if (open_socket_client(&babyClient) == 0) {
            printf("Connection to host completed!\n");
        }
    }

    return 0;
}