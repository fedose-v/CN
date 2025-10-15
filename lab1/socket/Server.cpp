#include "Server.h"

#include <iostream>
#include <netinet/in.h>

Server::Server(const std::string& serverName, int serverNumber, int port)
	: m_serverName(serverName)
	, m_serverNumber(serverNumber)
	, m_port(port)
	, m_listenSocketDescriptor(-1)
{
}

bool Server::Initialize()
{
	m_listenSocketDescriptor = m_socketProvider.CreateSocket();
	if (m_listenSocketDescriptor < 0)
	{
		return false;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	serverAddress.sin_port = htons(m_port);

	if (bind(m_listenSocketDescriptor, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Ошибка: Привязка сокета");
		m_socketProvider.CloseSocket(m_listenSocketDescriptor);
		return false;
	}

	if (listen(m_listenSocketDescriptor, 5) < 0)
	{
		perror("Ошибка: Прослушивание сокета");
		m_socketProvider.CloseSocket(m_listenSocketDescriptor);
		return false;
	}

	std::cout << "Сервер " << m_serverName << " запущен на порту " << m_port << ". Ожидание подключений..." << std::endl;
	return true;
}

void Server::LogInteraction(const MessageData& clientData) const
{
	int sum = clientData.clientNumber + m_serverNumber;
	std::cout << "\n[Взаимодействие]" << std::endl;
	std::cout << "  Имя клиента: " << clientData.clientName << std::endl;
	std::cout << "  Имя сервера: " << m_serverName << std::endl;
	std::cout << "  Число клиента: " << clientData.clientNumber << std::endl;
	std::cout << "  Число сервера: " << m_serverNumber << std::endl;
	std::cout << "  Сумма: " << sum << std::endl;
}

bool Server::ProcessClient(int clientSocketDescriptor)
{
	std::cout << "\n[Подключение принято] Обработка клиента..." << std::endl;

	std::string rawMessage = m_socketProvider.ReceiveData(clientSocketDescriptor);
	if (rawMessage.empty())
	{
		std::cerr << "Предупреждение: Клиент закрыл соединение или произошла ошибка приема." << std::endl;
		m_socketProvider.CloseSocket(clientSocketDescriptor);
		return true;
	}

	MessageData clientData = m_socketProvider.ParseMessage(rawMessage);

	if (clientData.clientName == "ERROR")
	{
		std::cerr << "Ошибка: Некорректный формат сообщения. Закрытие соединения с клиентом." << std::endl;
		m_socketProvider.CloseSocket(clientSocketDescriptor);
		return true;
	}

	LogInteraction(clientData);

	if (clientData.clientNumber < 1 || clientData.clientNumber > 100)
	{
		std::cout << "\n--- Получено число вне диапазона (" << clientData.clientNumber << "). Завершение работы сервера. ---" << std::endl;
		m_socketProvider.CloseSocket(clientSocketDescriptor);
		return false;
	}

	std::string response = m_socketProvider.CreateMessage(m_serverName, m_serverNumber);
	if (!m_socketProvider.SendData(clientSocketDescriptor, response))
	{
		std::cerr << "Ошибка: Не удалось отправить ответ клиенту." << std::endl;
	}
	else
	{
		std::cout << "Ответ успешно отправлен." << std::endl;
	}

	m_socketProvider.CloseSocket(clientSocketDescriptor);
	std::cout << "[Соединение с клиентом закрыто]" << std::endl;
	return true;
}

void Server::Start()
{
	if (!Initialize())
	{
		return;
	}

	while (true)
	{
		sockaddr_in clientAddress;
		socklen_t clientAddressSize = sizeof(clientAddress);

		int clientSocketDescriptor = accept(m_listenSocketDescriptor, (sockaddr*)&clientAddress, &clientAddressSize);

		if (clientSocketDescriptor < 0)
		{
			perror("Ошибка: Принятие соединения (accept)");
			continue;
		}

		if (!ProcessClient(clientSocketDescriptor))
		{
			break;
		}
	}

	m_socketProvider.CloseSocket(m_listenSocketDescriptor);
	std::cout << "Сервер завершил работу." << std::endl;
}