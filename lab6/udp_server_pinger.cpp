#include "udp/PingServer.h"

int main()
{
	try
	{
		PingServer server(12000);
		server.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}