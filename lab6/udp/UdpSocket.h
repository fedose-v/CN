#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <vector>

class UdpSocket
{
private:
	int m_socketFd;

public:
	UdpSocket()
	{
		m_socketFd = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socketFd < 0)
		{
			throw std::runtime_error("Socket creation failed");
		}
	}

	~UdpSocket()
	{
		Close();
	}

	void SetSocketTimeout(int seconds) const
	{
		struct timeval tv{};
		tv.tv_sec = seconds;
		tv.tv_usec = 0;
		if (setsockopt(m_socketFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		{
			throw std::runtime_error("Set socket timeout failed");
		}
	}

	void SendTo(const std::string& message, const std::string& ipAddress, int port) const
	{
		struct sockaddr_in serverAddr{};
		std::memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);

		if (inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr) <= 0)
		{
			throw std::runtime_error("Invalid address");
		}

		ssize_t sentBytes = sendto(m_socketFd, message.c_str(), message.length(), 0,
			(struct sockaddr*)&serverAddr, sizeof(serverAddr));

		if (sentBytes < 0)
		{
			throw std::runtime_error("Send failed");
		}
	}

	[[nodiscard]] std::string ReceiveFrom(int bufferSize) const
	{
		std::vector<char> buffer(bufferSize);
		struct sockaddr_in fromAddr{};
		socklen_t fromLen = sizeof(fromAddr);

		ssize_t recvBytes = recvfrom(m_socketFd, buffer.data(), bufferSize, 0,
			(struct sockaddr*)&fromAddr, &fromLen);

		if (recvBytes < 0)
		{
			return "";
		}

		return std::string(buffer.data(), recvBytes);
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