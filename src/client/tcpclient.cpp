#include "tcpclient.h"

TCPClient::TCPClient(const std::string& clientId)
    : m_client_id(clientId)
{
    initializeWinsock = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (initializeWinsock != 0) ErrorLog("WSAStartup failed to start");

    connectToServer();
}
void TCPClient::connectToServer(){
    addrinfo hints{};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int client_status = getaddrinfo(SERVER_HOST, DEFAULT_PORT, &hints, &m_serverinfo);
    if (client_status != 0) ErrorLog("getaddrinfo failed");

    createSocket();
}

void TCPClient::createSocket(){
    m_client_socket = socket(
        m_serverinfo->ai_family,
        m_serverinfo->ai_socktype,
        m_serverinfo->ai_protocol
    );

    if (m_client_socket == INVALID_SOCKET) ErrorLog("Error creating client socket");

    if (connect(m_client_socket,m_serverinfo->ai_addr,static_cast<int>(m_serverinfo->ai_addrlen)) == SOCKET_ERROR) ErrorLog("Failed to connect to server");

    std::cout << "Connected to LocalDrop server!" << std::endl;
    performHandshake();
}

void TCPClient::performHandshake(){
    const std::string& clientPublicKey = m_crypto.getPublicKey();

    std::string request =
        "GET /handshake HTTP/1.1\r\n"
        "Connection: keep-alive\r\n"
        "X-LocalDrop-Key: " +
        clientPublicKey +
        "\r\n";

    int bytes_sent = send(m_client_socket, request.data(), static_cast<int>(request.size()), 0);

    if (bytes_sent == SOCKET_ERROR) ErrorLog("Failed to send handshake");

    char buffer[4096];

    int bytes_received = recv(m_client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received == 0) ErrorLog("Server disconnected during handshake");

    if (bytes_received == SOCKET_ERROR) ErrorLog("Failed to receive handshake response");


    buffer[bytes_received] = '\0';

    std::string_view response(buffer);

    std::cout << "Server response:\n"<< response<<"\n";

    if (!response.starts_with("HTTP/1.1 200 OK")) ErrorLog("LocalDrop handshake failed");

    std::cout << "Handshake successful!\n\n";
}

void TCPClient::start() {communicate();}
void TCPClient::communicate(){
    while (true){
        const std::string message = m_message_generator.getRandomMessage();
        std::string data =
        "CLIENT_ID:" + m_client_id + "\n"
        "MESSAGE:" + message + "\n\n";
    
        int bytes_sent = send(m_client_socket, data.data(), static_cast<int>(data.size()), 0);
        if (bytes_sent == SOCKET_ERROR) ErrorLog("Failed to send message");

        if (message == DISCONNECT){
            std::cout << "Disconnect requested.\n";
            break;
        }   

        char buffer[4096];
        int bytes_received = recv(m_client_socket, buffer, sizeof(buffer) - 1, 0);
        buffer[bytes_received] = '\0';

        if (bytes_received == 0){
            std::cout << "Server disconnected." <<"\n";
            return;
        }

        if (bytes_received == SOCKET_ERROR) ErrorLog("Failed to receive response");

        std::string_view response(buffer);
        std::cout << "Server: " << response <<"\n";
    }
}

void TCPClient::closeSocket(){
    if (m_client_socket != INVALID_SOCKET) {
        closesocket(m_client_socket);
        m_client_socket = INVALID_SOCKET;
    }

    if (m_serverinfo != nullptr) {
        freeaddrinfo(m_serverinfo);
        m_serverinfo = nullptr;
    }

    WSACleanup();
}

TCPClient::~TCPClient()
{
    closeSocket();
}