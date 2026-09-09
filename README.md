# LocalDrop

LocalDrop is a C++-based local network file transfer application designed to allow devices connected to the same network to communicate and transfer files directly.

The project is being developed to explore low-level networking concepts in C++, including TCP sockets, client-server communication, HTTP communication, and file transfer.

## Project Status

🚧 **In Development**

The current implementation focuses on establishing TCP communication between a server and a client.

### Current Progress

* [x] Winsock initialization
* [x] TCP server socket creation
* [x] Server socket binding
* [x] Server listening
* [x] Client socket creation
* [x] Client-server connection
* [ ] HTTP request handling
* [ ] File metadata transfer
* [ ] File data transfer
* [ ] File saving
* [ ] Transfer confirmation
* [ ] Device discovery
* [ ] User interface

## Architecture

LocalDrop follows a client-server architecture.

```text
        Local Network
              │
       ┌──────┴──────┐
       │             │
    Server          Client
       │             │
   TCP Socket ─── TCP Socket
       │             │
       └──────┬──────┘
              │
        File Transfer
```

The server waits for incoming connections while the client connects to the server using its local network IP address and port.

## How It Works

The basic communication flow is:

```text
CLIENT
   │
   │ Connect
   ▼
SERVER
   │
   │ Accept connection
   ▼
CLIENT ↔ SERVER
   │
   │ Communication
   ▼
File Transfer
```

For local network testing, both devices should be connected to the same Wi-Fi network or local network.

The client connects to the server using:

```text
SERVER_IP:PORT
```

For example:

```text
192.168.1.10:3086
```

## Technologies

* C++
* Winsock2
* TCP/IP
* HTTP
* Git
* GitLab

## Project Structure

```text
localdrop/
│
├── src/
│   ├── main.cpp
│   ├── tcpserver.h
│   ├── tcpserver.cpp
│   ├── clientmain.cpp
│   ├── tcpclient.h
│   └── tcpclient.cpp
│
├── README.md
└── .gitignore
```

## Building the Server

From the `src` directory:

```powershell
g++ main.cpp tcpserver.cpp -o server.exe -lws2_32
```

Run the server:

```powershell
./server.exe
```

## Building the Client

From the `src` directory:

```powershell
g++ clientmain.cpp tcpclient.cpp -o client.exe -lws2_32
```

Run the client:

```powershell
./client.exe
```

## Testing

For testing across two physical devices:

1. Connect both devices to the same Wi-Fi/network.
2. Start the LocalDrop server on one device.
3. Find the server device's local IPv4 address.
4. Configure the client to connect to the server's IP address.
5. Use the same TCP port on both sides.
6. Start the client on the second device.

Example:

```text
Server:
192.168.1.10:3086

Client:
connects to 192.168.1.10:3086
```

## Future Improvements

The planned development of LocalDrop includes:

* Reliable file transfer
* HTTP request/response handling
* File metadata and size handling
* Multiple client connections
* Device discovery
* Transfer progress tracking
* Error handling
* Improved security
* Graphical user interface

## Learning Objectives

This project is being developed as a practical way to understand:

* Network programming in C++
* TCP sockets
* Client-server architecture
* HTTP communication
* File I/O
* Memory management
* Error handling
* Software architecture
* Build systems and testing

## Author

Christian O.
