#include "FetchUrlAsyncCoro.h"
#include <ranges>
#include <sstream>
#include <iostream>
#include "Asio.h"
#include <boost/url.hpp>
#include <boost/asio/ssl.hpp>

namespace as = boost::asio;
namespace url = boost::urls;
namespace rn = std::ranges;
namespace vi = std::views;
using as::ip::tcp;
using asio_error = boost::system::error_code;


std::vector<Response> FetchUrlAsyncCoro(std::vector<std::string> urls)
{
	as::io_context ioctx;
	tcp::resolver resolver{ ioctx };
	as::ssl::context sslContext{ as::ssl::context::sslv23 };

	auto Strand = [&](const std::string& urlString) -> as::awaitable<Response> {
		Response response{ .url = urlString };
		url::url url = url::url_view{ urlString };
		auto endpIter = co_await resolver.async_resolve(tcp::resolver::query{ url.host(), "https" }, as::use_awaitable);
		as::ssl::stream<tcp::socket> socket{ ioctx, sslContext };
		co_await as::async_connect(socket.lowest_layer(), endpIter, as::use_awaitable);
		co_await socket.async_handshake(as::ssl::stream_base::client, as::use_awaitable);
		{ // send request
			std::ostringstream request_stream;
			request_stream << "GET " << url.path() << " HTTP/1.0\r\n";
			request_stream << "Host: " << url.host() << "\r\n";
			request_stream << "Accept: */*\r\n";
			request_stream << "Connection: close\r\n\r\n";
			co_await as::async_write(socket, as::buffer(request_stream.str()), as::use_awaitable);
		}
		std::ostringstream contentStream;
		{ // receive header
			as::streambuf recvBuf;
			const auto nBytes = co_await as::async_read_until(socket, recvBuf, "\r\n\r\n", as::use_awaitable);
			response.header.resize_and_overwrite(nBytes,
				[&](char* p, size_t) { std::istream{ &recvBuf }.read(p, nBytes); return nBytes; }
			);
			if (recvBuf.size()) {
				contentStream << &recvBuf;
			}
		}
		std::string fixedBuffer(256, ' ');
		while (true) {
			const auto&& [ec, nBytes] = co_await as::async_read(socket, as::buffer(fixedBuffer), as::as_tuple(as::use_awaitable));
			if (ec && ec != as::error::eof) {
				throw boost::system::system_error{ ec };
			}
			fixedBuffer.resize(nBytes);
			contentStream << fixedBuffer;
			if (ec) break;
		}
		response.content = contentStream.str();
		co_return response;
	};

	auto responseFutures = urls | vi::transform([&](auto&& url) { return as::co_spawn(ioctx, Strand(url), as::use_future); }) |
		rn::to<std::vector>();

	ioctx.run();

	return responseFutures | vi::transform(&std::future<Response>::get) | rn::to<std::vector>();
}