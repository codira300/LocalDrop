#include "tcpserver.h"
char host[INET6_ADDRSTRLEN];

void ErrorLog(const std::string& err){
    std::cout<< err <<std::endl;
    exit(1);

}

TCPServer::TCPServer()
{
    initializeWinsock = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (initializeWinsock != 0)
    {
        ErrorLog("WSAStartup failed to start");
    }
    startServer();
}

void TCPServer::startServer() noexcept{
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    int server_status = getaddrinfo(NULL, DEFAULT_PORT, &hints, &m_serverinfo);

    if(server_status < 0){
        ErrorLog("getaddrinfo failed \n");
    }
    createSocket();
}


void TCPServer::createSocket() noexcept{
    sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(m_serverinfo->ai_addr);
    m_server_socket = socket(m_serverinfo->ai_family, m_serverinfo->ai_socktype, m_serverinfo->ai_protocol);
    if(m_server_socket == INVALID_SOCKET){
        ErrorLog("Error creating socket \n");
    }

    if(bind(m_server_socket, m_serverinfo->ai_addr, (int) m_serverinfo->ai_addrlen) < 0){
        ErrorLog("Binding of the port failed");
    }

    std::cout<<"The server is listening at "<< inet_ntop(m_serverinfo->ai_family, &ipv4->sin_addr, host, sizeof(host))<<":"<<DEFAULT_PORT<< '\n'; 
}

void TCPServer::acceptConnection() noexcept{
    if(listen(m_server_socket, 1) != 0){
        ErrorLog("Failed to listen to client \n");
    }

    m_client_socket = accept(m_server_socket, reinterpret_cast<sockaddr*>(&m_clientaddr), &m_len_clientaddr);

    if (m_client_socket == INVALID_SOCKET){
        ErrorLog("Failed to accept client connection");
    }

    std::cout << "Client connected! \n";
    performHandshake();
}

namespace LocalDropProtocol
{
    const std::string BAD_REQUEST =
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";
    
    const std::string GOOD_REQUEST =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 14\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "LocalDrop OK";
}

void TCPServer::performHandshake() noexcept{
    char buffer[4096];
    std::string request;

    while (request.find("\r\n\r\n") == std::string::npos){
        int bytes_received = recv( m_client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == 0) {
            std::cout << "Client disconnected during handshake.\n";
            return;
        }

        if (bytes_received == SOCKET_ERROR){
            ErrorLog("Failed to receive handshake");
        }

        buffer[bytes_received] = '\0';
        request += buffer;

        if (request.size() > 8192) ErrorLog("HTTP header too large");
    }

    const std::string& response = request.rfind("GET /handshake", 0) == 0 ? LocalDropProtocol::GOOD_REQUEST : LocalDropProtocol::BAD_REQUEST;
    std::cout<<"HTTP Request \n";
    std::cout << request << "\n";

    int bytes_sent = send(m_client_socket, response.c_str(), static_cast<int>(response.size()), 0 );

    if (bytes_sent == SOCKET_ERROR) {
        ErrorLog("Failed to send handshake response");
    }

    std::cout << "Handshake successful! \n";

    communicate();

}

void TCPServer::communicate() noexcept{
    char buffer[4096];
    int bytes_received = recv(m_client_socket, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes_received] = '\0';

    if (bytes_received == 0){
        ErrorLog("Client disconnected.");
    }
    if (bytes_received == SOCKET_ERROR){
        ErrorLog("Connection Error");
    }

    std::string_view message(buffer);

    std::cout << message;
    const bool disconnect = message.starts_with("DISCONNECT");
    std::string_view response = disconnect ? "DISCONNECT" : "ACK";

    int bytes_sent = send(m_client_socket, response.data(), static_cast<int>(response.size()), 0);
    if (bytes_sent == SOCKET_ERROR) {
        std::cout << "Failed to send response.\n";
        return;
    }

    if (disconnect) std::cout << "Client requested disconnect.\n";
}

void TCPServer::closeSocket() noexcept{
    if (m_client_socket != INVALID_SOCKET) {
        closesocket(m_client_socket);
        m_client_socket = INVALID_SOCKET;
    }
    if (m_server_socket != INVALID_SOCKET){
        closesocket(m_server_socket);
         m_server_socket = INVALID_SOCKET;
    }
    if (m_serverinfo != nullptr) {
        freeaddrinfo(m_serverinfo);
        m_serverinfo = nullptr;
    }

    WSACleanup();
}

TCPServer::~TCPServer()
{
    closeSocket();
}





