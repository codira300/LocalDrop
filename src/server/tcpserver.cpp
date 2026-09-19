#include "tcpserver.h"

#include "../error/error.h"

char host[INET6_ADDRSTRLEN];


TCPServer::TCPServer(){
    initializeWinsock = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (initializeWinsock != 0){
        ErrorLog("WSAStartup failed to start");
        return;
    }

    startServer();
}

void TCPServer::startServer() noexcept{
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int server_status = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &m_serverinfo);
    if (server_status != 0){
        ErrorLog("getaddrinfo failed");
        return;
    }

    createSocket();
}


void TCPServer::createSocket() noexcept{
    sockaddr_in* ipv4 =  reinterpret_cast<sockaddr_in*>(m_serverinfo->ai_addr);

    m_server_socket = socket(m_serverinfo->ai_family, m_serverinfo->ai_socktype, m_serverinfo->ai_protocol);
    if (m_server_socket == INVALID_SOCKET){
        ErrorLog("Error creating socket");
        return;
    }

    if (bind(m_server_socket, m_serverinfo->ai_addr, static_cast<int>(m_serverinfo->ai_addrlen)) == SOCKET_ERROR){
        ErrorLog("Binding of the port failed");
        return;
    }

    std::cout<< "The server is listening at "<< inet_ntop(m_serverinfo->ai_family, &ipv4->sin_addr, host, sizeof(host)) << ":" << DEFAULT_PORT << '\n';
}


void TCPServer::acceptConnection() noexcept{
    if (listen(m_server_socket, SOMAXCONN) == SOCKET_ERROR){
        ErrorLog("Failed to listen to client");
        return;
    }

    while (m_running){
        sockaddr_in clientAddress{};

        int clientAddressLength = sizeof(clientAddress);

        SOCKET clientSocket = accept(m_server_socket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);

        if (clientSocket == INVALID_SOCKET){
            if (!m_running) break;

            ErrorLog("Failed to accept client connection");
            continue;
        }

        std::cout<< "Client connected!\n";

        m_clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }

    std::cout<< "Server stopped accepting new clients.\n";
}


void TCPServer::handleClient(SOCKET clientSocket) noexcept{
    std::cout<< "Starting handshake...\n";

    performHandshake(clientSocket);
    communicate(clientSocket);
    closesocket(clientSocket);

    std::cout<< "Client connection closed.\n";
}


void TCPServer::performHandshake(SOCKET clientSocket) noexcept{
    char buffer[4096];
    std::string request;

    const std::string& serverPublicKey = m_crypto.getPublicKey();

    struct LocalDropProtocol{
        const std::string BAD_REQUEST =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";


        std::string GOOD_REQUEST;

        LocalDropProtocol(const std::string& serverPublicKey)
            : GOOD_REQUEST(
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 14\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: keep-alive\r\n"
                "X-LocalDrop-Key: " +
                serverPublicKey +
                "\r\n"
            )
        {}
    };

    LocalDropProtocol protocol(serverPublicKey);

    while (request.find("X-LocalDrop-Key: ") == std::string::npos){
        int bytes_received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == 0){
            std::cout<< "Client disconnected during handshake.\n";
            return;
        }

        if (bytes_received == SOCKET_ERROR){
            ErrorLog("Failed to receive handshake");
            return;
        }

        buffer[bytes_received] = '\0';

        request.append(buffer, bytes_received);

        if (request.size() > 8192){
            ErrorLog("HTTP header too large");
            return;
        }
    }

    std::cout<< "HTTP Request:\n"<< request<< "\n";

    bool validRequest = request.rfind("GET /handshake", 0) == 0;

    const std::string keyHeader = "X-LocalDrop-Key: ";

    std::size_t keyPosition = request.find(keyHeader);

    if (keyPosition ==std::string::npos){
        ErrorLog("Client public key missing");
        return;
    }

    keyPosition += keyHeader.size();
    std::size_t keyEnd = request.find("\r\n", keyPosition);
    if (keyEnd == std::string::npos){
        ErrorLog("Invalid public key header");
        return;
    }

    std::string clientPublicKey = request.substr(keyPosition, keyEnd - keyPosition);

    if (!m_crypto.deriveServerSessionKeys(clientPublicKey)){
        ErrorLog("Failed to establish shared secret");
        return;
    }
    std::cout<<"Shared secret established!\n";

    const std::string& response = validRequest ? protocol.GOOD_REQUEST : protocol.BAD_REQUEST;
    int bytes_sent = send(clientSocket, response.data(), static_cast<int>(response.size()), 0);
    if (bytes_sent == SOCKET_ERROR){
        ErrorLog("Failed to send handshake response");
        return;
    }

    std::cout<< "Handshake successful!\n";
}


void TCPServer::communicate(SOCKET clientSocket) noexcept{
    std::string receiveBuffer;

    while (m_running){
        char buffer[4096];

        int bytes_received =
            recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes_received == 0){
            std::cout<< "Client disconnected.\n";
            break;
        }

        if (bytes_received == SOCKET_ERROR){
            if (!m_running) break;

            ErrorLog("Connection Error");
            break;
        }


        receiveBuffer.append(buffer, bytes_received);
        std::size_t messageEnd;

        while ((messageEnd = receiveBuffer.find("\n\n")) != std::string::npos){
            std::string rawMessage = receiveBuffer.substr(0, messageEnd);

            receiveBuffer.erase(0, messageEnd + 2);
            std::unique_ptr<Message> parsedMessage = parseMessage(rawMessage);

            if (!parsedMessage){
                ErrorLog("Invalid message received.");
                continue;
            }

            std::cout<< "Client ID: "<< parsedMessage->clientId<< "\n";
            std::cout<< "Message: "<< parsedMessage->message<< "\n";

            const std::string disconnect {"Disconnect"};

            std::string_view response = (parsedMessage->message == disconnect) ? "DISCONNECTED" : "ACKNOWLEDGED";
            int bytes_sent = send(clientSocket, response.data(), static_cast<int>(response.size()), 0);
            if (bytes_sent == SOCKET_ERROR){
                ErrorLog("Failed to send response.");
                return;
            }

            if (response == "DISCONNECTED"){
                std::cout<< parsedMessage->clientId <<" requested disconnect.\n";
                return;
            }
        }
    }
}

void TCPServer::invalidate_socket(){
    m_running = false;

    if (m_server_socket !=INVALID_SOCKET){
        closesocket(m_server_socket);
        m_server_socket = INVALID_SOCKET;
    }
}

void TCPServer::shutdown() noexcept{
    invalidate_socket();
    std::cout<< "Server shutdown requested.\n";
}


void TCPServer::closeSocket() noexcept{
    invalidate_socket();

    if (m_serverinfo != nullptr){
        freeaddrinfo(m_serverinfo);
        m_serverinfo = nullptr;
    }

    for (std::thread& clientThread : m_clientThreads) if (clientThread.joinable()) clientThread.join();

    WSACleanup();
}

TCPServer::~TCPServer(){
    closeSocket();
}