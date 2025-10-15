#include "SocketProvider.h"
#include <algorithm>
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <iostream>
#include <stdexcept>

SocketProvider::SocketProvider() {}

SocketProvider::~SocketProvider() {}

int SocketProvider::CreateSocket()
{
	int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescriptor < 0)
	{
		perror("Ошибка: Не удалось создать сокет");
		return -1;
	}
	return socketDescriptor;
}

void SocketProvider::CloseSocket(int socketDescriptor)
{
	if (socketDescriptor >= 0)
	{
		close(socketDescriptor);
	}
}

bool SocketProvider::SendData(int socketDescriptor, const std::string& data)
{
	if (socketDescriptor < 0)
		return false;

	std::string message = data + "\n";

	ssize_t bytesSent = send(socketDescriptor, message.c_str(), message.length(), 0);
	if (bytesSent < 0)
	{
		perror("Ошибка: Отправка данных");
		return false;
	}
	return true;
}

std::string SocketProvider::ReceiveData(int socketDescriptor)
{
	if (socketDescriptor < 0)
		return "";

	char buffer[Config::BUFFER_SIZE] = { 0 };
	ssize_t bytesReceived = recv(socketDescriptor, buffer, Config::BUFFER_SIZE - 1, 0);

	if (bytesReceived > 0)
	{
		return std::string(buffer, bytesReceived);
	}
	else if (bytesReceived == 0)
	{
		return "";
	}
	else
	{
		perror("Ошибка: Получение данных");
		return "";
	}
}

MessageData SocketProvider::ParseMessage(const std::string& rawMessage)
{
	MessageData data = {};

	std::string cleanMessage = rawMessage;
	cleanMessage.erase(std::remove(cleanMessage.begin(), cleanMessage.end(), '\n'), cleanMessage.end());
	cleanMessage.erase(std::remove(cleanMessage.begin(), cleanMessage.end(), '\r'), cleanMessage.end());

	size_t colonPos = cleanMessage.find(':');
	if (colonPos == std::string::npos)
	{
		data.clientName = "ERROR";
		return data;
	}

	data.clientName = cleanMessage.substr(0, colonPos);
	std::string numberStr = cleanMessage.substr(colonPos + 1);

	try
	{
		data.clientNumber = std::stoi(numberStr);
	}
	catch (const std::exception&)
	{
		data.clientName = "ERROR";
		return data;
	}

	return data;
}

std::string SocketProvider::CreateMessage(const std::string& name, int number)
{
	return name + ":" + std::to_string(number);
}