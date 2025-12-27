#pragma once
#include "../utils/Logger.h"
#include "FileReader.h"
#include "UdpSocket.h"
#include <iomanip>

const int WINDOW_SIZE = 10;
const int TIMEOUT_MS = 100;

class RdtSender
{
private:
	UdpSocket m_socket;
	FileReader m_fileReader;
	bool m_debug;
	uint32_t m_base;
	uint32_t m_nextSeqNum;

	void SendPacket(const Packet& p)
	{
		m_socket.Send(p);
		if (m_debug)
		{
			std::cout << "[DEBUG] Sent packet Seq=" << p.seqNum << " Size=" << p.dataSize << std::endl;
		}
	}

public:
	RdtSender(bool debug)
		: m_debug(debug)
		, m_base(0)
		, m_nextSeqNum(0)
	{
	}

	void Run(const std::string& host, int port, const std::string& filename)
	{
		try
		{
			m_fileReader.Load(filename);
			m_socket.SetDestination(host, port);
			m_socket.EnableSimulation(true);

			size_t totalPackets = m_fileReader.GetCount();
			auto startTime = std::chrono::high_resolution_clock::now();
			uint64_t totalBytesSent = 0;

			Logger::Info("Starting transfer...");

			while (m_base < totalPackets)
			{
				while (m_nextSeqNum < m_base + WINDOW_SIZE && m_nextSeqNum < totalPackets)
				{
					SendPacket(m_fileReader.GetPacket(m_nextSeqNum));
					m_nextSeqNum++;
				}

				if (m_socket.Poll(TIMEOUT_MS))
				{
					Ack ack;
					if (m_socket.ReceiveAck(ack))
					{
						if (m_debug)
						{
							std::cout << "[DEBUG] Received ACK=" << ack.ackNum << std::endl;
						}
						if (ack.ackNum >= m_base)
						{
							m_base = ack.ackNum + 1;
						}
					}
				}
				else
				{
					if (m_debug)
					{
						std::cout << "[DEBUG] Timeout. Resending window from Base=" << m_base << std::endl;
					}
					for (uint32_t i = m_base; i < m_nextSeqNum; ++i)
					{
						SendPacket(m_fileReader.GetPacket(i));
					}
				}

				if (!m_debug)
				{
					float progress = (float)m_base / totalPackets * 100.0f;
					std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << progress << "%" << std::flush;
				}
			}

			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = endTime - startTime;

			std::cout << std::endl;
			Logger::Info("Transfer completed.");
			std::cout << "Time: " << diff.count() << " s" << std::endl;
		}
		catch (const std::exception& e)
		{
			Logger::Error(e.what());
		}
	}
};