#pragma once
#include <iostream>

class Logger
{
public:
	static void Log(bool debug, const std::string& message)
	{
		if (debug)
		{
			std::cout << "[DEBUG] " << message << std::endl;
		}
	}

	static void Error(const std::string& message)
	{
		std::cerr << "[ERROR] " << message << std::endl;
	}

	static void Info(const std::string& message)
	{
		std::cout << "[INFO] " << message << std::endl;
	}
};