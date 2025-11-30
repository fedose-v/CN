#pragma once

#include <iostream>

#include "FileHandler.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Socket.h"

class HttpServer
{
private:
	std::unique_ptr<Socket> ServerSocket;
	int Port;

public:
	explicit HttpServer(int port)
		: Port(port)
	{
		ServerSocket = std::make_unique<Socket>();
		ServerSocket->Bind(Port);
		ServerSocket->Listen();
		std::cout << "Server started on port " << Port << std::endl;
	}

	void Run()
	{
		while (true)
		{
			try
			{
				int clientSocket = ServerSocket->Accept();
				HandleClient(clientSocket);
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error accepting connection: " << e.what() << std::endl;
			}
		}
	}

private:
	static void HandleClient(int clientSocket)
	{
		try
		{
			HttpClient client(clientSocket);
			std::string requestStr = client.Receive();

			if (requestStr.empty())
			{
				return;
			}

			HttpRequest request(requestStr);
			std::string requestedPath = request.GetPath();

			if (request.GetMethod() != "GET")
			{
				HttpResponse response(405, "text/plain", "m_method Not Allowed");
				client.Send(response.ToString());
				std::cout << "Sent 405 response for method: " << request.GetMethod() << std::endl;
				return;
			}

			std::string fullPath = "." + SanitizePath(requestedPath);

			if (FileHandler::FileExists(fullPath))
			{
				std::string content = FileHandler::ReadFile(fullPath);
				std::string contentType = FileHandler::GetContentType(fullPath);
				HttpResponse response(200, contentType, content);
				client.Send(response.ToString());
				std::cout << "Sent 200 response for file: " << fullPath << std::endl;
			}
			else
			{
				HttpResponse response(404, "text/plain", "File Not Found");
				client.Send(response.ToString());
				std::cout << "Sent 404 response for file: " << fullPath << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error handling client: " << e.what() << std::endl;
			if (clientSocket >= 0)
			{
				close(clientSocket);
			}
		}
	}

	static std::string SanitizePath(const std::string& path)
	{
		if (path.find("..") != std::string::npos || path.find("//") != std::string::npos)
		{
			throw std::runtime_error("Invalid path");
		}
		return "." + path;
	}
};