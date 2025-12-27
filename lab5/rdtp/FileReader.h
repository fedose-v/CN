#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Packet.h"

class FileReader
{
private:
	std::vector<Packet> m_packets;

public:
	void Load(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("Cannot open file: " + filename);
		}

		uint32_t seq = 0;
		while (!file.eof())
		{
			Packet p;
			p.seqNum = seq;
			file.read(p.data, MSS);
			p.dataSize = file.gcount();
			if (p.dataSize > 0)
			{
				m_packets.push_back(p);
				seq++;
			}
			if (file.peek() == EOF)
				break;
		}

		Packet endP;
		endP.seqNum = seq;
		endP.dataSize = 0;
		m_packets.push_back(endP);
	}

	const Packet& GetPacket(size_t index) const
	{
		return m_packets.at(index);
	}

	size_t GetCount() const
	{
		return m_packets.size();
	}
};