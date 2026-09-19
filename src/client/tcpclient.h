#ifndef TCPCLIENT
#define TCPCLIENT

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>
#include <sodium.h>
#include "../crypto/crypto.h"
#include "../error/error.h"
#include "../message.h"
#include "../messagegenerator.h"

#define DEFAULT_PORT "8080"
#define SERVER_HOST "127.0.0.1"
#define DISCONNECT "Disconnect"

class TCPClient{
public:
    TCPClient(const std::string& clientId);
    ~TCPClient();
    void start();
    void connectToServer();

private:
    WSADATA wsadata;
    int initializeWinsock;

    addrinfo* m_serverinfo = nullptr;

    SOCKET m_client_socket = INVALID_SOCKET;
    std::string m_client_id;

    void createSocket();
    void performHandshake();
    void communicate();
    void closeSocket();
    Crypto m_crypto;
    MessageGenerator m_message_generator;

};

#endif