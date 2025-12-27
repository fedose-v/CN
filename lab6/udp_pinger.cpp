#include "udp/PingClient.h"

int main()
{
	try
	{
		PingClient client("127.0.0.1", 12000);
		client.Run();
	}
	catch (const std::exception& e)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}