#pragma once
#include <string>

struct ParsedUrl
{
	std::string Host;
	std::string Path;
	int Port;
	bool IsValid;
};