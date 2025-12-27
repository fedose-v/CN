#pragma once
#include "InputValidator.h"
#include "SmtpClient.h"
#include <iostream>

class UserInterface
{
public:
	explicit UserInterface(std::istream& istream, std::ostream& ostream)
		: m_istream(istream)
		, m_ostream(ostream)
	{
	}

	void Run()
	{
		try
		{
			int port = DEFAULT_PORT;
			std::string server = GetInput("Enter SMTP Server Address: ", InputValidator::IsValidSmtpServerAddress);
			std::string sender = GetInput("Enter Sender Email: ", InputValidator::IsValidEmail);
			std::string recipient = GetInput("Enter Recipient Email: ", InputValidator::IsValidEmail);
			std::string subject = GetInput("Enter Subject: ", InputValidator::IsNotEmpty);
			std::string body = GetInput("Enter Email Body: ", InputValidator::IsNotEmpty);

			m_ostream << "\nConnecting to " << server << ":" << port << "...\n";

			SmtpClient client(server, port);
			client.SendEmail(sender, recipient, subject, body);

			m_ostream << "Email sent successfully.\n";
		}
		catch (const std::exception& e)
		{
			m_ostream << "Application Error: " << e.what() << std::endl;
		}
	}

private:
	static inline const auto DEFAULT_PORT = 25;

	std::istream& m_istream;
	std::ostream& m_ostream;

	std::string GetInput(const std::string& prompt, bool (*validator)(const std::string&))
	{
		std::string input;
		while (true)
		{
			m_ostream << prompt;
			std::getline(m_istream, input);
			if (validator(input))
			{
				return input;
			}
			m_ostream << "Invalid input. Please try again.\n";
		}
	}
};