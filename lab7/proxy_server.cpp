#include "proxy/ProxyServer.h"

int main(int argc, char* argv[])
{
	int port = 8888;
	if (argc > 1)
	{
		port = std::atoi(argv[1]);
	}

	ProxyServer server(port);
	server.Start();

	return EXIT_SUCCESS;
}