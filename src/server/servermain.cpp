#include "tcpserver.h"

#include <iostream>
#include <thread>


int main(){
    TCPServer server;

    std::thread serverThread(&TCPServer::acceptConnection, &server);

    std::cout<< "Press ENTER to shutdown the server.\n";
    std::cin.get();

    server.shutdown();

    if (serverThread.joinable()){
        serverThread.join();
    }

    std::cout<< "Server shutdown complete.\n";
    return 0;
}