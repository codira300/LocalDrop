#include <iostream>
#include "message.h"

std::unique_ptr<Message> parseMessage(const std::string& rawMessage){
    std::size_t clientIdStart = rawMessage.find("CLIENT_ID:");
    clientIdStart += std::string("CLIENT_ID:").size();
    std::size_t clientIdEnd = rawMessage.find("\n", clientIdStart);
    std::string clientId = rawMessage.substr(clientIdStart, clientIdEnd - clientIdStart);

    std::size_t messageStart = rawMessage.find("MESSAGE:");
    messageStart += std::string("MESSAGE:").size();
    std::size_t messageEnd = rawMessage.find("\n", messageStart);
    std::string message = rawMessage.substr(messageStart, messageEnd - messageStart);
    return std::make_unique<Message>(clientId, message);
}