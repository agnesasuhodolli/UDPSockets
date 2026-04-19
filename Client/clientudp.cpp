#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFER_SIZE 2048
#define SERVER_PORT 4444

int main() {

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (clientSocket < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    char buffer[BUFFER_SIZE];
    char ip[50];

    std::cout << "Enter server IP: ";
    std::cin.getline(ip, 50);

    sockaddr_in serverAddr{};
    socklen_t serverLen = sizeof(serverAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    std::cout << "Connected to server: " << ip << "\n";

    std::cout << "================ COMMANDS ================\n";

    std::cout << "\nFor ALL clients:\n";
    std::cout << " - list (list files)\n";
    std::cout << " - read (read file)\n";
    std::cout << " - exit (disconnect)\n";

    std::cout << "\nADMIN only:\n";
    std::cout << " - write <text> (write to file)\n";
    std::cout << " - execute <cmd> (run system command)\n";

    std::cout << "\nAllowed execute commands:\n";
    std::cout << " - dir (Windows)\n";
    std::cout << " - ls (Linux)\n";
    std::cout << " - date\n";
    std::cout << " - whoami\n";

    std::cout << "==========================================\n";

    while (true) {

        std::cout << "\nCommand: ";
        std::cin.getline(buffer, BUFFER_SIZE);

        std::string input(buffer);

        if (input == "exit") {
        sendto(clientSocket, buffer, strlen(buffer), 0,
           (sockaddr*)&serverAddr, sizeof(serverAddr));
            std::cout << "Disconnecting...\n";
            break;
        }

        sendto(clientSocket, buffer, strlen(buffer), 0,
               (sockaddr*)&serverAddr, sizeof(serverAddr));

        int n = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0,
                         (sockaddr*)&serverAddr, &serverLen);

        if (n < 0) {
            std::cout << "No response from server\n";
            continue;
        }

        buffer[n] = '\0';

        std::cout << "Server: " << buffer << std::endl;
    }

#ifdef _WIN32
    closesocket(clientSocket);
    WSACleanup();
#else
    close(clientSocket);
#endif

    return 0;
}