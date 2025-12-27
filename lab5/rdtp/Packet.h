#pragma once
#include <arpa/inet.h>

const int MSS = 1024;

struct Packet
{
	uint32_t seqNum;
	uint32_t dataSize;
	char data[MSS];
};
