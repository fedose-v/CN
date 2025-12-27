#pragma once
#include "ProxyHandler.h"
#include <thread>

class ProxyServer
{
public:
	explicit ProxyServer(int port)
		: m_port(port)
		, m_serverSocket(-1)
	{
	}

	void Start()
	{
		signal(SIGPIPE, SIG_IGN);

		m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_serverSocket < 0)
		{
			StatisticsManager::LogError("Failed to create socket");
			return;
		}

		int opt = 1;
		setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		struct sockaddr_in address{};
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(m_port);

		if (bind(m_serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0)
		{
			StatisticsManager::LogError("Bind failed");
			return;
		}

		if (listen(m_serverSocket, 10) < 0)
		{
			StatisticsManager::LogError("Listen failed");
			return;
		}

		std::cout << "Proxy Server listening on port " << m_port << std::endl;

		while (true)
		{
			struct sockaddr_in clientAddr{};
			socklen_t clientLen = sizeof(clientAddr);
			int clientSocket = accept(m_serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

			if (clientSocket >= 0)
			{
				std::thread clientThread(&ProxyServer::Dispatch, this, clientSocket);
				clientThread.detach();
			}
		}
	}

private:
	int m_port;
	int m_serverSocket;

	void Dispatch(int clientSocket)
	{
		ProxyHandler handler(clientSocket);
		handler.Handle();
	}
};