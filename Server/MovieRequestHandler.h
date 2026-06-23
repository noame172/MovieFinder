#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "IRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include <iostream>
#include <cstdlib>
#include <curl/curl.h>

class MovieRequestHandler : public IRequestHandler
{
public:
	MovieRequestHandler() = default;
	virtual ~MovieRequestHandler() = default;
	bool isRequestRelevent(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;
	RequestResult getMovie(const RequestInfo& requestInfo);
private:
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};