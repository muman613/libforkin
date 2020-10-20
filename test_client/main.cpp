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
#include <sys/wait.h>
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

using namespace std;

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

    cout << "client_cb()" << endl;

    sendLineToServer(fd, "GET / HTTP/1.1\n");
    sendLineToServer(fd, "HOST: developer.mozilla.org\n");
    sendLineToServer(fd, "\n");

    char buf[8192];

    std::string html;

    while (true) {
      ssize_t bytes;

      bytes = recv(fd, buf, 8192, 0);
      cout << "bytes " << bytes << endl;
      cout << "buff[0]" << (int)buf[0] << endl;

      html += std::string(buf, bytes);

      if (bytes != 8192)
        break;
    }

    cout << html << endl;

    return 0;
}

int main(int argc, char * argv[]) {
    int         result = 0;
    std::string hostname;

    if (argc == 2) {
        hostname = argv[1];

        cout << "Resolving hostname " << hostname << endl;

        sockaddr_in addr = {};

        if (resolve_host(hostname, &addr)) {
            cout << "Resolved IP address as " << inet_ntoa(addr.sin_addr) << endl;
        }

        client_struct_t babyClient;

        create_inet_client(&babyClient, "babyClient", hostname, 80);

        if (open_socket_client(&babyClient) == 0) {
            int retcode = client_cb(&babyClient, babyClient.server_socket);
            cout << "Connection to host completed! retcode = " << retcode << endl;
        }
    } else {
      cerr << "ERROR: Must specify hostname" << endl;
      result = 10;
    }

    return result;
}