#pragma once
#include "../utils/TimeUtils.h"
#include "UdpSocket.h"
#include <iomanip>
#include <iostream>

class PingClient
{
private:
	std::string m_serverIp;
	int m_serverPort;
	int m_pingCount;
	UdpSocket m_udpSocket;

public:
	PingClient(const std::string& ip, int port)
		: m_serverIp(ip)
		, m_serverPort(port)
		, m_pingCount(10)
	{
	}

	void Run()
	{
		try
		{
			m_udpSocket.SetSocketTimeout(1);

			for (int i = 1; i <= m_pingCount; ++i)
			{
				long long timestamp = TimeUtils::GetCurrentTimeMillis();
				std::string message = "Ping " + std::to_string(i) + " " + std::to_string(timestamp);

				double startTime = TimeUtils::GetTimeInSeconds();

				m_udpSocket.SendTo(message, m_serverIp, m_serverPort);

				std::string response = m_udpSocket.ReceiveFrom(1024);

				double endTime = TimeUtils::GetTimeInSeconds();

				if (response.empty())
				{
					std::cout << "Request timed out" << std::endl;
				}
				else
				{
					double rtt = endTime - startTime;
					std::cout << "Ответ от сервера: " << response
							  << ", RTT = " << std::fixed << std::setprecision(3) << rtt << " сек"
							  << std::endl;
				}
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
};