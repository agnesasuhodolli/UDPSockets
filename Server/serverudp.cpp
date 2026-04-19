#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <chrono>

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

bool isAllowedCommand(const std::string& cmd) {
    return (cmd == "dir" || cmd == "ls" || cmd == "date" || cmd == "whoami");
}

int main() {

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (serverSocket < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

    sockaddr_in serverAddr{}, clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    std::cout << "Server running on port " << SERVER_PORT << "\n";

    char buffer[BUFFER_SIZE];
    std::vector<Client> clients;

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

        if (msg == "list") {
            response = "Files: test.txt";
        }

        else if (msg == "read") {
            std::ifstream file("test.txt");

            if (!file) {
                response = "File not found";
            } else {
                std::string content((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
                response = content;
            }
        }

        else if (msg.find("write ") == 0) {

            if (!currentClient.isAdmin) {
                response = "Permission denied (WRITE only for admin)";
            } else {
                std::ofstream file("test.txt", std::ios::app);
                file << msg.substr(6) << "\n";
                response = "Written to file";
            }
        }

        else if (msg.find("execute ") == 0) {

            if (!currentClient.isAdmin) {
                response = "Permission denied (EXECUTE only for admin)";
            } else {

                std::string command = msg.substr(8);

#ifdef _WIN32
                if (command == "ls") command = "dir";
                if (command == "date") command = "echo %DATE%";
#else
                if (command == "date") command = "date";
#endif

                if (!isAllowedCommand(msg.substr(8))) {
                    response = "Command not allowed!";
                } else {

#ifdef _WIN32
                    std::string fullCommand = "cmd /c " + command;
#else
                    std::string fullCommand = command;
#endif

                    FILE* pipe = popen(fullCommand.c_str(), "r");

                    if (!pipe) {
                        response = "Execution failed";
                    } else {
                        char result[256];
                        std::string output = "";

                        auto start = std::chrono::steady_clock::now();
                        const int TIMEOUT_SECONDS = 3;

                        while (fgets(result, sizeof(result), pipe) != NULL) {

                            output += result;

                            auto now = std::chrono::steady_clock::now();
                            int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

                            if (elapsed > TIMEOUT_SECONDS) {
                                output += "\n[ERROR: Command timeout]";
                                break;
                            }
                        }

                        pclose(pipe);

                        response = output.empty() ? "Executed (no output)" : output;
                    }
                }
            }
        }

        else {
            response = "Unknown command";
        }

        sendto(serverSocket, response.c_str(), response.size(), 0,
               (sockaddr*)&clientAddr, addrLen);
    }

#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif

    return 0;
}