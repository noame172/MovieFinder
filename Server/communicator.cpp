#include "communicator.h"

Communicator::Communicator(RequestHandlerFactory& factory)
	: m_handlerFactory(factory)
{

	// this Communicator use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the Communicator use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

Communicator::~Communicator()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(m_serverSocket);
	}
	catch (...) {}
}

void Communicator::startListening()
{
	bindAndListen();
}

void Communicator::bindAndListen()
{

	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(PORT); // port that Communicator will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::cout << "Listening on port " << PORT << std::endl;

	while (true)
	{
		startHandleRequests();
	}
}

void Communicator::handleNewClient(SOCKET clientSocket)
{
	try {

		while (true) {
			unsigned char code = (unsigned char)getIntPartFromSocket(clientSocket, 1);
			int size = getIntPartFromSocket(clientSocket, 4);

			char* data = getPartFromSocket(clientSocket, size, 0);
			Buffer buffer(data, data + size);
			delete data;

			std::unique_ptr<IRequestHandler> handler = m_handlerFactory.createMovieRequestHandler();
			RequestInfo requestInfo{ char(code), time(nullptr), buffer };
			RequestResult result = handler->handleRequest(requestInfo);
			if (result.newHandler != nullptr) {
				handler.reset();
			}
			send(clientSocket, (const char*)result.buffer.data(), result.buffer.size(), 0);
		}
	}
	catch (const std::exception& ex) {
		std::cerr << "Error while handling client: " << ex.what() << std::endl;
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
	return atoi(data);
}

void Communicator::startHandleRequests()
{

	// this accepts the client and create a specific socket from Communicator to this client
	// the process will not continue until a client connects to the Communicator
	SOCKET client_socket = accept(m_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::cout << "Client accepted. Communicator and client can speak" << std::endl;
	// the function that handle the conversation with the client
	handleNewClient(client_socket);
}