#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>

class FileHandler
{
public:
	static std::string ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("Cannot open file: " + path);
		}

		std::ostringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	static bool FileExists(const std::string& path)
	{
		struct stat buffer{};
		return (stat(path.c_str(), &buffer) == 0);
	}

	static std::string GetContentType(const std::string& path)
	{
		std::string extension;
		size_t pos = path.find_last_of('.');
		if (pos != std::string::npos)
		{
			extension = path.substr(pos);
		}

		if (extension == ".html" || extension == ".htm")
		{
			return "text/html";
		}
		else if (extension == ".css")
		{
			return "text/css";
		}
		else if (extension == ".js")
		{
			return "application/javascript";
		}
		else if (extension == ".png")
		{
			return "image/png";
		}
		else if (extension == ".jpg" || extension == ".jpeg")
		{
			return "image/jpeg";
		}
		else if (extension == ".gif")
		{
			return "image/gif";
		}
		else
		{
			return "text/plain";
		}
	}
};