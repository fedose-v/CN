#pragma once
#include <cstring>
#include <netdb.h>
#include <poll.h>
#include <stdexcept>
#include <thread>
#include <unistd.h>

#include "Packet.h"
#include "Ack.h"

const auto MIN_DELAY_MS = 0;
const auto MAX_DELAY_MS = 20;
const auto ACK_LOSS_RATE = 0.1;
const auto PACKET_LOSS_RATE = 0.1;

class UdpSocket
{
private:
	int m_socketFd;
	struct sockaddr_in m_localAddr;
	struct sockaddr_in m_remoteAddr;
	bool m_simulateLoss;

	bool ShouldDrop(double probability)
	{
		if (!m_simulateLoss)
			return false;
		double r = (double)rand() / RAND_MAX;
		return r < probability;
	}

	void SimulateDelay()
	{
		if (!m_simulateLoss)
			return;
		if (MAX_DELAY_MS > 0)
		{
			int delay = MIN_DELAY_MS + rand() % (MAX_DELAY_MS - MIN_DELAY_MS + 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}
	}

public:
	UdpSocket()
		: m_socketFd(-1)
		, m_simulateLoss(false)
	{
		m_socketFd = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socketFd < 0)
		{
			throw std::runtime_error("Failed to create socket");
		}
		srand(time(NULL));
	}

	~UdpSocket()
	{
		if (m_socketFd >= 0)
		{
			close(m_socketFd);
		}
	}

	void Bind(int port)
	{
		memset(&m_localAddr, 0, sizeof(m_localAddr));
		m_localAddr.sin_family = AF_INET;
		m_localAddr.sin_addr.s_addr = INADDR_ANY;
		m_localAddr.sin_port = htons(port);

		if (bind(m_socketFd, (struct sockaddr*)&m_localAddr, sizeof(m_localAddr)) < 0)
		{
			throw std::runtime_error("Failed to bind socket");
		}
	}

	void SetDestination(const std::string& host, int port)
	{
		struct hostent* he;
		if ((he = gethostbyname(host.c_str())) == NULL)
		{
			throw std::runtime_error("Failed to resolve hostname");
		}

		memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));
		m_remoteAddr.sin_family = AF_INET;
		m_remoteAddr.sin_port = htons(port);
		memcpy(&m_remoteAddr.sin_addr, he->h_addr_list[0], he->h_length);
	}

	void EnableSimulation(bool enable)
	{
		m_simulateLoss = enable;
	}

	bool Send(const Packet& packet)
	{
		if (ShouldDrop(PACKET_LOSS_RATE))
		{
			return true;
		}
		SimulateDelay();
		ssize_t sent = sendto(m_socketFd, &packet, sizeof(Packet), 0,
			(struct sockaddr*)&m_remoteAddr, sizeof(m_remoteAddr));
		return sent == sizeof(Packet);
	}

	bool SendAck(const Ack& ack, const struct sockaddr_in& dest)
	{
		if (ShouldDrop(ACK_LOSS_RATE))
		{
			return true;
		}
		SimulateDelay();
		ssize_t sent = sendto(m_socketFd, &ack, sizeof(Ack), 0,
			(struct sockaddr*)&dest, sizeof(dest));
		return sent == sizeof(Ack);
	}

	bool Receive(Packet& packet, struct sockaddr_in& sender)
	{
		socklen_t addrLen = sizeof(sender);
		ssize_t received = recvfrom(m_socketFd, &packet, sizeof(Packet), 0,
			(struct sockaddr*)&sender, &addrLen);
		return received > 0;
	}

	bool ReceiveAck(Ack& ack)
	{
		struct sockaddr_in sender;
		socklen_t addrLen = sizeof(sender);
		ssize_t received = recvfrom(m_socketFd, &ack, sizeof(Ack), 0,
			(struct sockaddr*)&sender, &addrLen);
		return received == sizeof(Ack);
	}

	bool Poll(int timeoutMs)
	{
		struct pollfd pfd;
		pfd.fd = m_socketFd;
		pfd.events = POLLIN;
		int ret = poll(&pfd, 1, timeoutMs);
		return ret > 0;
	}
};