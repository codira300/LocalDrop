#include "clientmanager.h"
#include "client/tcpclient.h"
#include <iostream>
#include <utility>

ClientManager::ClientManager(std::size_t clientCount)
    : m_randomEngine(std::random_device{}()){
    const std::size_t idWidth = std::to_string(clientCount).length();

    for (std::size_t i = 1; i <= clientCount; ++i){
        std::string id = std::to_string(i);

        m_availableClients.push_back(std::string(idWidth - id.length(), '0') + id);
    }

    m_managerThread = std::thread(&ClientManager::processRequests, this);
}

ClientManager::~ClientManager(){
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_running = false;
    }

    m_condition.notify_all();

    if (m_managerThread.joinable()) m_managerThread.join();

    for (std::thread& clientThread : m_clientThreads) if (clientThread.joinable()) clientThread.join();
}


void ClientManager::spawn(){
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_running) return;

        m_pendingRequests.push(1); // This is just to specify to number of requests yet to be processed
    }

    m_condition.notify_one();
}


void ClientManager::processRequests(){
    while (true){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock,
            [this]{
                return !m_running || (!m_pendingRequests.empty() && !m_availableClients.empty());
            }
        );


        if (!m_running) return;

        if (m_pendingRequests.empty() || m_availableClients.empty()) continue;

        m_pendingRequests.pop();

        std::uniform_int_distribution<std::size_t> distribution(0, m_availableClients.size() - 1);
        std::size_t index = distribution(m_randomEngine);

        std::string clientId = std::move(m_availableClients[index]);

        m_availableClients[index] = std::move(m_availableClients.back());
        m_availableClients.pop_back();
        lock.unlock();
        m_clientThreads.emplace_back(&ClientManager::runClient, this, std::move(clientId));
    }
}


void ClientManager::runClient(std::string clientId){
    std::cout<< "Client "<< clientId<< " spawned.\n";
    TCPClient client(clientId);
    client.start();
    releaseClient(clientId);
}

void ClientManager::releaseClient(const std::string& clientId){
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_availableClients.push_back(clientId);
    }

    m_condition.notify_one();
}