#include "MovieRequestHandler.h"

bool MovieRequestHandler::isRequestRelevent(const RequestInfo& requestInfo)
{
    return requestInfo.id == int(ResponseCode::Movie_Response);
}

RequestResult MovieRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
    if(!isRequestRelevent(requestInfo))
    {
		return RequestResult();
	}

    switch (static_cast<ResponseCode>(requestInfo.id)) {
        case ResponseCode::Movie_Response:
            return getMovie(requestInfo);
    }
}

size_t MovieRequestHandler::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

RequestResult MovieRequestHandler::getMovie(const RequestInfo& requestInfo)
{
    // Step 1: Securely validate and load the API key from environment variables
    const char* apiKeyEnv = std::getenv("TMDB_API_KEY");
    if (apiKeyEnv == nullptr) {
        std::cerr << "Error: TMDB_API_KEY environment variable not set." << std::endl;
        return RequestResult();
    }

    // Create the Bearer token header for authentication
    std::string auth_header = "Authorization: Bearer " + std::string(apiKeyEnv);

    // Deserialize the incoming binary/text buffer into a structured C++ request object
    GetMovieRequest request = JsonRequestPacketDeserializer::deserializeGetMovieRequest(requestInfo.buffer);

    // Step 2: Initialize libcurl and shared HTTP parameters
    CURL* hnd = curl_easy_init();
    if (!hnd) {
        std::cerr << "Error initializing CURL" << std::endl;
        return RequestResult();
    }

    std::string readBuffer; // Buffer to temporarily store JSON payloads received from the server
    struct curl_slist* headers = NULL;

    // Append required HTTP headers
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());

    // PHASE A: Search for the movie by name to retrieve its unique TMDB ID

    // URL-encode the movie name to safely handle spaces and special characters (e.g., "The Matrix" -> "The%20Matrix")
    char* encoded_name = curl_easy_escape(hnd, request.movieName.c_str(), request.movieName.length());
    std::string safe_movie_name(encoded_name);
    curl_free(encoded_name); // Free the temporary binding memory allocated by curl

    // Construct the search endpoint URL
    std::string search_url = "https://api.themoviedb.org/3/search/movie?query=" + safe_movie_name + "&language=en-US";

    // Configure CURL options for the search request
    curl_easy_setopt(hnd, CURLOPT_URL, search_url.c_str());
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback); // Callback to handle incoming data stream
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &readBuffer);       // Pass our string buffer as the target data sink
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    // Execute the synchronous HTTP GET request for the search
    CURLcode res = curl_easy_perform(hnd);
    std::string movie_id = "";

    if (res != CURLE_OK) {
        std::cerr << "Search request failed: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);
        return RequestResult();
    }

    // Parse the search JSON results to extract the highest-matching movie's ID
    try {
        auto searchJson = nlohmann::json::parse(readBuffer);
        if (!searchJson["results"].empty()) {
            // Pick the very first result (index 0) as it represents the most relevant match
			std::cout << "Found movie: " << searchJson["results"][0]["title"] << " with ID: " << searchJson["results"][0]["id"] << std::endl;
            int id = searchJson["results"][0]["id"];
            movie_id = std::to_string(id);
        }
        else {
            std::cerr << "No movie found with name: " << request.movieName << std::endl;
			std::cout << "Search JSON payload: " << readBuffer << std::endl;
            curl_slist_free_all(headers);
            curl_easy_cleanup(hnd);
            return RequestResult();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "JSON Parsing error during search phase: " << e.what() << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);
        return RequestResult();
    }

    // PHASE B: Fetch comprehensive movie metadata using the extracted ID

    // Clear the string buffer so it can accept the new movie details payload
    readBuffer.clear();

    // Construct the explicit details endpoint URL
    std::string movie_url = "https://api.themoviedb.org/3/movie/" + movie_id + "?language=en-US";

    // Reuse the existing CURL handle, only overriding the target URL destination
    curl_easy_setopt(hnd, CURLOPT_URL, movie_url.c_str());

    // Execute the secondary synchronous HTTP GET request
    res = curl_easy_perform(hnd);

	GetMovieResponse response;
    if (res != CURLE_OK) {
        std::cerr << "Fetch movie details failed: " << curl_easy_strerror(res) << std::endl;
    }
    else {
        std::cout << "Successfully fetched full movie data for ID: " << movie_id << std::endl;

        try {
            // At this point, 'readBuffer' contains the final, complete JSON layout of the movie
            auto movieDetailsJson = nlohmann::json::parse(readBuffer);
			std::vector<std::string> genres;
            for(auto& genre : movieDetailsJson["genres"]) {
                genres.push_back(genre["name"]);
			}
			response.status = 1;
            response.movie = MovieData(
                movieDetailsJson["title"],
                movieDetailsJson["runtime"],
                genres,
                movieDetailsJson["overview"]
			);
        }
        catch (const std::exception& e) {
            std::cerr << "JSON Parsing error during details phase: " << e.what() << std::endl;
        }
    }

    // Step 3: Cleanup allocations to ensure zero memory leaks
    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);

    // Step 4: Serialize response packet and return the result state
    RequestResult result;
	result.buffer = JsonResponsePacketSerializer::serializeResponse(response);
    result.newHandler = nullptr;
    return result;
}
