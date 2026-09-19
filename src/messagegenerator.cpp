#include "messagegenerator.h"
#include <random>
#include <string>

MessageGenerator::MessageGenerator(){
    m_messages = {
        "Hello from client",
        "How are you?",
        "Testing LocalDrop",
        "This is a random message",
        "C++ networking is interesting",
        "Message received from client",
        "LocalDrop is running",
        "Testing the message server",
        "Another message from client",
        "Sending data to the server",
        "Client is still connected",
        "Random message number one",
        "Random message number two",
        "Random message number three",
        "Checking server communication",
        "TCP connection is active",
        "Sending another message",
        "The client is working",
        "Testing concurrent communication",
        "Message processing test",
        "Hello server",
        "Client sending data",
        "Testing message queue",
        "FIFO processing test",
        "Another random message",
        "Server communication test",
        "LocalDrop client test",
        "Sending a new message",
        "The connection is still active",
        "Testing network communication",
        "Message from LocalDrop",
        "Client message",
        "Random text from client",
        "Testing the TCP connection",
        "Sending data continuously",
        "Another client message",
        "Testing server response",
        "Message queue testing",
        "Checking connection",
        "LocalDrop networking test",
        "Client is sending",
        "Testing message handling",
        "Random client communication",
        "Testing multiple messages",
        "Server should receive this",
        "Another test message",
        "Continuous message test",
        "Client is communicating",
        "Testing message ordering",
        "Final normal message",
        "Disconnect"
    };
}



std::string MessageGenerator::getRandomMessage(){
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());

    std::uniform_int_distribution<std::size_t> distribution(0, m_messages.size() - 1);
    return m_messages[distribution(generator)];
}