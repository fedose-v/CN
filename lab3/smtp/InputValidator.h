#pragma once
#include <regex>
#include <string>
#include <ranges>

class InputValidator
{
public:
	static bool IsValidEmail(const std::string& email)
	{
		const std::regex pattern(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
		return std::regex_match(email, pattern);
	}

	static bool IsNotEmpty(const std::string& str)
	{
		return !str.empty();
	}

	static bool IsValidSmtpServerAddress(const std::string& str)
	{
		return std::find(AVAILABLE_SMTP_SERVERS.begin(), AVAILABLE_SMTP_SERVERS.end(), str) != AVAILABLE_SMTP_SERVERS.end();
	}

private:
	static inline const std::vector<std::string> AVAILABLE_SMTP_SERVERS = {
		"smtp.mail.ru",
		"smtp.yandex.ru",
	};
};