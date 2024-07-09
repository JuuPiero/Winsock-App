#pragma once
#include "pch.h"

class Client {
public:
    enum Command {
        CONNECT = 10,
    };
public:
    ~Client() = default;
    void ShutDown();
    static Client* GetInstance();
    void SendCommand(const std::string& command);
    void Connect();
    inline void SetId(int id) { m_Id = id; }
    inline int GetId() { return m_Id; }
   
private:
    Client();
    void ReceiveMessages();
private:
    static Client* s_Instance;
    WSADATA m_WsaData;
    SOCKET m_ClientSocket = INVALID_SOCKET;
    struct sockaddr_in m_ServerAddr;
    std::thread m_ReceiveThread;
    uint32_t m_ServerPort;
    const char* m_ServerAddress;
    int iResult; // Result of recv()
    int m_Id;
};