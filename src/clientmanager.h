#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>

class ClientManager{
public:
    explicit ClientManager(std::size_t clientCount);
    ~ClientManager();
    void spawn();

private:
    std::vector<std::string> m_availableClients;
    std::vector<std::thread> m_clientThreads;
    std::queue<int> m_pendingRequests;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::mt19937 m_randomEngine;
    std::thread m_managerThread;

    bool m_running = true;
    void releaseClient(const std::string& clientId);
    void processRequests();
    void runClient(std::string clientId);
};