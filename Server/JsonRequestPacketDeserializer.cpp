#include "JsonRequestPacketDeserializer.h"

GetMovieRequest JsonRequestPacketDeserializer::deserializeGetMovieRequest(const Buffer& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	json j = json::parse(jsonStr);
	GetMovieRequest request;
	request.movieName = j["movieName"];
	return request;
}
