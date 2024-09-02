#include "FetchUrlAsyncCallbacks.h"
#include <ranges>
#include <sstream>
#include <iostream>
#include "Asio.h"
#include <boost/url.hpp>
#include <boost/asio/ssl.hpp>

namespace as = boost::asio;
namespace url = boost::urls;
namespace rn = std::ranges;
namespace vi = rn::views;
using as::ip::tcp;
using asio_error = boost::system::error_code;


std::vector<Response> FetchUrlsAsyncCallbacks(std::vector<std::string> urls)
{
	std::vector<Response> responses(urls.size());

	as::io_context ioctx;
	tcp::resolver resolver{ ioctx };
	as::ssl::context sslContext{ as::ssl::context::sslv23 };
	std::string fail;

	for (auto&& [urlString, response] : vi::zip(urls, responses)) {
		response.url = urlString;
		url::url url = url::url_view{ urlString };

		// resolve and connect
		resolver.async_resolve(tcp::resolver::query{ url.host(), "https" },
			[&](const asio_error& err, tcp::resolver::iterator endpIter) {
				if (!err) {
					auto pSocket = std::make_shared<as::ssl::stream<tcp::socket>>(ioctx, sslContext);
					as::async_connect(pSocket->lowest_layer(), endpIter,
						[&, pSocket](const asio_error& err, tcp::resolver::iterator) {
							if (!err) {
								pSocket->async_handshake(as::ssl::stream_base::client, 
									[&, pSocket](const asio_error& err) {
										if (!err) {
											// send GET request
											url::url url = url::url_view{ urlString };
											std::ostringstream request_stream;
											request_stream << "GET " << url.path() << " HTTP/1.0\r\n";
											request_stream << "Host: " << url.host() << "\r\n";
											request_stream << "Accept: */*\r\n";
											request_stream << "Connection: close\r\n\r\n";
											as::async_write(*pSocket, as::buffer(request_stream.str()),
												[&, pSocket](const asio_error& err, std::size_t) {
													if (!err) {
														auto pRecvBuf = std::make_shared<as::streambuf>();
														as::async_read_until(*pSocket, *pRecvBuf, "\r\n\r\n",
															[&, pSocket, pRecvBuf](const asio_error& err, std::size_t nBytes) {
																if (!err) {
																	response.header.resize_and_overwrite(nBytes,
																		[&](char* p, size_t) { std::istream{ pRecvBuf.get() }.read(p, nBytes); return nBytes; }
																	);

																	struct ReadContext {
																		std::shared_ptr<as::ssl::stream<tcp::socket>> pSocket;
																		std::ostringstream contentStream;
																		std::string fixedBuffer = std::string(256, ' ');
																		std::function<void(const asio_error&, std::size_t)> ReadSome;
																	};
																	auto pReadCtx = std::make_shared<ReadContext>();
																	pReadCtx->pSocket = pSocket;

																	if (pRecvBuf->size() > 0) {
																		pReadCtx->contentStream << pRecvBuf.get();
																	}

																	pReadCtx->ReadSome = [&, pReadCtx](const asio_error& err, std::size_t nBytes) {
																		if (err && err != as::error::eof) {
																			fail = err.what();
																			ioctx.stop();
																		}
																		pReadCtx->fixedBuffer.resize(nBytes);
																		pReadCtx->contentStream << pReadCtx->fixedBuffer;
																		if (!err) {
																			as::async_read(*pReadCtx->pSocket, as::buffer(pReadCtx->fixedBuffer), pReadCtx->ReadSome);
																		}
																		else {
																			response.content = pReadCtx->contentStream.str();
																		}
																	};

																	as::async_read(*pReadCtx->pSocket, as::buffer(pReadCtx->fixedBuffer), pReadCtx->ReadSome);
																}
																else {
																	fail = err.what();
																	ioctx.stop();
																}
															}
														);
													}
													else {
														fail = err.what();
														ioctx.stop();
													}
												}
											);
										}
										else {
											fail = err.what();
											ioctx.stop();
										}
									}
								);
							}
							else {
								fail = err.what();
								ioctx.stop();
							}
						}
					);
				}
				else {
					fail = err.what();
					ioctx.stop();
				}
			}
		);
	}

	ioctx.run();

	if (!fail.empty()) {
		throw std::runtime_error(fail);
	}

	return responses;
}