#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <unordered_map>
#include <iostream>
#include <exception>
#include <string>
#include <cstring>
#include "IRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "RequestHandlerFactory.h"


constexpr int PORT = 5000;

class Communicator
{
public:
	void startListening();
	Communicator(RequestHandlerFactory& factory);
	~Communicator();
	void startHandleRequests();
private:
	SOCKET m_serverSocket;
	RequestHandlerFactory& m_handlerFactory;

	void bindAndListen();
	void handleNewClient(SOCKET clientSocket);
	char* getPartFromSocket(SOCKET sc, int bytesNum, int flags);
	int getIntPartFromSocket(SOCKET sc, int bytesNum);
};