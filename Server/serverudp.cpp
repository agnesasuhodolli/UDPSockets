//Ahmet


//Agnesa
    while (true) {

        int n = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                         (sockaddr*)&clientAddr, &addrLen);

        if (n <= 0) continue;

        buffer[n] = '\0';
        std::string msg(buffer);

        std::string clientID = std::string(inet_ntoa(clientAddr.sin_addr))
                             + ":" + std::to_string(ntohs(clientAddr.sin_port));

        // Regjistrimi i klientëve
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

        // Gjej klientin aktual
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