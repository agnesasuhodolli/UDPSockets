//Naila
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFER_SIZE 2048

int main() {

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4444);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.29"); //Qetu e shton IP adresen tande

    char buffer[BUFFER_SIZE];

    int timeout = 3000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
               (const char*)&timeout, sizeof(timeout));

    std::cout << "Type 'exit' anytime to leave the server.\n";

//Ahmet
std::cout << "================ COMMANDS ================\n";

std::cout << "\nFor ALL clients:\n";
std::cout << " - list (list files)\n";
std::cout << " - read (read file)\n";
std::cout << " - exit (disconnect)\n";

std::cout << "\nADMIN only:\n";
std::cout << " - write <text> (write to file)\n";
std::cout << " - execute <cmd> (run system command)\n";

std::cout << "\nAllowed execute commands:\n";
std::cout << " - dir / ls\n";
std::cout << " - date\n";
std::cout << " - whoami\n";

std::cout << "==========================================\n";

//Agnesa
    while (true) {

        std::cout << "\nCommand: ";
        std::cin.getline(buffer, BUFFER_SIZE);

        std::string input(buffer);

        if (input == "exit") {
            std::cout << "Disconnecting...\n";
            break;
        }

        sendto(clientSocket, buffer, strlen(buffer), 0,
               (sockaddr*)&serverAddr, sizeof(serverAddr));

        int n = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, nullptr, nullptr);

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