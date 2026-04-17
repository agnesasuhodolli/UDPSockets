//Ahmet
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define popen _popen
#define pclose _pclose
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <string>

#define BUFFER_SIZE 2048
#define SERVER_PORT 4444

struct Client {
std::string id;
bool isAdmin;
};

// Funksion për kontroll të komandave
bool isAllowedCommand(const std::string& cmd) {
return (cmd == "dir" ||
cmd == "ls" ||
cmd == "date" ||
cmd == "whoami");
}

int main() {

#ifdef _WIN32
WSADATA wsa;
WSAStartup(MAKEWORD(2,2), &wsa);
#endif

int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

sockaddr_in serverAddr{}, clientAddr{};
socklen_t addrLen = sizeof(clientAddr);

serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(SERVER_PORT);
serverAddr.sin_addr.s_addr = INADDR_ANY;

bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

std::cout << "Server running on port " << SERVER_PORT << "\n";

char buffer[BUFFER_SIZE];
std::vector<Client> clients;

//Agnesa
    while (true) {

        int n = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                         (sockaddr*)&clientAddr, &addrLen);

        if (n <= 0) continue;

        buffer[n] = '\0';
        std::string msg(buffer);

        if (msg == "DISCOVER_SERVER") {
            std::string response = "SERVER_HERE";
            sendto(serverSocket, response.c_str(), response.size(), 0,
                   (sockaddr*)&clientAddr, addrLen);
            continue;
        }

        std::string clientID = std::string(inet_ntoa(clientAddr.sin_addr))
                             + ":" + std::to_string(ntohs(clientAddr.sin_port));

        bool exists = false;
        for (auto &c : clients) {
            if (c.id == clientID) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            Client newClient;
            newClient.id = clientID;
            newClient.isAdmin = clients.empty();
            clients.push_back(newClient);

            std::cout << "New client: " << clientID;
            if (newClient.isAdmin) std::cout << " (ADMIN)";
            std::cout << std::endl;
        }

        Client currentClient;
        for (auto &c : clients) {
            if (c.id == clientID) {
                currentClient = c;
                break;
            }
        }

        std::cout << clientID << " -> " << msg << std::endl;

        std::string response;

//Naila
std::string temp = msg;

    for (auto &c : temp) {
        c = tolower(c);
    }

    if (temp == "exit") {
        continue;
    }

    std::string response = "OK: " + msg;

    sendto(serverSocket, response.c_str(), response.size(), 0,
           (sockaddr*)&clientAddr, addrLen);
}
}