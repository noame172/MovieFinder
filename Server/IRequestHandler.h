#pragma once
#include <time.h>
#include <vector>
using Buffer = std::vector<unsigned char>;
class IRequestHandler;

struct RequestInfo {
	char id;
	time_t receivalTime;
	Buffer buffer;
};

struct RequestResult
{
	Buffer buffer;
	IRequestHandler* newHandler = nullptr;
};
class IRequestHandler
{
public:
	IRequestHandler() = default;
	virtual ~IRequestHandler() = default;
	virtual bool isRequestRelevent(const RequestInfo& requestInfo) = 0;
	virtual RequestResult handleRequest(const RequestInfo& requestInfo) = 0;
};