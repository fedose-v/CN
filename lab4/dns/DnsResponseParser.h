#pragma once
#include <stdexcept>
#include <vector>

#include "DnsHeader.h"
#include "DnsRecord.h"

class DnsResponseParser
{
public:
	DnsResponseParser(const std::vector<uint8_t>& buffer, int size)
		: m_buffer(buffer)
		, m_size(size)
		, m_pos(0)
	{
	}

	void Parse(std::vector<DnsRecord>& answers, std::vector<DnsRecord>& authorities, std::vector<DnsRecord>& additionals)
	{
		if (m_size < static_cast<int>(sizeof(DnsHeader)))
			throw std::runtime_error("Response too short");

		auto* h = (DnsHeader*)m_buffer.data();
		int qd = ntohs(h->qdCount);
		int an = ntohs(h->anCount);
		int ns = ntohs(h->nsCount);
		int ar = ntohs(h->arCount);

		m_pos = sizeof(DnsHeader);

		for (int i = 0; i < qd; ++i)
			SkipQuestion();
		for (int i = 0; i < an; ++i)
			answers.push_back(ParseRecord());
		for (int i = 0; i < ns; ++i)
			authorities.push_back(ParseRecord());
		for (int i = 0; i < ar; ++i)
			additionals.push_back(ParseRecord());
	}

private:
	std::string ReadName()
	{
		std::string name;
		int p = m_pos;
		int jumped = 0;
		int jumps = 0;

		while (true)
		{
			if (jumps > 10)
				throw std::runtime_error("Loop detected in name compression");
			if (p >= m_size)
				throw std::runtime_error("Packet overflow reading name");

			uint8_t len = m_buffer[p];

			if (len == 0)
			{
				p++;
				break;
			}

			if ((len & 0xC0) == 0xC0)
			{
				if (p + 1 >= m_size)
					throw std::runtime_error("Packet overflow reading pointer");
				int offset = ((len & 0x3F) << 8) | m_buffer[p + 1];
				if (!jumped)
				{
					m_pos = p + 2;
				}
				jumped = 1;
				p = offset;
				jumps++;
			}
			else
			{
				p++;
				if (p + len > m_size)
					throw std::runtime_error("Packet overflow reading label");
				if (!name.empty())
					name += ".";
				for (int i = 0; i < len; ++i)
				{
					name += (char)m_buffer[p + i];
				}
				p += len;
			}
		}

		if (!jumped)
			m_pos = p;
		return name;
	}

	void SkipQuestion()
	{
		ReadName();
		m_pos += 4;
	}

	DnsRecord ParseRecord()
	{
		DnsRecord record;
		record.name = ReadName();

		if (m_pos + 10 > m_size)
			throw std::runtime_error("Packet overflow reading record header");

		uint16_t type = (m_buffer[m_pos] << 8) | m_buffer[m_pos + 1];
		record.type = type;

		uint16_t dataLen = (m_buffer[m_pos + 8] << 8) | m_buffer[m_pos + 9];
		m_pos += 10;

		if (m_pos + dataLen > m_size)
			throw std::runtime_error("Packet overflow reading rdata");

		if (type == 1 && dataLen == 4)
		{
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &m_buffer[m_pos], ip, INET_ADDRSTRLEN);
			record.rdata = std::string(ip);
		}
		else if (type == 28 && dataLen == 16)
		{
			char ip[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &m_buffer[m_pos], ip, INET6_ADDRSTRLEN);
			record.rdata = std::string(ip);
		}
		else if (type == 2 || type == 5)
		{
			int oldPos = m_pos;
			record.rdata = ReadName();
			m_pos = oldPos;
		}
		else if (type == 48)
		{
			record.rdata = "DNSKEY (Hex): " + ToHex(m_pos, dataLen);
		}
		else if (type == 43)
		{
			record.rdata = "DS (Hex): " + ToHex(m_pos, dataLen);
		}
		else if (type == 46)
		{
			record.rdata = "RRSIG (Hex): " + ToHex(m_pos, dataLen);
		}
		else if (type == 47)
		{
			record.rdata = "NSEC Record";
		}
		else if (type == 50)
		{
			record.rdata = "NSEC3 Record";
		}
		else
		{
			record.rdata = "(binary data type " + std::to_string(type) + ")";
		}

		m_pos += dataLen;
		return record;
	}

	std::string ToHex(int start, int length)
	{
		static const char hex[] = "0123456789ABCDEF";
		std::string result;
		for (int i = 0; i < length; ++i)
		{
			if (start + i >= m_size)
				break;
			unsigned char byte = m_buffer[start + i];
			result += hex[byte >> 4];
			result += hex[byte & 0x0F];
		}
		return result;
	}

	const std::vector<uint8_t>& m_buffer;
	int m_size;
	int m_pos;
};