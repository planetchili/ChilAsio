#include <iostream>
#include <Core/src/log/Log.h>
#include "Boot.h"
#include "CliOptions.h"
#include <ranges>
#include "FetchUrlSync.h"

using namespace chil;
namespace rn = std::ranges;
namespace vi = rn::views;

int main()
{
	if (auto code = opt::Init(false)) {
		return *code;
	}
	Boot();
	auto& opts = opt::Get();

	try {
		const auto responses = FetchUrlsSync(*opts.url);
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