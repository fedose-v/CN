#pragma once
#include "ParsedUrl.h"

class UrlParser
{
public:
	static ParsedUrl Parse(const std::string& url)
	{
		ParsedUrl result;
		result.Port = 80;
		result.IsValid = false;

		std::string cleanUrl = url;
		if (cleanUrl.find("http://") == 0)
		{
			cleanUrl = cleanUrl.substr(7);
		}
		if (cleanUrl.find("https://") == 0)
		{
			cleanUrl = cleanUrl.substr(8);
		}

		size_t pathPos = cleanUrl.find('/');
		std::string hostPort;

		if (pathPos == std::string::npos)
		{
			hostPort = cleanUrl;
			result.Path = "/";
		}
		else
		{
			hostPort = cleanUrl.substr(0, pathPos);
			result.Path = cleanUrl.substr(pathPos);
		}

		size_t portPos = hostPort.find(':');
		if (portPos != std::string::npos)
		{
			result.Host = hostPort.substr(0, portPos);
			try
			{
				result.Port = std::stoi(hostPort.substr(portPos + 1));
			}
			catch (...)
			{
				return result;
			}
		}
		else
		{
			result.Host = hostPort;
		}

		if (!result.Host.empty())
		{
			result.IsValid = true;
		}

		return result;
	}
};