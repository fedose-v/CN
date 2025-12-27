#pragma once
#include <arpa/inet.h>
#include <string>

struct DnsRecord
{
	std::string name;
	uint16_t type;
	std::string rdata;
};