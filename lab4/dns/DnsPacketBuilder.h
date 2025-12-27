#pragma once
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <vector>

#include "DnsHeader.h"

class DnsPacketBuilder
{
public:
	static std::vector<uint8_t> BuildQuery(const std::string& domain, uint16_t type)
	{
		std::vector<uint8_t> packet;

		DnsHeader header{};
		memset(&header, 0, sizeof(header));
		header.id = htons(getpid() & 0xFFFF);
		header.flags = htons(0x0100);
		header.qdCount = htons(1);
		header.arCount = htons(1);

		auto* p = reinterpret_cast<uint8_t*>(&header);
		packet.insert(packet.end(), p, p + sizeof(header));

		AppendName(packet, domain);

		uint16_t qtype = htons(type);
		uint16_t qclass = htons(1);

		p = reinterpret_cast<uint8_t*>(&qtype);
		packet.insert(packet.end(), p, p + 2);
		p = reinterpret_cast<uint8_t*>(&qclass);
		packet.insert(packet.end(), p, p + 2);

		AppendOptRr(packet);

		return packet;
	}

private:
	static void AppendName(std::vector<uint8_t>& packet, const std::string& domain)
	{
		size_t start = 0;
		size_t end;

		while ((end = domain.find('.', start)) != std::string::npos)
		{
			packet.push_back(static_cast<uint8_t>(end - start));
			for (size_t i = start; i < end; ++i)
			{
				packet.push_back(domain[i]);
			}
			start = end + 1;
		}

		if (start < domain.length())
		{
			packet.push_back(static_cast<uint8_t>(domain.length() - start));
			for (size_t i = start; i < domain.length(); ++i)
			{
				packet.push_back(domain[i]);
			}
		}
		packet.push_back(0);
	}

	static void AppendOptRr(std::vector<uint8_t>& packet)
	{
		packet.push_back(0);

		uint16_t type = htons(41);
		uint16_t payloadSize = htons(4096);

		uint32_t flags = htonl(0x00008000);

		uint16_t dataLen = 0;

		auto* p = reinterpret_cast<uint8_t*>(&type);
		packet.insert(packet.end(), p, p + 2);

		p = reinterpret_cast<uint8_t*>(&payloadSize);
		packet.insert(packet.end(), p, p + 2);

		p = reinterpret_cast<uint8_t*>(&flags);
		packet.insert(packet.end(), p, p + 4);

		p = reinterpret_cast<uint8_t*>(&dataLen);
		packet.insert(packet.end(), p, p + 2);
	}
};