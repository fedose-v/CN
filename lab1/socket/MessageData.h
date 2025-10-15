#pragma once

#include <string>

namespace Config
{
const int PORT = 55002;
const int BUFFER_SIZE = 1024;
const std::string SERVER_IP = "127.0.0.1";
const std::string SERVER_NAME = "Server of Vladimir Fedoseev";
const int SERVER_FIXED_NUMBER = 50;
const std::string CLIENT_NAME = "Client of Fladimir Vedoseev";
} // namespace Config

struct MessageData
{
	std::string clientName;
	int clientNumber;
	std::string serverName;
	int serverNumber;
};