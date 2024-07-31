#include <iostream>
#include <Core/src/log/Log.h>
#include "Boot.h"
#include "CliOptions.h"
#include "Asio.h"
#include <boost/url.hpp>
#include <boost/asio/ssl.hpp>

using namespace chil;
using namespace std::string_literals;
namespace as = boost::asio;
namespace url = boost::urls;
using as::ip::tcp;

int main()
{
	if (auto code = opt::Init(false)) {
		return *code;
	}
	Boot();

	try {
		url::url url = url::url_view{ "https://www.boots69420.org/LICENSE_1_0.txt" };

		as::io_context ioctx;

		tcp::resolver resolver{ ioctx };

		const std::string server = url.host();
		const std::string path = url.path();

		auto endpoints = resolver.resolve(tcp::resolver::query{ server, "https" });
		std::cout << "Resolved url!" << std::endl;

		for (auto& e : endpoints) {
			std::cout << e.endpoint().address().to_string() << std::endl;
		}

		as::ssl::context sslContext{ as::ssl::context::sslv23 };
		as::ssl::stream<tcp::socket> socket{ ioctx, sslContext };

		as::connect(socket.lowest_layer(), endpoints);
		std::cout << "Connected to server!" << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << typeid(e).name() << ">" << e.what();
		return -1;
	}

	return 0;
}