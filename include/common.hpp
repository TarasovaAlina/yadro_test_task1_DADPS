#include <iostream>
#include <sys/socket.h>

std::string read_line(int sock);

std::string read_line(int sock) {
    std::string str;
    char ch;
    while (recv(sock, &ch, 1, 0) > 0 && ch != '\n') {
        str += ch;
        if (str.empty()) break;
    }
    return str;
}