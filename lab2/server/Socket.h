#pragma once
#include <memory>
#include <netinet/in.h>

class Socket
{
private:
	int m_socketFd;

public:
	Socket()
		: m_socketFd(-1)
	{
		m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_socketFd < 0)
		{
			throw std::runtime_error("Cannot create socket");
		}
	}

	~Socket()
	{
		if (m_socketFd >= 0)
		{
			close(m_socketFd);
		}
	}

	void Bind(int port) const
	{
		struct sockaddr_in serverAddr{};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(port);

		if (::bind(m_socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			throw std::runtime_error("Cannot bind socket");
		}
	}

	void Listen(int backlog = 10) const
	{
		if (::listen(m_socketFd, backlog) < 0)
		{
			throw std::runtime_error("Cannot listen on socket");
		}
	}

	[[nodiscard]] int Accept() const
	{
		struct sockaddr_in clientAddr{};
		socklen_t clientLen = sizeof(clientAddr);
		int clientSocket = ::accept(m_socketFd, (struct sockaddr*)&clientAddr, &clientLen);
		if (clientSocket < 0)
		{
			throw std::runtime_error("Cannot accept connection");
		}
		return clientSocket;
	}

	void SetSocketTimeout(int seconds)
	{
		struct timeval timeout;
		timeout.tv_sec = seconds;
		timeout.tv_usec = 0;

		setsockopt(m_socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		setsockopt(m_socketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	}
};