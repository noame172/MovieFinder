#pragma once
#include "MovieRequestHandler.h"
#include <memory>

class RequestHandlerFactory
{
public:
	RequestHandlerFactory();
	std::unique_ptr<IRequestHandler> createMovieRequestHandler();
};