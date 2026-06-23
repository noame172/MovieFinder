#include "multiServer.h"
#include "WSAInitializer.h"

int main()
{
	try
	{
		WSAInitializer wsaInit;
		Server server;
		server.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}
	return 0;
}