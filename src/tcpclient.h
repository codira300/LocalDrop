#ifndef TCPCLIENT
#define TCPCLIENT

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>

#define DEFAULT_PORT "8080"
#define SERVER_HOST "127.0.0.1"

class TCPClient
{
public:
    TCPClient();
    ~TCPClient();

    void connectToServer();

private:
    WSADATA wsadata;
    int initializeWinsock;

    addrinfo* m_serverinfo = nullptr;

    SOCKET m_client_socket = INVALID_SOCKET;

    void createSocket();
    void closeSocket();
};

#endif