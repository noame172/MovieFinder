#include "Communicator.h"

Communicator::Communicator(RequestHandlerFactory& factory) : m_handlerFactory(factory)
{
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverSocket == INVALID_SOCKET)
		throw std::exception("Failed to create socket");
}

Communicator::~Communicator() {}

void Communicator::startHandleRequests()
{
	SOCKET client_socket = accept(m_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::cout << "Client accepted. Server and client can speak" << std::endl;
	// the function that handle the conversation with the client
	handleNewClient(client_socket);
}


void Communicator::bindAndListen()
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::cout << "Listening on port " << PORT << std::endl;
}

void Communicator::startListening()
{
	bindAndListen();
}

void Communicator::handleNewClient(SOCKET clientSocket)
{

	try
	{
		while (true)
		{
			unsigned char code = (unsigned char)getIntPartFromSocket(clientSocket, 1);
			int size = getIntPartFromSocket(clientSocket, 4);


			char* s = getPartFromSocket(clientSocket, size, 0);
			Buffer buffer(s, s + size);
			delete[] s;

			RequestInfo requestInfo{ (char)code, time(NULL), buffer };
			RequestResult result = m_handlerFactory.createMovieRequestHandler()->handleRequest(requestInfo);
			if (result.newHandler != nullptr)
			{
				m_handlerFactory.createMovieRequestHandler().reset(result.newHandler);
			}
			send(clientSocket, (const char*)result.buffer.data(), result.buffer.size(), 0);

		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error in client handler: " << e.what() << std::endl;
	}
	closesocket(clientSocket);
}

char* Communicator::getPartFromSocket(SOCKET sc, int bytesNum, int flags)
{
	if (bytesNum == 0)
	{
		return (char*)"";
	}

	char* data = new char[bytesNum + 1];
	int res = recv(sc, data, bytesNum, flags);

	if (res == INVALID_SOCKET)
	{
		std::string s = "Error while recieving from socket: ";
		s += std::to_string(sc);
		throw std::exception(s.c_str());
	}

	data[bytesNum] = 0;
	return data;
}

int Communicator::getIntPartFromSocket(SOCKET sc, int bytesNum)
{
	char* data = getPartFromSocket(sc, bytesNum, 0);
	int result = 0;
	for (int i = 0; i < bytesNum; i++)
	{
		result = (result << 8) | (unsigned char)data[i];
	}
	if (bytesNum > 0) delete[] data;
	return result;
}