//Naila


//Ahmet


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