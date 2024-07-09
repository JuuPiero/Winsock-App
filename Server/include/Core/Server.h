#pragma once


#include "pch.h"

class Server {
public:
    enum Command {
        CONNECT = 10,
    };
public:
    static Server* GetInstance();
    void ShutDown();
    void Run();
    ~Server() = default;


    void OnClientConnect(uint32_t clientId);
    void OnClientSearch(uint32_t clientId, std::string keywords);
private:
    void ClientHandler(uint32_t clientSocketId);
private:
    Server();
    static Server* s_Insntance;
    WSADATA m_WsaData;
    SOCKET m_ListenSocket;
    std::unordered_map<uint32_t, SOCKET> m_ClientSockets;
    sockaddr_in m_ServerAddr;
    uint32_t m_Port;
    int iResult;
};
