# LocalDrop

LocalDrop is a C++-based local network communication project designed to enable devices connected to the same network to communicate directly.

The project was developed as a practical exploration of **network programming, concurrency, cryptographic key exchange, and systems-level software development in C++**.

The current implementation focuses on establishing TCP connections, performing a custom handshake, exchanging cryptographic public keys, managing multiple clients concurrently, exchanging messages, and shutting down the server gracefully.

> **Status: Core networking implementation complete**

---

## Overview

LocalDrop follows a **client-server architecture** over TCP.

The server listens for incoming connections while clients connect to the server using its IP address and port.

The current communication flow is:

```text
Client
   │
   │ TCP Connection
   ▼
Server
   │
   │ LocalDrop Handshake
   ▼
Public-Key Exchange
   │
   ▼
Session-Key Derivation
   │
   ▼
Message Communication
   │
   ▼
Client Disconnect
```

The project is designed for direct device-to-device communication over a local network without requiring an external cloud service.

---

## Current Implementation

### Networking

* [x] Winsock initialization
* [x] TCP server socket creation
* [x] TCP socket binding
* [x] Server listening
* [x] Client socket creation
* [x] Client-server TCP connection
* [x] Client connection acceptance
* [x] Multiple client connections
* [x] Per-client communication
* [x] Client disconnection handling
* [x] Graceful server shutdown

### Handshake & Protocol

* [x] Custom HTTP-style handshake
* [x] HTTP request parsing
* [x] HTTP response generation
* [x] `X-LocalDrop-Key` custom header
* [x] Client public-key transmission
* [x] Server public-key transmission
* [x] Custom message protocol
* [x] Message parsing
* [x] Server acknowledgements

### Cryptography

* [x] libsodium integration
* [x] Client key-pair generation
* [x] Server key-pair generation
* [x] Public-key exchange
* [x] Session-key derivation

> **Note:** The current implementation establishes shared session keys but does not yet provide authenticated peer identity or encrypted application data. These are possible future extensions.

### Concurrency

* [x] Multiple client connections
* [x] Thread-per-client model using `std::thread`
* [x] Client management
* [x] Thread joining and cleanup
* [x] Synchronization with `std::mutex`
* [x] Coordination with `std::condition_variable`
* [x] Server state management with `std::atomic`
* [x] Graceful shutdown

### File Transfer

File transfer is part of the original project goal but is **not yet implemented in the current core version**.

* [ ] File metadata transfer
* [ ] File size handling
* [ ] File data transfer
* [ ] File saving
* [ ] File integrity verification
* [ ] Transfer confirmation
* [ ] Transfer progress tracking

---

## Architecture

LocalDrop uses a TCP client-server architecture with a dedicated thread for each connected client.

```text
                         Local Network
                              │
                              ▼
                     ┌─────────────────┐
                     │      Server     │
                     │                 │
                     │ Listening Socket│
                     └────────┬────────┘
                              │
                    acceptConnection()
                              │
             ┌────────────────┼────────────────┐
             │                │                │
             ▼                ▼                ▼
        Client Socket    Client Socket    Client Socket
             │                │                │
          Thread           Thread           Thread
             │                │                │
        Handshake         Handshake        Handshake
             │                │                │
       Communication    Communication    Communication
```

The listening socket is responsible for accepting new connections.

Each accepted client receives its own socket and is handled independently by a dedicated thread.

This allows multiple clients to communicate with the server concurrently.

---

## Server

The server performs the following operations:

1. Initializes Winsock.
2. Creates the listening TCP socket.
3. Binds the socket to the configured port.
4. Starts listening for connections.
5. Accepts incoming clients.
6. Creates a dedicated thread for each client.
7. Performs the LocalDrop handshake.
8. Exchanges cryptographic public keys.
9. Derives session keys.
10. Receives and parses client messages.
11. Sends responses to clients.
12. Handles client disconnection.
13. Cleans up client resources.
14. Supports graceful server shutdown.

---

## Client

Each client:

1. Initializes Winsock.
2. Resolves the server address.
3. Creates a TCP socket.
4. Connects to the server.
5. Generates its cryptographic key pair.
6. Sends its public key during the handshake.
7. Receives the server's public key.
8. Derives session keys.
9. Sends messages to the server.
10. Receives server responses.
11. Disconnects when requested.

---

## Client Manager

LocalDrop includes a `ClientManager` for managing multiple simulated clients.

The manager is responsible for:

* Maintaining available client IDs.
* Generating client requests.
* Creating client threads.
* Releasing client IDs after a client disconnects.
* Coordinating waiting clients.
* Synchronizing access to shared client state.
* Joining client threads during shutdown.

The client manager uses:

```text
std::thread
std::mutex
std::condition_variable
std::queue
std::vector
std::atomic
```

This part of the project was particularly useful for understanding thread ownership, synchronization, and the lifecycle of concurrent operations.

---

## Cryptographic Key Exchange

LocalDrop uses **libsodium** for its cryptographic operations.

During the handshake, the client and server generate their own public/private key pairs.

Only the public keys are exchanged.

```text
CLIENT                              SERVER

   │                                   │
   │ Generate key pair                 │ Generate key pair
   │                                   │
   │ Client Public Key                 │
   │──────────────────────────────────>│
   │                                   │
   │                                   │ Derive session keys
   │                                   │
   │ Server Public Key                 │
   │<──────────────────────────────────│
   │                                   │
   │ Derive session keys               │
   │                                   │
   └──────── Shared Session Keys ──────┘
```

The public key is transmitted through the custom:

```text
X-LocalDrop-Key: <public-key>
```

header.

Private keys remain local to their respective processes.

> **Security note:** The current implementation demonstrates key exchange and session-key derivation. It should not be considered a production-ready secure file-transfer protocol because peer authentication and encrypted application-data transmission have not yet been implemented.

---

## Communication Protocol

The client initiates the handshake with a request similar to:

```text
GET /handshake HTTP/1.1
Connection: keep-alive
X-LocalDrop-Key: CLIENT_PUBLIC_KEY
```

The server responds with its public key:

```text
HTTP/1.1 200 OK
X-LocalDrop-Key: SERVER_PUBLIC_KEY
```

After the handshake, clients communicate using the LocalDrop message format:

```text
CLIENT_ID:01
MESSAGE:Hello from LocalDrop

```

The blank line marks the end of a message.

The server parses the message and responds with an acknowledgement.

For example:

```text
Client
   │
   │ CLIENT_ID:01
   │ MESSAGE:Hello from LocalDrop
   │
   ▼
Server
   │
   │ Parse message
   │
   ▼
   ACKNOWLEDGED
```

A client can also request disconnection using the `Disconnect` message.

---

## Graceful Shutdown

The server supports graceful shutdown rather than simply terminating the process.

The shutdown flow is:

```text
Server Running
      │
      ▼
Accepting Clients
      │
      ▼
User Requests Shutdown
      │
      ▼
Stop Accepting New Clients
      │
      ▼
Clean Up Client Threads
      │
      ▼
Close Server Socket
      │
      ▼
WSACleanup()
      │
      ▼
Server Shutdown
```

This provides practical experience with thread ownership, `joinable()`, `join()`, socket cleanup, and controlled application termination.

---

## Local Network Usage

LocalDrop is designed for devices connected to the same local network.

Both devices should be connected to the same:

* Wi-Fi network, or
* Ethernet/local network.

The server currently uses TCP port:

```text
8080
```

For example:

```text
Server:
192.168.1.10:8080

Client:
connects to 192.168.1.10:8080
```

For testing on the same computer, the client can connect to:

```text
127.0.0.1:8080
```

When testing between two devices, the client must use the server device's actual local IPv4 address.

Windows Firewall may need an inbound rule allowing the LocalDrop server to accept connections.

---

## Technologies

* **C++20**
* **Winsock2**
* **TCP/IP**
* **HTTP-style protocol**
* **libsodium**
* **C++ Standard Library**
* **Git**
* **GitLab**
* **MSYS2**
* **MinGW-w64 / GCC**
* **PowerShell**

---

## Dependencies

### Compiler

LocalDrop is currently developed using:

```text
GCC / MinGW-w64
```

### Windows Networking

The project uses:

```text
Winsock2
```

and links against:

```text
-lws2_32
```

### Cryptography

The project uses:

```text
libsodium
```

and links against:

```text
-lsodium
```

---

## Project Structure

The project is organized into separate server, client, cryptography, error-handling, and messaging components.

```text
localdrop/
│
├── src/
│   │
│   ├── server/
│   │   ├── servermain.cpp
│   │   ├── tcpserver.h
│   │   └── tcpserver.cpp
│   │
│   ├── client/
│   │   ├── clientmain.cpp
│   │   ├── clientmanager.h
│   │   ├── clientmanager.cpp
│   │   ├── tcpclient.h
│   │   └── tcpclient.cpp
│   │
│   ├── crypto/
│   │   ├── crypto.h
│   │   └── crypto.cpp
│   │
│   ├── error/
│   │   ├── error.h
│   │   └── error.cpp
│   │
│   ├── message.h
│   ├── message.cpp
│   ├── messagegenerator.h
│   └── messagegenerator.cpp
│
├── README.md
├── .gitignore
└── .gitlab-ci.yml
```

---

## Building

LocalDrop currently uses direct `g++` compilation.

CMake has **not yet been introduced** into the project. It can be added later as a build-system improvement.

### Server

From the server source directory, compile the server together with its dependencies:

```powershell
g++ servermain.cpp tcpserver.cpp ../error/error.cpp ../crypto/crypto.cpp ../messagegenerator.cpp ../message.cpp -o server.exe -lws2_32 -lsodium
```

Run:

```powershell
./server.exe
```

### Client

Compile the client and its supporting components:

```powershell
g++ clientmain.cpp tcpclient.cpp ../error/error.cpp ../crypto/crypto.cpp ../messagegenerator.cpp ../message.cpp ../clientmanager.cpp -o client.exe -lws2_32 -lsodium
```

Run:

```powershell
./client.exe
```

---

## Testing

### Same-device testing

The easiest way to test LocalDrop is to run the server and client on the same computer.

```text
Server
   │
   │ 127.0.0.1:8080
   │
   ▼
Client
```

This allows the networking and protocol implementation to be tested without requiring two physical devices.

### Multiple-client testing

The client manager can create multiple client instances.

Each client receives a unique identifier:

```text
01
02
03
04
...
```

The clients connect to the server concurrently and communicate independently.

Example:

```text
Server
  │
  ├── Client 01 → Thread 01
  ├── Client 02 → Thread 02
  ├── Client 03 → Thread 03
  └── Client 04 → Thread 04
```

### Two-device testing

To test over a real local network:

1. Connect both devices to the same network.
2. Start the LocalDrop server on one device.
3. Find the server's local IPv4 address.
4. Configure the client to use that address.
5. Make sure both applications use port `8080`.
6. Start the client.
7. Allow the application through Windows Firewall if required.

Example:

```text
Server device:

192.168.1.10
Port: 8080

Client:

192.168.1.10:8080
```

---

## Future Extensions

The core networking project is complete enough to serve as a foundation for future work.

Possible extensions include:

### File Transfer

* [ ] File metadata protocol
* [ ] File size handling
* [ ] File streaming
* [ ] File storage
* [ ] File integrity verification
* [ ] Transfer confirmation
* [ ] Transfer progress tracking

### Security

* [ ] Peer authentication
* [ ] Encrypted application data
* [ ] Message authentication
* [ ] Secure session management

### Networking

* [ ] Device discovery
* [ ] Improved protocol framing
* [ ] More robust error recovery
* [ ] Connection timeouts

### Software Engineering

* [ ] CMake build system
* [ ] Automated unit tests
* [ ] Integration tests
* [ ] Improved CI/CD pipeline
* [ ] Cross-platform support

### User Experience

* [ ] Command-line configuration
* [ ] Graphical user interface
* [ ] Transfer progress display
* [ ] Improved configuration management

---

## Learning Objectives

LocalDrop was developed as a practical project for understanding:

* C++ network programming
* TCP/IP
* Winsock
* Client-server architecture
* Socket programming
* HTTP protocol fundamentals
* Cryptographic key exchange
* libsodium
* Message protocols
* File I/O concepts
* Memory management
* Error handling
* C++ resource management
* `std::thread`
* Thread lifecycle
* `std::mutex`
* `std::condition_variable`
* `std::atomic`
* Concurrent client management
* Software architecture
* Build systems
* Testing
* CI/CD

---

## What This Project Demonstrates

LocalDrop provided practical experience with the lifecycle of a networked C++ application:

```text
Socket Creation
      ↓
Binding
      ↓
Listening
      ↓
Connection Acceptance
      ↓
Client Thread Creation
      ↓
Handshake
      ↓
Key Exchange
      ↓
Message Communication
      ↓
Client Disconnect
      ↓
Resource Cleanup
      ↓
Graceful Shutdown
```

The project also provided experience moving from a single-client networking program toward a concurrent server capable of managing multiple clients.

---

## Author

**Christian O.**

LocalDrop was developed as a hands-on systems and networking project to deepen practical understanding of C++, TCP networking, concurrency, cryptographic concepts, and software engineering.
