#pragma once
#include <vector>
#include <string>
#include "Response.h"

std::vector<Response> FetchUrlAsyncCoro(std::vector<std::string> urls);