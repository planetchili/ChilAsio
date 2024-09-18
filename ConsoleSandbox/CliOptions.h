#pragma once
#include <Core/src/cli/CliFramework.h>
#include <Core/src/log/Level.h>

enum class Mode
{
	Sync,
	AsyncCallbacks,
	AsyncCoro,
};

namespace cli
{
	using namespace ::chil::cli;
	using ::chil::log::Level;

	struct Options : public OptionsContainer<Options>
	{
		CHIL_CLI_OPT(logLevel, Level, "Level to log at", Level::Info, cust::EnumMap<Level>());
		CHIL_CLI_OPT(url, std::vector<std::string>, "URL to fetch");
		CHIL_CLI_OPT(mode, Mode, "Mode to use for fetching", Mode::Sync, cust::EnumMap<Mode>());
	private:
		std::string GetDesc() const override { return "Async task processing experiments with boost.ASIO"; };
	};
}

using opt = ::cli::Options;