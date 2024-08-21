#pragma once
#define BOOST_ASIO_SEPARATE_COMPILATION
#include <Core/src/win/ChilWin.h>
#pragma warning(push)
#pragma warning(disable : 26436 26433 26495 26451 6255 6387 6031 26819 6258 6001 26498 26439 26437 6294 6201)
#include <boost/asio.hpp>
#include <boost/url.hpp>
#include <boost/asio/ssl.hpp>
#pragma warning(pop)