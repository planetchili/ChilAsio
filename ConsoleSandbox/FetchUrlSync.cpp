#include "FetchUrlSync.h"
#include <ranges>
#include <sstream>
#include "Asio.h"
#include <boost/url.hpp>
#include <boost/asio/ssl.hpp>

namespace as = boost::asio;
namespace url = boost::urls;
namespace rn = std::ranges;
namespace vi = rn::views;
using as::ip::tcp;
using asio_error = boost::system::error_code;


std::vector<Response> FetchUrlsSync(std::vector<std::string> urls)
{
	std::vector<Response> responses;

	as::io_context ioctx;
	tcp::resolver resolver{ ioctx };
	as::ssl::context sslContext{ as::ssl::context::sslv23 };

	for (auto& urlString : urls) {
		Response response{ .url = urlString };
		url::url url = url::url_view{ urlString };

		const std::string server = url.host();
		const std::string path = url.path();

		// resolve and connect
		auto endpoints = resolver.resolve(tcp::resolver::query{ server, "https" });
		as::ssl::stream<tcp::socket> socket{ ioctx, sslContext };
		as::connect(socket.lowest_layer(), endpoints);
		socket.handshake(as::ssl::stream_base::client);

		// send GET request
		{
			std::ostringstream request_stream;
			request_stream << "GET " << path << " HTTP/1.0\r\n";
			request_stream << "Host: " << server << "\r\n";
			request_stream << "Accept: */*\r\n";
			request_stream << "Connection: close\r\n\r\n";
			const auto request = request_stream.str();
			as::write(socket, as::buffer(request));
		}

		as::streambuf recvBuf;

		// receive header and store
		response.header.resize_and_overwrite(
			as::read_until(socket, recvBuf, "\r\n\r\n"),
			[&](char* p, size_t nBytes) { std::istream{ &recvBuf }.read(p, nBytes); return nBytes; }
		);

		// receive content and store
		std::ostringstream contentStream;
		contentStream << &recvBuf;

		asio_error ec;
		std::string fixedBuffer(256, ' ');
		while (!ec) {
			const auto nBytes = as::read(socket, as::buffer(fixedBuffer), ec);
			if (ec && ec != as::error::eof) {
				throw boost::system::system_error{ ec };
			}
			fixedBuffer.resize(nBytes);
			contentStream << fixedBuffer;
		}

		response.content = contentStream.str();
		responses.push_back(std::move(response));
	}
	return responses;
}