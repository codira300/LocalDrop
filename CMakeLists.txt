cmake_minimum_required(VERSION 3.20)

project(LocalDrop)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/server/servermain.cpp
    src/server/tcpserver.cpp

    src/crypto/crypto.cpp
    src/error/error.cpp
    src/message.cpp
    src/messagegenerator.cpp
)

add_executable(client
    src/client/clientmain.cpp
    src/clientmanager.cpp
    src/client/tcpclient.cpp

    src/crypto/crypto.cpp
    src/error/error.cpp
    src/message.cpp
    src/messagegenerator.cpp
)

target_link_libraries(server
    ws2_32
    sodium
)

target_link_libraries(client
    ws2_32
    sodium
)