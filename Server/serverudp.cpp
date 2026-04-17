//Ahmet


//Naila


//Agnesa
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
#endif

                if (!isAllowedCommand(command)) {
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

                        while (fgets(result, sizeof(result), pipe) != NULL) {
                            output += result;
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