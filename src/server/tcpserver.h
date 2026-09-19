#ifndef TCPSERVER

#define TCPSERVER

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <sodium.h>

#include "../crypto/crypto.h"
#include "../message.h"

#define DEFAULT_PORT "8080"

class TCPServer{
public:
    TCPServer();
    ~TCPServer();
    void acceptConnection() noexcept;
    void shutdown() noexcept;


private:
    WSADATA wsadata;

    int initializeWinsock;

    addrinfo hints{};

    sockaddr_in m_sockaddr;

    addrinfo* m_serverinfo = nullptr;

    void createSocket() noexcept;

    void startServer() noexcept;

    void handleClient(SOCKET clientSocket) noexcept;

    void performHandshake(SOCKET clientSocket) noexcept;

    void TCPServer::invalidate_socket();

    void communicate(SOCKET clientSocket) noexcept;

    void closeSocket() noexcept;

    Crypto m_crypto;

    SOCKET m_server_socket = INVALID_SOCKET;

    std::vector<std::thread> m_clientThreads;

    std::atomic<bool> m_running = true;
};

#endif