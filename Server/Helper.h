#pragma once
#include "IRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "Responses.h"

class Helper
{
public:
	template <typename T>
	static RequestResult makeResult(const T& response)
	{
		RequestResult result;
		result.buffer = JsonResponsePacketSerializer::serializeResponse(response);
		return result;
	}
};