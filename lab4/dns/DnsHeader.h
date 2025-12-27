#pragma once
#include <arpa/inet.h>

struct DnsHeader
{
	uint16_t id;
	uint16_t flags;
	uint16_t qdCount;
	uint16_t anCount;
	uint16_t nsCount;
	uint16_t arCount;
};