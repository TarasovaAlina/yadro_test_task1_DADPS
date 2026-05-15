#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "../include/common.hpp"
#include "../include/json.hpp"

using json = nlohmann::json;

std::string load_set_data(std::string& line);

int main() {

    tcp_client B{3426};
    B.connect_client();

    while (true) {
        std::string str = B.read_info();
        if (str.empty()) break;
        
        std::string line = load_set_data(str);
        B.send_info(line);

    }

    return 0;
}

std::string load_set_data(std::string& line){

    json j = json::parse(line), file;
    double x = j["x"], y = j["y"], z = j["z"], module = sqrt(x*x + y*y + z*z);

    file["version"] = 1;
    file["timestamp"] = j["timestamp"];
    file["module"] = module;

    return (file.dump() + "\n");
}
