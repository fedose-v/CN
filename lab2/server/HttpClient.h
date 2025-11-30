#pragma once
#include <cstring>
#include <memory>
#include <sys/socket.h>

class HttpClient
{
private:
	int SocketFd;

public:
	explicit HttpClient(int socketFd)
		: SocketFd(socketFd)
	{
	}

	~HttpClient()
	{
		if (SocketFd >= 0)
		{
			close(SocketFd);
		}
	}

	std::string Receive(size_t maxSize = 8192)
	{
		std::string result;
		char buffer[1024];
		size_t totalReceived = 0;

		while (totalReceived < maxSize)
		{
			ssize_t bytesRead = recv(SocketFd, buffer,
				std::min(sizeof(buffer), maxSize - totalReceived), 0);

			if (bytesRead < 0)
			{
				if (errno == EINTR)
				{
					continue;
				}
				throw std::runtime_error("Error reading from socket: " + std::string(strerror(errno)));
			}
			else if (bytesRead == 0)
			{
				if (totalReceived == 0)
				{
					throw std::runtime_error("Client disconnected");
				}
				break;
			}

			result.append(buffer, bytesRead);
			totalReceived += bytesRead;

			if (result.find("\r\n\r\n") != std::string::npos)
			{
				break;
			}

			if (static_cast<size_t>(bytesRead) < sizeof(buffer))
			{
				break;
			}
		}

		if (totalReceived >= maxSize && result.find("\r\n\r\n") == std::string::npos)
		{
			throw std::runtime_error("Request too large");
		}

		return result;
	}

	void Send(const std::string& data)
	{
		const char* ptr = data.c_str();
		size_t totalSent = 0;
		while (totalSent < data.length())
		{
			ssize_t bytesSent = send(SocketFd, ptr + totalSent, data.length() - totalSent, 0);
			if (bytesSent < 0)
			{
				throw std::runtime_error("Error sending data");
			}
			totalSent += bytesSent;
		}
	}
};