#include "JsonresponsePacketSerializer.h"

Buffer JsonResponsePacketSerializer::buildBuffer(unsigned char code, const std::string& jsonStr)
{
    Buffer buffer;
    buffer.push_back(code);
    int size = jsonStr.size();
    buffer.push_back((size >> 24) & 0xFF);
    buffer.push_back((size >> 16) & 0xFF);
    buffer.push_back((size >> 8) & 0xFF);
    buffer.push_back(size & 0xFF);
    buffer.insert(buffer.end(), jsonStr.begin(), jsonStr.end());
    return buffer;
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetMovieResponse& response)
{
    json r;
	r["title"] = response.movie.getTitle();
	r["runTimeInMinutes"] = response.movie.getRunTimeInMinutes();
	r["genres"] = response.movie.getGenres();
	r["overview"] = response.movie.getOverview();
	r["director"] = response.movie.getDirector();
    json j = { {"status", response.status}, {"movie", r} };
    return buildBuffer((unsigned char)ResponseCode::Movie_Response, j.dump());
}
