#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <set>
#include <map>
#include <thread>
#include <iostream>
#include "Communicator.h"
#include "RequestHandlerFactory.h"

class Server
{
public:
	Server();
	~Server();
	void run();

private:
	RequestHandlerFactory m_factory;
	Communicator m_communicator;
};
