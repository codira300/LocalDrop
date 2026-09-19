#pragma once

#include <memory>
#include <string>

struct Message {
    std::string clientId;
    std::string message;
};

std::unique_ptr<Message> parseMessage(const std::string& rawMessage);