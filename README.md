# LocalDrop

LocalDrop is a C++-based local network file transfer application designed to enable devices connected to the same network to communicate and transfer files directly.

The project is being developed as a practical exploration of **network programming and systems-level software development in C++**, with a focus on TCP sockets, client-server architecture, HTTP-based communication, cryptographic key exchange, file I/O, and concurrent connections.

> 🚧 **Status: In Development**

---

## Overview

LocalDrop follows a client-server architecture.

A device running the LocalDrop server listens for incoming TCP connections, while a client connects to the server using its local network IP address and port.

The intended workflow is:

```text
Client
   │
   │ TCP Connection
   ▼
Server
   │
   │ HTTP Handshake
   ▼
Key Exchange
   │
   │ Session Keys
   ▼
Secure Communication
   │
   │ File Metadata
   ▼
File Transfer
   │
   ▼
File Storage
```

The application is intended for direct device-to-device transfers over a local network without requiring an external cloud service.

---

## Current Project Status

### Networking

* [x] Winsock initialization
* [x] TCP server socket creation
* [x] TCP socket binding
* [x] Server listening
* [x] Client socket creation
* [x] Client-server TCP connection
* [x] Client connection acceptance
* [x] Basic client-server communication

### HTTP Communication

* [x] HTTP-style handshake
* [x] HTTP request parsing
* [x] HTTP response generation
* [x] Custom `X-LocalDrop-Key` header
* [ ] Complete HTTP request handling
* [ ] File upload/download endpoints

### Cryptography

* [x] libsodium integration
* [x] Client key-pair generation
* [x] Server key-pair generation
* [x] Public-key exchange
* [x] Client session-key derivation
* [x] Server session-key derivation
* [ ] Authenticated key exchange
* [ ] Encrypted data transfer
* [ ] Message authentication

### File Transfer

* [ ] File metadata transfer
* [ ] File size handling
* [ ] File data transfer
* [ ] File saving
* [ ] File integrity verification
* [ ] Transfer confirmation
* [ ] Transfer progress tracking

### Concurrency

* [ ] Multiple client connections
* [ ] Client handling with `std::thread`
* [ ] Thread synchronization
* [ ] Concurrent file transfers

### Application

* [ ] Device discovery
* [ ] User interface
* [ ] Improved error handling
* [ ] Configuration management

---

## Architecture

LocalDrop uses a client-server architecture over TCP.

```text
                    Local Network
                         │
              ┌──────────┴──────────┐
              │                     │
           Server                 Client
              │                     │
        TCP Socket             TCP Socket
              │                     │
              └──────────┬──────────┘
                         │
                  HTTP Handshake
                         │
                    Key Exchange
                         │
                  Session Keys
                         │
                  Secure Channel
                         │
                    File Transfer
```

### Server

The server:

1. Initializes Winsock.
2. Creates a TCP socket.
3. Binds the socket to a local port.
4. Starts listening for connections.
5. Accepts incoming clients.
6. Performs the LocalDrop handshake.
7. Exchanges cryptographic public keys.
8. Establishes session keys.
9. Communicates with the client.
10. Handles file transfers.

### Client

The client:

1. Initializes Winsock.
2. Resolves the server address.
3. Creates a TCP socket.
4. Connects to the server.
5. Generates its cryptographic key pair.
6. Sends its public key during the handshake.
7. Receives the server's public key.
8. Derives the client session keys.
9. Communicates with the server.
10. Transfers files.

---

## Cryptographic Key Exchange

LocalDrop uses **libsodium** for its cryptographic operations.

During the handshake, both the client and server generate their own public/private key pairs.

Only the public keys are transmitted.

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

The public keys are exchanged using the custom HTTP header:

```text
X-LocalDrop-Key: <public-key>
```

The private keys remain on their respective devices.

> **Note:** The current key exchange establishes session keys but does not yet provide peer authentication. Authenticated key exchange and encrypted data transmission are planned next.

---

## Communication Flow

The current handshake follows this general flow:

```text
CLIENT
   │
   │ GET /handshake
   │ X-LocalDrop-Key: CLIENT_PUBLIC_KEY
   │
   ▼
SERVER
   │
   │ Extract client public key
   │
   │ Derive server session keys
   │
   │ Generate server response
   │
   │ X-LocalDrop-Key: SERVER_PUBLIC_KEY
   │
   ▼
CLIENT
   │
   │ Extract server public key
   │
   │ Derive client session keys
   │
   ▼
Secure Communication
```

---

## Local Network Usage

LocalDrop is designed for devices connected to the same local network.

Both devices should be connected to the same:

* Wi-Fi network, or
* Ethernet/local network.

The client connects using the server's local IPv4 address and TCP port.

Example:

```text
Server:
192.168.1.10:3086

Client:
connects to 192.168.1.10:3086
```

The server should listen on the configured port, while the client uses the server device's actual LAN IP address.

---

## Technologies

* **C++20**
* **Winsock2**
* **TCP/IP**
* **HTTP**
* **libsodium**
* **Git**
* **GitLab**
* **PowerShell**
* **MSYS2 / MinGW-w64**

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

LocalDrop uses:

```text
libsodium
```

and links against:

```text
-lsodium
```

---

## Project Structure

```text
localdrop/
│
├── src/
│   ├── servermain.cpp
│   ├── tcpserver.h
│   ├── tcpserver.cpp
│   │
│   ├── clientmain.cpp
│   ├── tcpclient.h
│   ├── tcpclient.cpp
│   │
│   ├── crypto.h
│   ├── crypto.cpp
│   │
│   ├── error.h
│   └── error.cpp
│
├── README.md
├── .gitignore
└── .gitlab-ci.yml
```

---

## Building the Server

From the `src` directory:

```powershell
g++ servermain.cpp tcpserver.cpp crypto.cpp error.cpp -o server.exe -lws2_32 -lsodium
```

Run the server:

```powershell
./server.exe
```

---

## Building the Client

From the `src` directory:

```powershell
g++ clientmain.cpp tcpclient.cpp crypto.cpp error.cpp -o client.exe -lws2_32 -lsodium
```

Run the client:

```powershell
./client.exe
```

---

## Testing

### Same-device testing

The client and server can initially be tested on the same computer.

```text
Server
   │
   │ 127.0.0.1:PORT
   │
   ▼
Client
```

### Two-device testing

1. Connect both devices to the same local network.
2. Start the LocalDrop server on one device.
3. Find the server's local IPv4 address.
4. Configure the client with the server's IP address.
5. Make sure both applications use the same TCP port.
6. Start the client.

Example:

```text
Server device:
192.168.1.10

Port:
3086

Client:
192.168.1.10:3086
```

If Windows Firewall blocks the connection, an inbound firewall rule may need to be created for the LocalDrop server.

---

## Development Roadmap

### Phase 1 — Networking

* [x] TCP server
* [x] TCP client
* [x] Socket connection
* [x] Connection acceptance
* [x] Basic communication

### Phase 2 — Protocol

* [x] HTTP-style handshake
* [x] Request parsing
* [x] Response handling
* [x] Custom LocalDrop headers

### Phase 3 — Cryptography

* [x] libsodium integration
* [x] Key-pair generation
* [x] Public-key exchange
* [x] Session-key derivation
* [ ] Authentication
* [ ] Encryption
* [ ] Message integrity

### Phase 4 — File Transfer

* [ ] File metadata protocol
* [ ] File size handling
* [ ] File streaming
* [ ] File storage
* [ ] File integrity verification
* [ ] Transfer confirmation

### Phase 5 — Concurrency

* [ ] Multiple clients
* [ ] Thread-per-client model
* [ ] Thread synchronization
* [ ] Concurrent transfers

### Phase 6 — User Experience

* [ ] Device discovery
* [ ] Transfer progress
* [ ] User interface
* [ ] Improved configuration
* [ ] Production-ready error handling

---

## Learning Objectives

LocalDrop is being developed as a practical project for understanding:

* C++ network programming
* TCP/IP
* Winsock
* Client-server architecture
* HTTP protocol fundamentals
* Socket programming
* Cryptographic key exchange
* Secure communication
* File I/O
* Memory management
* Error handling
* Concurrency
* Thread synchronization
* Software architecture
* Build systems
* Testing
* CI/CD

---

## Author

**Christian O.**

LocalDrop is developed as a hands-on systems and networking project to deepen practical understanding of C++ and software engineering.
