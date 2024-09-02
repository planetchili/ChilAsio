#include <iostream>
#include <Core/src/log/Log.h>
#include "Boot.h"
#include "CliOptions.h"
#include <ranges>
#include <chrono>
#include "FetchUrlSync.h"
#include "FetchUrlAsyncCallbacks.h"

using namespace chil;
namespace rn = std::ranges;
namespace vi = rn::views;
using clk = std::chrono::high_resolution_clock;

int main()
{
	if (auto code = opt::Init(false)) {
		return *code;
	}
	Boot();
	auto& opts = opt::Get();

	try {
		std::vector<Response> responses;
		const auto start = clk::now();
		switch (*opts.mode) {
		case Mode::AsyncCallbacks: responses = FetchUrlsAsyncCallbacks(*opts.url); break;
		default:case Mode::Sync: responses = FetchUrlsSync(*opts.url); break;
		}
		std::cout << std::format("Fetching {} urls took {:%S}s\n\n", opts.url->size(), clk::now() - start);
		for (auto&& [i, r] : vi::enumerate(responses)) {
			std::cout << ">>> URL (" << i << ") [" << r.url << "] <<<\n======= \n%% Header %%\n"
				<< r.header << "%% Content %%\n" << r.content << "\n\n" << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cout << typeid(e).name() << ">" << e.what();
		return -1;
	}

	return 0;
}