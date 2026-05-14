#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "json.hpp"
#include <cmath>
#include "common.hpp"

using json = nlohmann::json;

bool check_duplicates(const json& j, const json& j_temp);

int main()
{
    json j_a{}, j_b{};
    int sock_a, sock_b, listener_a, listener_b;
    struct sockaddr_in addr_a, addr_b;

    listener_a = socket(AF_INET, SOCK_STREAM, 0);
    listener_b = socket(AF_INET, SOCK_STREAM, 0);

    if(listener_a < 0 || listener_b < 0)
    {
        perror("socket");
        exit(1);
    }
    
    addr_a.sin_family = AF_INET;
    addr_a.sin_port = htons(3425);
    addr_a.sin_addr.s_addr = htonl(INADDR_ANY);

    addr_b.sin_family = AF_INET;
    addr_b.sin_port = htons(3426);
    addr_b.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listener_a, (struct sockaddr *)&addr_a, sizeof(addr_a)) < 0 || 
        bind(listener_b, (struct sockaddr *)&addr_b, sizeof(addr_b)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener_a, 1);
    listen(listener_b, 2);
    
    while(1)
    {
        sock_a = accept(listener_a, NULL, NULL);
        sock_b = accept(listener_b, NULL, NULL);
        if(sock_a < 0 || sock_b < 0)
        {
            perror("accept");
            exit(3);
        }
            std::string line_a = read_line(sock_a);

            if(line_a.empty()) {
                close(sock_a);
                close(sock_b);
                continue;
            }

            try {
                json j_temp_a = json::parse(line_a);

                if (!check_duplicates(j_a, j_temp_a)) {
                    j_a["timestamp"] = j_temp_a["timestamp"];
                    j_a["x"] = j_temp_a["x"];
                    j_a["y"] = j_temp_a["y"];
                    j_a["z"] = j_temp_a["z"];
                } else {
                    close(sock_a);
                    close(sock_b);
                    continue;
                }

                std::string str_a = j_a.dump() + '\n';
                send(sock_b, str_a.c_str(), str_a.size(), 0);

                std::string line_b = read_line(sock_b);

                json j_temp_b = json::parse(line_b);

                j_b["module"] = j_temp_b["module"];
                j_b["timestamp"] = j_temp_b["timestamp"];

                std::string str_b = j_b.dump() + '\n';
                send(sock_a, str_b.c_str(), str_b.size(), 0);

            } catch(const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }

        close(sock_a);
        close(sock_b);
    }

    return 0;
}

bool check_duplicates(const json& j, const json& j_temp) {
    if (j.empty()) return false;
    double x1 = j["x"].get<double>(),
        y1 = j["y"].get<double>(),
        z1 = j["z"].get<double>(),
        x2 = j_temp["x"].get<double>(),
        y2 = j_temp["y"].get<double>(),
        z2 = j_temp["z"].get<double>();

    if (std::fabs(x1 - x2) < 1e-4 &&
        std::fabs(y1 - y2) < 1e-4 &&
        std::fabs(z1 - z2) < 1e-4) {
        return true;
    }
    return false;
}