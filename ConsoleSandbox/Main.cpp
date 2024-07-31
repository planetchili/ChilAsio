#include <iostream>
#include <Core/src/log/Log.h>
#include "Boot.h"
#include "CliOptions.h"
#include "Asio.h"
#include <boost/url.hpp>
#include <boost/asio/ssl.hpp>
#include <sstream>

using namespace chil;
using namespace std::string_literals;
namespace as = boost::asio;
namespace url = boost::urls;
using as::ip::tcp;
using asio_error = boost::system::error_code;

int main()
{
	if (auto code = opt::Init(false)) {
		return *code;
	}
	Boot();

	try {
		url::url url = url::url_view{ "https://www.boost.org/LICENSE_1_0.txt" };

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

		socket.handshake(as::ssl::stream_base::client);
		std::cout << "Handshake successful!" << std::endl;

		std::string request;
		{
			std::ostringstream request_stream;
			request_stream << "GET " << path << " HTTP/1.0\r\n";
			request_stream << "Host: " << server << "\r\n";
			request_stream << "Accept: */*\r\n";
			request_stream << "Connection: close\r\n\r\n";
			request = request_stream.str();
		}

		as::write(socket, as::buffer(request));
		std::cout << "Request sent!" << std::endl;

		as::streambuf recvBuf;
		std::istream is{ &recvBuf };
		as::read_until(socket, recvBuf, "\r\n\r\n");
		
		{
			std::string headerLine;
			std::cout << "Received header:\n";
			while (std::getline(is, headerLine)) {
				std::cout << headerLine << std::endl;
			}
		}

		std::cout << "Content:\n";
		asio_error ec;
		while (!ec) {
			std::string fixedBuffer(256, ' ');
			const auto nBytes = as::read(socket, as::buffer(fixedBuffer), ec);
			if (ec && ec != as::error::eof) {
				throw boost::system::system_error{ ec };
			}
			fixedBuffer.resize(nBytes);
			std::cout << fixedBuffer << std::endl;
		}

		std::cout << "\n\nTransfer complete!" << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << typeid(e).name() << ">" << e.what();
		return -1;
	}

	return 0;
}