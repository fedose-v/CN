#include <iostream>
#include <vector>

#include "server/HttpServer.h"

int main()
{
	try
	{
		HttpServer server(8880);
		server.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Server error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}