#include "rdtp/RdtReceiver.h"
#include "rdtp/RdtSender.h"
#include "utils/Logger.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "  Sender:   " << argv[0] << " sender <host> <port> <file> [-d]" << std::endl;
		std::cerr << "  Receiver: " << argv[0] << " receiver <port> <file> [-d]" << std::endl;
		return EXIT_FAILURE;
	}

	std::string mode = argv[1];
	bool debug = false;

	for (int i = 2; i < argc; ++i)
	{
		if (std::string(argv[i]) == "-d")
		{
			debug = true;
		}
	}

	if (mode == "sender")
	{
		if (argc < 5)
		{
			std::cerr << "Invalid sender arguments" << std::endl;
			return EXIT_FAILURE;
		}
		std::string host = argv[2];
		int port = std::atoi(argv[3]);
		std::string filename = argv[4];

		RdtSender sender(debug);
		sender.Run(host, port, filename);
	}
	else if (mode == "receiver")
	{
		if (argc < 4)
		{
			std::cerr << "Invalid receiver arguments" << std::endl;
			return EXIT_FAILURE;
		}
		int port = std::atoi(argv[2]);
		std::string filename = argv[3];

		RdtReceiver receiver(debug);
		receiver.Run(port, filename);
	}
	else
	{
		std::cerr << "Unknown mode: " << mode << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}