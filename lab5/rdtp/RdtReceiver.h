#pragma once
#include "fstream"

#include "../utils/Logger.h"
#include "UdpSocket.h"

class RdtReceiver
{
private:
	UdpSocket m_socket;
	bool m_debug;
	uint32_t m_expectedSeqNum;

public:
	RdtReceiver(bool debug)
		: m_debug(debug)
		, m_expectedSeqNum(0)
	{
	}

	void Run(int port, const std::string& filename)
	{
		try
		{
			m_socket.Bind(port);
			std::ofstream outFile(filename, std::ios::binary);
			if (!outFile.is_open())
			{
				throw std::runtime_error("Cannot open output file");
			}

			Logger::Info("Waiting for data on port " + std::to_string(port) + "...");

			while (true)
			{
				Packet packet;
				struct sockaddr_in senderAddr;

				if (m_socket.Receive(packet, senderAddr))
				{
					if (packet.seqNum == m_expectedSeqNum)
					{
						if (m_debug)
						{
							std::cout << "[DEBUG] Accepted Packet Seq=" << packet.seqNum << std::endl;
						}

						if (packet.dataSize == 0)
						{
							Ack ack;
							ack.ackNum = packet.seqNum;
							m_socket.SendAck(ack, senderAddr);
							Logger::Info("End of transmission received.");
							break;
						}

						outFile.write(packet.data, packet.dataSize);

						Ack ack;
						ack.ackNum = m_expectedSeqNum;
						m_socket.SendAck(ack, senderAddr);

						m_expectedSeqNum++;
					}
					else
					{
						if (m_debug)
						{
							std::cout << "[DEBUG] Out of order. Expected=" << m_expectedSeqNum << " Got=" << packet.seqNum << std::endl;
						}
						if (m_expectedSeqNum > 0)
						{
							Ack ack;
							ack.ackNum = m_expectedSeqNum - 1;
							m_socket.SendAck(ack, senderAddr);
						}
					}
				}
			}
			outFile.close();
		}
		catch (const std::exception& e)
		{
			Logger::Error(e.what());
		}
	}
};