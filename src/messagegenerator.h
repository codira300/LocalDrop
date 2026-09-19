#pragma once

#include <string>
#include <vector>

class MessageGenerator{
public:
    MessageGenerator();
    std::string getRandomMessage();

private:
    std::vector<std::string> m_messages;
};