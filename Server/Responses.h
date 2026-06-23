#pragma once
#include <string>
#include "MovieData.h"

struct GetMovieResponse
{
	unsigned int status;
	MovieData movie;
};