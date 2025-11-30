#pragma once
#include <string>

class HttpResponse
{
private:
	std::string m_statusLine;
	std::string m_headers;
	std::string m_body;

public:
	HttpResponse(int statusCode, const std::string& contentType, const std::string& body)
	{
		if (statusCode == 200)
		{
			m_statusLine = "HTTP/1.1 200 OK\r\n";
		}
		else if (statusCode == 404)
		{
			m_statusLine = "HTTP/1.1 404 Not Found\r\n";
		}
		else
		{
			m_statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
		}

		m_headers = "Content-Type: " + contentType + "\r\n";
		m_headers += "Connection: close\r\n";
		m_headers += "\r\n";
		m_body = body;
	}

	[[nodiscard]] std::string ToString() const
	{
		return m_statusLine + m_headers + m_body;
	}
};