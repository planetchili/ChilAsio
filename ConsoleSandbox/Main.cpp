#include <iostream>
#include <Core/src/log/Log.h>
#include "Boot.h"
#include "CliOptions.h"
#include "Asio.h"

using namespace chil;
using namespace std::string_literals;

int main()
{
	if (auto code = opt::Init(false)) {
		return *code;
	}
	Boot();

	chilog.info(L"Test info message");

	return 0;
}