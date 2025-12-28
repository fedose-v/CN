#include "dns/DnsResolver.h"
#include "utils/Logger.h"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <domain> <type> [-d]" << std::endl;
		return 1;
	}

	std::string domain = argv[1];
	std::string type = argv[2];

	for (int i = 3; i < argc; ++i)
	{
		if (std::string(argv[i]) == "-d")
		{
			Logger::SetDebug(true);
		}
	}

	DnsResolver resolver;
	resolver.Resolve(domain, type);

	return EXIT_SUCCESS;
}