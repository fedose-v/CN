#pragma once
#include "SocketAddress.h"
#include <stdexcept>
#include <unistd.h>
#include <vector>

class UdpServerSocket
{
private:
	int m_socketFd;

public:
	explicit UdpServerSocket(int port)
	{
		m_socketFd = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socketFd < 0)
		{
			throw std::runtime_error("Socket creation failed");
		}

		struct sockaddr_in serverAddr{};
		std::memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(port);

		if (bind(m_socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			Close();
			throw std::runtime_error("Bind failed");
		}
	}

	~UdpServerSocket()
	{
		Close();
	}

	std::string ReceiveFrom(SocketAddress& clientAddr, int bufferSize) const
	{
		std::vector<char> buffer(bufferSize);
		ssize_t recvBytes = recvfrom(m_socketFd, buffer.data(), bufferSize, 0,
			clientAddr.GetSockAddr(), clientAddr.GetAddrLenPtr());

		if (recvBytes < 0)
		{
			return "";
		}

		return std::string(buffer.data(), recvBytes);
	}

	void SendTo(const std::string& message, SocketAddress& clientAddr) const
	{
		ssize_t sentBytes = sendto(m_socketFd, message.c_str(), message.length(), 0,
			clientAddr.GetSockAddr(), clientAddr.GetAddrLen());

		if (sentBytes < 0)
		{
			throw std::runtime_error("Send failed");
		}
	}

	void Close()
	{
		if (m_socketFd >= 0)
		{
			close(m_socketFd);
			m_socketFd = -1;
		}
	}
};