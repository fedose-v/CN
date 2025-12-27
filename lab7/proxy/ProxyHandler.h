#pragma once
#include "../utils/StatisticsManager.h"
#include "CacheManager.h"
#include "NetworkClient.h"
#include "UrlParser.h"

class ProxyHandler
{
public:
	explicit ProxyHandler(int clientSocket)
		: m_clientSocket(clientSocket)
	{
	}

	void Handle()
	{
		std::vector<char> buffer(8192);
		auto bytesReceived = recv(m_clientSocket, buffer.data(), buffer.size(), 0);

		if (bytesReceived <= 0)
		{
			CloseConnection();
			return;
		}

		std::string rawRequest(buffer.data(), bytesReceived);
		std::string url = ExtractUrl(rawRequest);

		if (url.empty())
		{
			CloseConnection();
			return;
		}

		StatisticsManager::LogRequest(url);
		ProcessRequest(url);
		CloseConnection();
	}

private:
	int m_clientSocket;

	static std::string ExtractUrl(const std::string& request)
	{
		std::stringstream ss(request);
		std::string method, url;
		ss >> method >> url;
		if (method != "GET")
			return "";
		return url;
	}

	void ProcessRequest(const std::string& url)
	{
		if (CacheManager::IsCached(url))
		{
			StatisticsManager::LogHit(url);
			std::vector<char> data = CacheManager::Retrieve(url);
			SendToClient(data);
		}
		else
		{
			StatisticsManager::LogMiss(url);
			ParsedUrl parsed = UrlParser::Parse(url);

			if (!parsed.IsValid)
			{
				SendError(400, "Bad Request");
				return;
			}

			std::vector<char> data;
			if (NetworkClient::FetchData(parsed, data))
			{
				CacheManager::Store(url, data);
				SendToClient(data);
			}
			else
			{
				SendError(404, "Not Found or Connection Failed");
			}
		}
	}

	void SendToClient(const std::vector<char>& data) const
	{
		send(m_clientSocket, data.data(), data.size(), 0);
	}

	void SendError(int code, const std::string& message) const
	{
		std::string response = "HTTP/1.0 " + std::to_string(code) + " " + message + "\r\n";
		response += "Content-Type: text/plain\r\n\r\n";
		response += message;
		send(m_clientSocket, response.c_str(), response.length(), 0);
		StatisticsManager::LogError("Sent Error " + std::to_string(code) + ": " + message);
	}

	void CloseConnection() const
	{
		close(m_clientSocket);
	}
};