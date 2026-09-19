#include "../clientmanager.h"

#include <chrono>
#include <random>
#include <thread>

int main(){
    ClientManager clientManager(10);

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    std::uniform_int_distribution<int> delay(1, 3);

    for (int i = 0; i < 50; ++i){
        std::this_thread::sleep_for(std::chrono::seconds(delay(generator)));

        clientManager.spawn();
    }
    return 0;
}