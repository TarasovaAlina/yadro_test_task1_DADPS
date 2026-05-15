#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include "../include/common.hpp"
#include "../include/json.hpp"


using json = nlohmann::json;

bool check_duplicates(const json& j, const json& j_temp);

int main()
{
    tcp_server A(3425), B(3426);
    json j_a{}, j_b{};

    A.bind_server();
    B.bind_server();

    A.listen_server(1);
    B.listen_server(2);

    while(1)
    {
        A.accept_client();
        B.accept_client();

            std::string line_a = A.read_info();

            if(line_a.empty()) {
                A.close_client_socket();
                B.close_client_socket();
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
                    A.close_client_socket();
                    B.close_client_socket();
                    continue;
                }

                std::string str_a = j_a.dump() + '\n';
                B.send_info(str_a);

                std::string line_b = B.read_info();

                json j_temp_b = json::parse(line_b);

                j_b["module"] = j_temp_b["module"];
                j_b["timestamp"] = j_temp_b["timestamp"];

                std::string str_b = j_b.dump() + '\n';
                A.send_info(str_b);

            } catch(const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
    }
    A.close_client_socket();
    B.close_client_socket();

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