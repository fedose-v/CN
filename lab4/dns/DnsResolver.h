#pragma once
#include "../utils/Logger.h"
#include "DnsPacketBuilder.h"
#include "DnsResponseParser.h"
#include "SocketHandler.h"

#include <random>

class DnsResolver
{
public:
	DnsResolver()
	{
		m_rootServers = {
			"198.41.0.4",
			"199.9.14.201",
			"192.33.4.12",
			"199.7.91.13",
			"192.203.230.10",
			"192.5.5.241",
			"192.112.36.4",
			"198.97.190.53",
			"192.36.148.17",
			"192.58.128.30",
			"193.0.14.129",
			"199.7.83.42",
			"202.12.27.33"
		};
	}

	void Resolve(const std::string& domain, const std::string& recordTypeStr)
	{
		uint16_t type = RecordTypeToId(recordTypeStr);
		try
		{
			std::string result = InternalResolve(domain, type, 0);
			if (!result.empty())
			{
				std::cout << result << std::endl;
			}
			else
			{
				std::cout << "No record found." << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			Logger::Error(e.what());
		}
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
		throw std::invalid_argument("Unsupported record type");
	}

	std::string GetRandomRoot()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(0, m_rootServers.size() - 1);
		return m_rootServers[distrib(gen)];
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

	std::string InternalResolve(const std::string& domain, uint16_t type, int depth)
	{
		if (depth > 10)
			throw std::runtime_error("Max recursion depth reached");

		std::string currentNs = GetRandomRoot();
		Logger::Log("Starting resolution for " + domain + " with root " + currentNs);

		int loops = 0;
		while (loops++ < 30)
		{
			Logger::Log("Querying server: " + currentNs + " for " + domain);

			try
			{
				auto response = Query(currentNs, domain, type);

				if (!response.answers.empty())
				{
					for (const auto& ans : response.answers)
					{
						if (ans.type == type)
						{
							return ans.rdata;
						}
						else if (ans.type == 5)
						{
							Logger::Log("CNAME found: " + ans.rdata + ". Restarting.");
							return InternalResolve(ans.rdata, type, depth + 1);
						}
					}
				}

				if (response.authorities.empty())
				{
					Logger::Log("No authorities found.");
					return "";
				}

				bool newNsFound = false;
				std::string missingGlueNs;

				for (const auto& ns : response.authorities)
				{
					if (ns.type == 2)
					{
						bool hasGlue = false;
						for (const auto& add : response.additionals)
						{
							if (add.name == ns.rdata && add.type == 1)
							{
								currentNs = add.rdata;
								Logger::Log("Following glue: " + ns.rdata + " -> " + currentNs);
								hasGlue = true;
								newNsFound = true;
								break;
							}
						}
						if (hasGlue)
							break;
						if (missingGlueNs.empty())
							missingGlueNs = ns.rdata;
					}
				}

				if (!newNsFound)
				{
					if (!missingGlueNs.empty())
					{
						Logger::Log("Missing glue for NS " + missingGlueNs + ". resolving recursively.");
						std::string resolvedIp = InternalResolve(missingGlueNs, 1, depth + 1);
						if (!resolvedIp.empty())
						{
							currentNs = resolvedIp;
							Logger::Log("Resolved NS " + missingGlueNs + " to " + currentNs);
							continue;
						}
					}
					throw std::runtime_error("Dead end in resolution chain");
				}
			}
			catch (const std::exception& e)
			{
				Logger::Log(std::string("Query failed: ") + e.what());
				if (loops == 1)
				{
					currentNs = GetRandomRoot();
				}
				else
				{
					throw;
				}
			}
		}
		throw std::runtime_error("Too many loops");
	}
};