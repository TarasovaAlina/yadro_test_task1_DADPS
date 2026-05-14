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

std::string load_data(json& file);

int main()
{
    json file{};

    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

    while (true) {
        std::string str = load_data(file);
        send(sock, str.c_str(), str.size(), 0);

        std::string result = read_line(sock);
        
        std::cout << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    close(sock);

    return 0;
}

std::string load_data(json& file){
    static double t = 0;

    file["x"] = 2.0 * std::sin(t);
    file["y"] = 2.0 * std::sin(t + 2.0);
    file["z"] = 2.0 * std::sin(t + 4.0);

    t += 0.05;

    auto current_time = std::chrono::system_clock::now();
    auto duration = current_time.time_since_epoch();
    auto epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    file["timestamp"] = epoch_time;

    return (file.dump() + "\n");
}