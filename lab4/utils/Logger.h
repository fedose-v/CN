#pragma once
#include <iostream>
#include <string>

class Logger
{
public:
	static void SetDebug(bool debug)
	{
		m_debug = debug;
	}

	static void Log(const std::string& message)
	{
		if (m_debug)
		{
			std::cout << "[DEBUG] " << message << std::endl;
		}
	}

	static void Error(const std::string& message)
	{
		std::cerr << "[ERROR] " << message << std::endl;
	}

private:
	static bool m_debug;
};

bool Logger::m_debug = false;