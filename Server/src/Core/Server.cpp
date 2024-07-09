#include "Core/Server.h"
#include "spdlog/spdlog.h"
#include "pch.h"
Server* Server::s_Insntance = nullptr;

Server::Server(): m_Port(5500) {
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &m_WsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        ShutDown();
        return;
    }
    // Create a socket for listening
    m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_ListenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        ShutDown();
        return;
    }

    // Setup server address
    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = INADDR_ANY; // cái này lắng nghe mọi máy từ mạng cục bộ
    m_ServerAddr.sin_port = htons(m_Port);

    // Bind socket
    iResult = bind(m_ListenSocket, (struct sockaddr*)&m_ServerAddr, sizeof(m_ServerAddr));
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        ShutDown();
        return;
    }
    // Listen on socket
    iResult = listen(m_ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(m_ListenSocket);
        WSACleanup();
        return;
    }
    spdlog::info("Server listening on port: {}", m_Port);
}

Server* Server::GetInstance() {
    if (s_Insntance == nullptr) {
        s_Insntance = new Server();
    }
    return s_Insntance;
}


void Server::Run() {
    // Accept a client socket
    SOCKET clientSocket;
    clientSocket = accept(m_ListenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(m_ListenSocket);
        WSACleanup();
        return;
    }
    
    uint32_t clientSocketId = GetId();
    m_ClientSockets[clientSocketId] = clientSocket;
    
    // Create a thread to handle client
   
    std::thread handlerThread(&Server::ClientHandler, this, clientSocketId);
    handlerThread.detach(); // Detach thread to run independently
}

void Server::ClientHandler(uint32_t clientSocketId) {
    char recvbuf[DEFAULT_BUFLEN];
    SOCKET clientSocket = m_ClientSockets[clientSocketId];
    
    spdlog::info("A client connected to server: {}",  clientSocketId);
   
    do {
        iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);

        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            //parse data from client
            json requestData = json::parse(std::string(recvbuf));
            std::cout << "message from client: " << std::endl;
            std::cout << requestData.dump(4) << std::endl;
            int command = requestData["command"];

            if(command == Command::CONNECT) {
                OnClientConnect(clientSocketId);
            }
            else {
                //
            }
            
            
        } else if (iResult == 0) {
            std::cout << "Connection closed by client" << std::endl;
        } else {
            spdlog::error("A client disconnected : {}", WSAGetLastError());
        }
    } while (iResult > 0);

    closesocket(clientSocket);

    m_ClientSockets.erase(clientSocketId);
}
void Server::ShutDown() {
    closesocket(m_ListenSocket);
    WSACleanup();
    delete s_Insntance;
    s_Insntance = nullptr;
}