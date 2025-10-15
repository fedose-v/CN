#include "socket/Client.h"
#include <cstdlib>

int main() {
	Client myClient(Config::SERVER_IP, Config::PORT, Config::CLIENT_NAME);
	myClient.Start();

	return EXIT_SUCCESS;
}