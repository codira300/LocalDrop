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

void TCPServer::startServer(){
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


void TCPServer::createSocket(){
    m_server_socket = socket(m_serverinfo->ai_family, m_serverinfo->ai_socktype, m_serverinfo->ai_protocol);
    if(m_server_socket == INVALID_SOCKET){
        ErrorLog("Error creating socket \n");
    }

    if(bind(m_server_socket, m_serverinfo->ai_addr, (int) m_serverinfo->ai_addrlen) < 0){
        ErrorLog("Binding of the port failed");
    }

    std::cout<<"The server is listening at "<< inet_ntop(m_serverinfo->ai_family, m_serverinfo->ai_addr, host, sizeof(host))<<":"<<DEFAULT_PORT<< '\n'; 
}

void TCPServer::acceptConnection()
{
    if(listen(m_server_socket, 1) != 0){
        ErrorLog("Failed to listen to client \n");
    }

    m_client_socket = accept(m_server_socket, reinterpret_cast<sockaddr*>(&m_clientaddr), &m_len_clientaddr);
    if (m_client_socket == INVALID_SOCKET){
        ErrorLog("Failed to accept client connection");
    }

    std::cout << "Client connected! \n";
}

void TCPServer::closeSocket(){
    if (m_client_socket != INVALID_SOCKET ){
        closesocket(m_client_socket);
    }
    WSACleanup();
}

TCPServer::~TCPServer()
{
    closeSocket();
    freeaddrinfo(m_serverinfo);
}





