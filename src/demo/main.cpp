// Minimal demo: start EventBase + TCPSever and run loop
#include "../../include/EventBase.hpp"
#include "../../include/Sever/TCPSever.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    int port = 8080;
    if (argc >= 2) port = std::atoi(argv[1]);

    EventBase base;
    TCPSever server(&base);

    if (server.listen(port) != 0) {
        std::cerr << "server.listen(" << port << ") failed\n";
        return 1;
    }

    std::cout << "MiniEvent demo listening on port " << port << "\n";
    base.loop(); // blocking event loop
    return 0;
}
