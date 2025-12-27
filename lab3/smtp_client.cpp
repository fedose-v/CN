#include "smtp/UserInterface.h"

int main()
{
	auto ui = UserInterface(std::cin, std::cout);
	ui.Run();

	return EXIT_SUCCESS;
}