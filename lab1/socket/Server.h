#pragma once

#include "SocketProvider.h"

class Server
{
public:
	Server(const std::string& serverName, int serverNumber, int port);
	void Start();

private:
	std::string m_serverName;
	int m_serverNumber;
	int m_port;
	int m_listenSocketDescriptor;
	SocketProvider m_socketProvider;

	bool Initialize();
	bool ProcessClient(int clientSocketDescriptor);
	void LogInteraction(const MessageData& clientData) const;
};