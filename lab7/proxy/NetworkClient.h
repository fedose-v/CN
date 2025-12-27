#pragma once
#include <cstring>
#include <netdb.h>
#include <csignal>
#include <vector>

#include "ParsedUrl.h"

class NetworkClient
{
public:
	static bool FetchData(const ParsedUrl& target, std::vector<char>& outputData)
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0)
			return false;

		struct hostent* host = gethostbyname(target.Host.c_str());
		if (host == nullptr)
		{
			close(sock);
			return false;
		}

		struct sockaddr_in serverAddr{};
		std::memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		std::memcpy(&serverAddr.sin_addr.s_addr, host->h_addr, host->h_length);
		serverAddr.sin_port = htons(target.Port);

		if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			close(sock);
			return false;
		}

		std::string request = "GET " + target.Path + " HTTP/1.0\r\n";
		request += "Host: " + target.Host + "\r\n";
		request += "Connection: close\r\n\r\n";

		if (send(sock, request.c_str(), request.length(), 0) < 0)
		{
			close(sock);
			return false;
		}

		char buffer[4096];
		size_t bytesRead;
		while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0)
		{
			outputData.insert(outputData.end(), buffer, buffer + bytesRead);
		}

		close(sock);
		return true;
	}
};