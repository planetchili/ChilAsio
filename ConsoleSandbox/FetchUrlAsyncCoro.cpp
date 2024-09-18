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
	return {};
}