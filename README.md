# UDPSockets
This project is developed as part of the Computer Networks course and focuses on implementing a client-server architecture using C++.

The system consists of a server and multiple clients that communicate over network sockets using IP addresses and ports. The server is responsible for listening to incoming client connections, processing requests, and managing access to files and resources. Each client connects to the server, sends requests, receives responses, and interacts with the server based on assigned permissions.

Key functionalities of the project include:

* Establishing socket-based communication between server and multiple clients
* Handling concurrent client connections
* Processing client requests and sending appropriate responses
* Implementing permission control (read, write, execute) for different clients
* Enabling file access and management on the server side
* Ensuring reliable communication using proper connection handling

The project demonstrates core networking concepts such as socket programming, client-server communication, concurrency, and access control mechanisms in C++.

This implementation provides a practical understanding of how networked systems operate and how multiple clients can securely interact with a centralized server.
