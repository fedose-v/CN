#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class SocketHandler
{
public:
	SocketHandler()
		: m_sock(-1)
	{
		m_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_sock < 0)
		{
			throw std::runtime_error("Failed to create socket");
		}

		struct timeval tv;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		if (setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		{
			Close();
			throw std::runtime_error("Failed to set socket timeout");
		}
	}

	~SocketHandler()
	{
		Close();
	}

	void Send(const std::vector<uint8_t>& data, const std::string& ip, int port) const
	{
		struct sockaddr_in dest{};
		memset(&dest, 0, sizeof(dest));
		dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		if (inet_pton(AF_INET, ip.c_str(), &dest.sin_addr) <= 0)
		{
			throw std::runtime_error("Invalid IP address format");
		}

		ssize_t sent = sendto(m_sock, data.data(), data.size(), 0, (struct sockaddr*)&dest, sizeof(dest));
		if (sent < 0)
		{
			throw std::runtime_error("Failed to send data");
		}
	}

	int Receive(std::vector<uint8_t>& buffer) const
	{
		struct sockaddr_in src{};
		socklen_t len = sizeof(src);
		ssize_t received = recvfrom(m_sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&src, &len);
		if (received < 0)
		{
			return -1;
		}
		return static_cast<int>(received);
	}

private:
	void Close()
	{
		if (m_sock >= 0)
		{
			close(m_sock);
			m_sock = -1;
		}
	}

	int m_sock;
};