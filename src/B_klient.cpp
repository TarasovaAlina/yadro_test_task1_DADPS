#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include "json.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "common.hpp"

using json = nlohmann::json;

std::string load_set_data(std::string& line);

int main() {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    while (true) {
        std::string str = read_line(sock);
        if (str.empty()) break;
        
        std::string line = load_set_data(str);
        send(sock, line.c_str(), line.size(), 0);

    }
    close(sock);
    return 0;
}

std::string load_set_data(std::string& line){

    json j = json::parse(line), file;
    double x = j["x"], y = j["y"], z = j["z"], module = sqrt(x*x + y*y + z*z);

    auto current_time = std::chrono::system_clock::now();
    auto duration = current_time.time_since_epoch();
    auto epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    file["timestamp"] = epoch_time;
    file["module"] = module;

    return (file.dump() + "\n");
}