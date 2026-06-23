#pragma once
#include "Requests.h"
#include "json.hpp"
#include "IRequestHandler.h"
using json = nlohmann::json;

class JsonRequestPacketDeserializer
{
public:
	static GetMovieRequest deserializeGetMovieRequest(const Buffer& buffer);
};