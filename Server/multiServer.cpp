#include "multiServer.h"


Server::Server() : m_communicator(m_factory)
{
}

Server::~Server()
{

}

void Server::run()
{
	m_communicator.startListening();
	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "Waiting for client connection request" << std::endl;
		m_communicator.startHandleRequests();
	}
}