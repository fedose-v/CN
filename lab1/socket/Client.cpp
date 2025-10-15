#include "Client.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <netinet/in.h>
#include <unistd.h>

Client::Client(const std::string& serverIp, int port, const std::string& clientName)
	: m_serverIp(serverIp)
	, m_port(port)
	, m_clientName(clientName)
	, m_socketDescriptor(-1)
{
}

int Client::GetClientNumber() const
{
	int clientNumber;
	while (true)
	{
		std::cout << "Введите целое число (1-100) или другое число для остановки сервера: ";
		if (!(std::cin >> clientNumber))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Ошибка ввода. Пожалуйста, введите число." << std::endl;
			continue;
		}
		return clientNumber;
	}
}

bool Client::EstablishConnection()
{
	m_socketDescriptor = m_socketProvider.CreateSocket();
	if (m_socketDescriptor < 0)
	{
		return false;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(m_port);

	if (connect(m_socketDescriptor, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Ошибка: Не удалось подключиться к серверу");
		m_socketProvider.CloseSocket(m_socketDescriptor);
		return false;
	}

	std::cout << "Установлено соединение с " << m_serverIp << ":" << m_port << std::endl;
	return true;
}

void Client::DisplayResult(int clientNumber, const MessageData& serverData) const
{
	int sum = clientNumber + serverData.serverNumber;

	std::cout << "\n--- РЕЗУЛЬТАТ ВЗАИМОДЕЙСТВИЯ ---" << std::endl;
	std::cout << "Собственное имя: " << m_clientName << std::endl;
	std::cout << "Имя сервера:     " << serverData.serverName << std::endl;
	std::cout << "Свое число:      " << clientNumber << std::endl;
	std::cout << "Число сервера:   " << serverData.serverNumber << std::endl;
	std::cout << "Сумма чисел:     " << sum << std::endl;
	std::cout << "--------------------------------" << std::endl;
}

void Client::Start()
{
	std::cout << "--- Клиент: " << m_clientName << " ---" << std::endl;

	int clientNumber = GetClientNumber();

	if (!EstablishConnection())
	{
		std::cerr << "Не удалось установить соединение. Завершение работы." << std::endl;
		return;
	}

	std::string message = m_socketProvider.CreateMessage(m_clientName, clientNumber);
	if (!m_socketProvider.SendData(m_socketDescriptor, message))
	{
		m_socketProvider.CloseSocket(m_socketDescriptor);
		return;
	}
	std::cout << "Сообщение отправлено: '" << message << "'" << std::endl;

	if (clientNumber >= 1 && clientNumber <= 100)
	{
		std::cout << "Ожидание ответа от сервера..." << std::endl;
		std::string rawResponse = m_socketProvider.ReceiveData(m_socketDescriptor);

		if (rawResponse.empty())
		{
			std::cerr << "Ошибка: Не получен ответ от сервера или сервер закрыл соединение." << std::endl;
		}
		else
		{
			MessageData serverData = SocketProvider::ParseMessage(rawResponse);
			if (serverData.clientName == "ERROR")
			{
				std::cerr << "Ошибка: Некорректный формат ответа от сервера." << std::endl;
			}
			else
			{
				serverData.serverName = serverData.clientName;
				serverData.serverNumber = serverData.clientNumber;
				DisplayResult(clientNumber, serverData);
			}
		}
	}
	else
	{
		std::cout << "Отправлено число вне диапазона. Сервер должен завершить работу." << std::endl;
	}

	m_socketProvider.CloseSocket(m_socketDescriptor);
}