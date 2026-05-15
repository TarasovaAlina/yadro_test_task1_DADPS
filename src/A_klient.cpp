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

std::string load_data(json& file);

int main()
{
    json file{};

    tcp_client A{3425};
    A.connect_client();

    while (true) {
        std::string str = load_data(file);
        A.send_info(str);

        std::string result = A.read_info();
        
        std::cout << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    return 0;
}

std::string load_data(json& file){
    static double t = 0;

    file["version"] = 1;
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