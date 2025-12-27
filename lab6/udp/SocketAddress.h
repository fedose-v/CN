#pragma once
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>

class SocketAddress
{
private:
	struct sockaddr_in m_addr{};
	socklen_t m_addrLen;

public:
	SocketAddress()
		: m_addrLen(sizeof(m_addr))
	{
		std::memset(&m_addr, 0, sizeof(m_addr));
	}

	struct sockaddr* GetSockAddr()
	{
		return (struct sockaddr*)&m_addr;
	}

	socklen_t* GetAddrLenPtr()
	{
		return &m_addrLen;
	}

	[[nodiscard]] socklen_t GetAddrLen() const
	{
		return m_addrLen;
	}
};