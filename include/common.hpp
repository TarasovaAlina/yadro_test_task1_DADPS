#include <iostream>
#include <sys/socket.h>

std::string read_line(int sock) {
    std::string str;
    char ch;
    while (recv(sock, &ch, 1, 0) > 0 && ch != '\n') {
        str += ch;
        if (str.empty()) break;
    }
    return str;
};

class tcp_client {
    int sock;
    struct sockaddr_in addr;
public:
    tcp_client(int port) {
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if (sock < 0) {
            perror("socket");
            exit(1);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    ~tcp_client() { if (sock >= 0) close(sock); }
    void connect_client() {
        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("connect");
            exit(2);
        }
    }
    void send_info(std::string& line) {
         send(sock, line.c_str(), line.size(), 0);
    }
    std::string read_info() {
        return read_line(sock);
    }
};

class tcp_server {
    int sock{}, listener{}, accept_sock{};
    struct sockaddr_in addr;
public:
    tcp_server(int port) {
        listener = socket(AF_INET, SOCK_STREAM, 0);

        if (listener < 0)
        {
            perror("socket");
            exit(1);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

    }
    ~tcp_server() { 
        if (sock >= 0) close(sock); 
        if (accept_sock >= 0) close(accept_sock);
    }

    void bind_server() {
        if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("bind");
            exit(2);
        }
    }
    void listen_server(int n) {
        listen(listener, n);
    }
    int accept_client(){
        accept_sock = accept(listener, NULL, NULL);
        if (accept_sock < 0)
        {
            perror("accept");
            exit(3);
        }
        return accept_sock;
    }
    void send_info(std::string& line) {
         send(accept_sock, line.c_str(), line.size(), 0);
    }
    std::string read_info() {
        return read_line(accept_sock);
    }
    void close_client_socket(){
        close(accept_sock);
    }
};