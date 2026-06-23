#include "RequestHandlerFactory.h"

RequestHandlerFactory::RequestHandlerFactory() {}

std::unique_ptr<IRequestHandler> RequestHandlerFactory::createMovieRequestHandler()
{
	return std::make_unique<MovieRequestHandler>();
}
