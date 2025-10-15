#include "socket/Server.h"
#include <cstdlib>

int main() {
	Server myServer(Config::SERVER_NAME, Config::SERVER_FIXED_NUMBER, Config::PORT);
	myServer.Start();

	return EXIT_SUCCESS;
}