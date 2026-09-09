#ifndef TCPSERVER
#define TCPSERVER
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>

#define DEFAULT_PORT "8080"
class TCPServer{
    public: 
        TCPServer();
        ~TCPServer();
        void acceptConnection();
       

        private:
            WSADATA wsadata;
            int initializeWinsock;
            addrinfo hints{};
            struct sockaddr_in m_sockaddr;
            addrinfo* m_serverinfo = nullptr;
            void createSocket();
            void startServer();  
            void closeSocket();
            SOCKET m_server_socket = INVALID_SOCKET;
            SOCKET m_client_socket = INVALID_SOCKET;
            sockaddr_in m_clientaddr{};
            int m_len_clientaddr = sizeof(m_clientaddr);
                   
};

#endif