#pragma once
#include <Core/src/cli/CliFramework.h>
#include <Core/src/log/Level.h>

namespace cli
{
	using namespace ::chil::cli;
	using ::chil::log::Level;

	using MyPair = std::pair<int, std::string>;
	struct Options : public OptionsContainer<Options>
	{
		CHIL_CLI_OPT(logLevel, Level, "Level to log at", Level::Info, cust::EnumMap<Level>());
	private:
		std::string GetDesc() const override { return "Async task processing experiments with boost.ASIO"; };
	};
}

using opt = ::cli::Options;