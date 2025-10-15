#pragma once

#include "SocketProvider.h"

class Client {
public:
	Client(const std::string& serverIp, int port, const std::string& clientName);
	void Start();

private:
	std::string m_serverIp;
	int m_port;
	std::string m_clientName;
	int m_socketDescriptor;
	SocketProvider m_socketProvider;

	int GetClientNumber() const;
	bool EstablishConnection();
	void DisplayResult(int clientNumber, const MessageData& serverData) const;
};