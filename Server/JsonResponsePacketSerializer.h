#pragma once
#include "Responses.h"
#include "json.hpp"
#include <bitset>
#include <vector>
#include "IRequestHandler.h"
#include <string>

using json = nlohmann::json;

enum class ResponseCode : unsigned char
{
	Movie_Response = 1
};

class JsonResponsePacketSerializer
{
public:
	static Buffer serializeResponse(const GetMovieResponse& reponse);
private:
	static Buffer buildBuffer(unsigned char code, const std::string& jsonStr);
};