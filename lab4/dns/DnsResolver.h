#pragma once
#include "../utils/Logger.h"
#include "DnsPacketBuilder.h"
#include "DnsResponseParser.h"
#include "SocketHandler.h"

class DnsResolver
{
public:
	DnsResolver()
	{
		m_rootServers = { "198.41.0.4", "199.9.14.201", "192.33.4.12" };
	}

	void Resolve(const std::string& domain, const std::string& recordTypeStr)
	{
		uint16_t type = RecordTypeToId(recordTypeStr);
		std::string currentNs = m_rootServers[0];

		Logger::Log("Starting resolution for " + domain);

		int loops = 0;
		while (loops++ < 20)
		{
			Logger::Log("Querying server: " + currentNs);

			try
			{
				auto response = Query(currentNs, domain, type);

				if (!response.answers.empty())
				{
					bool found = false;
					for (const auto& ans : response.answers)
					{
						if (ans.type == type)
						{
							std::cout << ans.rdata << std::endl;
							found = true;
						}
						else if (ans.type == 46)
						{
							Logger::Log("RRSIG found: " + ans.rdata);
							if (recordTypeStr == "DNSSEC" || recordTypeStr == "RRSIG")
							{
								std::cout << ans.rdata << std::endl;
								found = true;
							}
						}
						else if (ans.type == 5)
						{
							Logger::Log("CNAME found: " + ans.rdata + ". Restarting resolution.");
							Resolve(ans.rdata, recordTypeStr);
							return;
						}
					}
					if (found)
						return;
				}

				if (response.authorities.empty())
				{
					Logger::Error("No authorities found. Resolution failed.");
					return;
				}

				bool glueFound = false;
				for (const auto& ns : response.authorities)
				{
					if (ns.type == 2)
					{
						for (const auto& add : response.additionals)
						{
							if (add.name == ns.rdata && add.type == 1)
							{
								currentNs = add.rdata;
								Logger::Log("Found glue record for " + ns.rdata + " -> " + currentNs);
								glueFound = true;
								break;
							}
						}
						if (glueFound)
							break;
					}
				}

				if (!glueFound)
				{
					Logger::Log("No glue record found. Attempting to resolve NS: " + response.authorities[0].rdata);

					Logger::Error("Deep recursion required (not implemented in basic resolver). Aborting.");
					return;
				}
			}
			catch (const std::exception& e)
			{
				Logger::Error(std::string("Communication error: ") + e.what());
				return;
			}
		}
		Logger::Error("Too many redirection loops.");
	}

private:
	struct DnsResponse
	{
		std::vector<DnsRecord> answers;
		std::vector<DnsRecord> authorities;
		std::vector<DnsRecord> additionals;
	};

	std::vector<std::string> m_rootServers;

	static uint16_t RecordTypeToId(const std::string& type)
	{
		if (type == "A")
			return 1;
		if (type == "NS")
			return 2;
		if (type == "CNAME")
			return 5;
		if (type == "AAAA")
			return 28;
		if (type == "DNSKEY")
			return 48;
		if (type == "DS")
			return 43;
		if (type == "RRSIG")
			return 46;
		throw std::invalid_argument("Unsupported record type");
	}

	static DnsResponse Query(const std::string& serverIp, const std::string& domain, uint16_t type)
	{
		SocketHandler socket;
		std::vector<uint8_t> query = DnsPacketBuilder::BuildQuery(domain, type);

		socket.Send(query, serverIp, 53);

		std::vector<uint8_t> buffer(65535);
		int bytesRead = socket.Receive(buffer);

		if (bytesRead <= 0)
		{
			throw std::runtime_error("Timeout or empty response from " + serverIp);
		}

		DnsResponseParser parser(buffer, bytesRead);
		DnsResponse response;
		parser.Parse(response.answers, response.authorities, response.additionals);

		return response;
	}
};