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


//static void sendLineToServer(int fd, const std::string & line) {
//    std::string sline = line;
//
//    if (sline.back() != '\n') {
//        sline += '\n';
//    }
//
//    write(fd, sline.c_str(), sline.length());
//}

static void sendLineToServer(int fd, const char * line) {
#if 1
    send(fd, line, strlen(line), 0);
#else
    write(fd, line, strlen(line));
#endif
}

int client_cb(const client_struct_t * client_desc, int fd) {

    printf("client_cb()\n");

    sendLineToServer(fd, "GET / HTTP/1.1\n");
    sendLineToServer(fd, "\n");

    char buf[1024];

#if 1
    ssize_t bytes = recv(fd, buf, sizeof(buf), 0);
#else
    ssize_t bytes = read(fd, &buf, 1024);
#endif

    buf[bytes] = 0;

    printf("%s\n", buf);

    return 0;
}

int main(int argc, char * argv[]) {
    std::string hostname;

    if (argc == 2) {
        hostname = argv[1];

        printf("Resolving hostname %s\n", hostname.c_str());

        sockaddr_in addr = {};

        if (resolve_host(hostname, &addr)) {
            printf( "Resolved IP address as %s\n", inet_ntoa(addr.sin_addr));

        }

        client_struct_t babyClient;

        create_inet_client(&babyClient, "babyClient", hostname, 80);

        if (open_socket_client(&babyClient) == 0) {

//            int retcode = run_socket_client(&babyClient, client_cb);

            int retcode = client_cb(&babyClient, babyClient.server_socket);

            printf("Connection to host completed! retcode = %d\n", retcode);
        }
    }

    return 0;
}