#pragma once

#include "MessageData.h"
#include <sys/socket.h>
#include <string>

class SocketProvider {
public:
    SocketProvider();
    ~SocketProvider();

    int CreateSocket();
    void CloseSocket(int socketDescriptor);

    bool SendData(int socketDescriptor, const std::string& data);
    std::string ReceiveData(int socketDescriptor);

    static MessageData ParseMessage(const std::string& rawMessage);
    static std::string CreateMessage(const std::string& name, int number);
};