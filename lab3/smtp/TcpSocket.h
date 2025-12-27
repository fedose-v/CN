#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class TcpSocket
{
public:
	TcpSocket()
		: m_socketFd(-1)
	{
		m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_socketFd < 0)
		{
			throw std::runtime_error("Failed to create socket");
		}
	}

	~TcpSocket()
	{
		if (m_socketFd >= 0)
		{
			close(m_socketFd);
		}
	}

	void Connect(const std::string& host, int port) const
	{
		struct hostent* server = gethostbyname(host.c_str());
		if (server == nullptr)
		{
			throw std::runtime_error("Failed to resolve hostname");
		}

		struct sockaddr_in serverAddr{};
		std::memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		std::memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
		serverAddr.sin_port = htons(port);

		if (connect(m_socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			throw std::runtime_error("Failed to connect to server");
		}
	}

	void Send(const std::string& data) const
	{
		if (send(m_socketFd, data.c_str(), data.length(), 0) < 0)
		{
			throw std::runtime_error("Failed to send data");
		}
	}

	[[nodiscard]] std::string Receive() const
	{
		char buffer[4096];
		std::memset(buffer, 0, sizeof(buffer));
		size_t bytesRead = recv(m_socketFd, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead == 0)
		{
			throw std::runtime_error("Failed to receive data");
		}
		return std::string(buffer);
	}

private:
	int m_socketFd;
};