#define BOOST_ASIO_SEPARATE_COMPILATION
#define ZC_FULL_WINTARD
#include <Core/src/win/ChilWin.h>
#pragma warning(push)
#pragma warning(disable : 26436 26433 26495 26451 6255 6387 6031 26819 6258 6001 26498 26439 26437)
#include <boost/asio/impl/src.hpp>
#include <boost/asio/ssl/impl/src.hpp>
#pragma warning(pop)