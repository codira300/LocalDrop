#include "tcpclient.h"

void ErrorLog(const std::string& err)
{
    std::cout << err << std::endl;
    exit(1);
}

TCPClient::TCPClient()
{
    initializeWinsock = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (initializeWinsock != 0)
    {
        ErrorLog("WSAStartup failed to start");
    }

    connectToServer();
}

void TCPClient::connectToServer()
{
    addrinfo hints{};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int client_status = getaddrinfo(
        SERVER_HOST,
        DEFAULT_PORT,
        &hints,
        &m_serverinfo
    );

    if (client_status != 0)
    {
        ErrorLog("getaddrinfo failed");
    }

    createSocket();
}

void TCPClient::createSocket(){
    m_client_socket = socket(
        m_serverinfo->ai_family,
        m_serverinfo->ai_socktype,
        m_serverinfo->ai_protocol
    );

    if (m_client_socket == INVALID_SOCKET)    {
        ErrorLog("Error creating client socket");
    }

    if (connect(m_client_socket,m_serverinfo->ai_addr,static_cast<int>(m_serverinfo->ai_addrlen)) == SOCKET_ERROR){
        ErrorLog("Failed to connect to server");
    }

    std::cout << "Connected to LocalDrop server!" << std::endl;
}

void TCPClient::closeSocket()
{
    if (m_client_socket != INVALID_SOCKET)
    {
        closesocket(m_client_socket);
        m_client_socket = INVALID_SOCKET;
    }

    if (m_serverinfo != nullptr)
    {
        freeaddrinfo(m_serverinfo);
        m_serverinfo = nullptr;
    }

    WSACleanup();
}

TCPClient::~TCPClient()
{
    closeSocket();
}