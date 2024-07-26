#include <iostream>
#include <Core/src/log/Log.h>
#include <Core/src/ioc/Container.h>
#include <Core/src/log/SeverityLevelPolicy.h>
#include "CliOptions.h"


using namespace chil;
using namespace std::string_literals;

void Boot()
{
	log::Boot();

	ioc::Get().Register<log::ISeverityLevelPolicy>([] {
		return std::make_shared<log::SeverityLevelPolicy>(*opt::Get().logLevel);
	});
}

int main()
{
	if (auto code = opt::Init(false)) {
		return *code;
	}
	Boot();

	chilog.info(L"Test info message");

	return 0;
}