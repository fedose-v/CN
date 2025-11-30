#pragma once
#include <sstream>
#include <string>
#include <vector>

class HttpRequest
{
private:
	std::string m_method;
	std::string m_path;
	std::string m_version;
	std::vector<std::string> m_headers;

public:
	explicit HttpRequest(const std::string& request)
	{
		Parse(request);
	}

	void Parse(const std::string& request)
	{
		std::istringstream iss(request);
		std::string line;
		std::getline(iss, line);

		std::istringstream lineStream(line);
		lineStream >> m_method >> m_path >> m_version;

		if (m_path == "/")
		{
			m_path = "/index.html";
		}

		std::string headerLine;
		while (std::getline(iss, headerLine) && headerLine != "\r" && !headerLine.empty())
		{
			m_headers.push_back(headerLine);
		}
	}

	[[nodiscard]] std::string GetMethod() const
	{
		return m_method;
	}

	[[nodiscard]] std::string GetPath() const
	{
		return m_path;
	}
};