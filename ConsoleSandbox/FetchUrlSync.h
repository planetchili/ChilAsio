#pragma once
#include <vector>
#include <string>
#include "Response.h"

std::vector<Response> FetchUrlsSync(std::vector<std::string> urls);