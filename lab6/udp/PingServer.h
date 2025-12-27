#pragma once
#include <iostream>

#include "../utils/RandomGenerator.h"
#include "UdpServerSocket.h"

class PingServer
{
private:
	int m_port;
	UdpServerSocket m_serverSocket;
	RandomGenerator m_rng;

public:
	explicit PingServer(int port)
		: m_port(port)
		, m_serverSocket(port)
	{
	}

	void Run()
	{
		std::cout << "Server started on port " << m_port << std::endl;
		std::cout << "Press Ctrl+C to stop..." << std::endl;

		while (true)
		{
			try
			{
				SocketAddress clientAddr;
				std::string message = m_serverSocket.ReceiveFrom(clientAddr, 1024);

				if (message.empty())
				{
					continue;
				}

				if (m_rng.ShouldDropPacket())
				{
					std::cout << "Packet dropped: " << message << std::endl;
					continue;
				}

				std::cout << "Received and replying: " << message << std::endl;
				m_serverSocket.SendTo(message, clientAddr);
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error handling packet: " << e.what() << std::endl;
			}
		}
	}
};