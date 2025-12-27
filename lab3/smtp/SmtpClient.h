#pragma once
#include <utility>

#include "TcpSocket.h"

class SmtpClient
{
public:
	SmtpClient(std::string  server, int port)
		: m_server(std::move(server))
		, m_port(port)
	{
	}

	void SendEmail(const std::string& from, const std::string& to, const std::string& subject, const std::string& body)
	{
		try
		{
			m_socket.Connect(m_server, m_port);
			CheckResponse("220");

			SendMessage("HELO " + m_server + "\r\n");
			CheckResponse("250");

			SendMessage("MAIL FROM:<" + from + ">\r\n");
			CheckResponse("250");

			SendMessage("RCPT TO:<" + to + ">\r\n");
			CheckResponse("250");

			SendMessage("DATA\r\n");
			CheckResponse("354");

			std::string emailContent = "From: " + from + "\r\n" + "To: " + to + "\r\n" + "Subject: " + subject + "\r\n" + "\r\n" + body + "\r\n" + ".\r\n";

			SendMessage(emailContent);
			CheckResponse("250");

			SendMessage("QUIT\r\n");
		}
		catch (const std::exception& e)
		{
			throw;
		}
	}

private:
	std::string m_server;
	int m_port;
	TcpSocket m_socket;

	void SendMessage(const std::string& message)
	{
		m_socket.Send(message);
	}

	void CheckResponse(const std::string& expectedCode)
	{
		std::string response = m_socket.Receive();
		if (response.substr(0, 3) != expectedCode)
		{
			throw std::runtime_error("SMTP Error. Expected: " + expectedCode + ", Got: " + response);
		}
	}
};