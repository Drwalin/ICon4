/*
 *  This file is part of ICon4. Please see README for details.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon4 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef GENERIC_SOCKET_SSL_IMPL_HPP
#define GENERIC_SOCKET_SSL_IMPL_HPP

#include "GenericSocket.hpp"

template<>
inline GenericSocket<Streams::SSL>::GenericSocket(Endpoint endpoint,
		const char* rootCertFile, boost::system::error_code& err) :
	socket(IoContext(), [&]()->boost::asio::ssl::context&{
			static thread_local boost::asio::ssl::context* sslContext = NULL;
			if(sslContext != NULL)
				delete sslContext;
			sslContext = new boost::asio::ssl::context(
						boost::asio::ssl::context::sslv23);
			sslContext->load_verify_file(rootCertFile);
			return *sslContext;
			}()) {
	socket.lowest_layer().connect(endpoint.TcpEndpoint(), err);
	if(err)
		return;
	socket.handshake(boost::asio::ssl::stream_base::client, err);
}

template<>
inline void GenericSocket<Streams::SSL>::Close() {
	socket.lowest_layer().cancel();
}

template<>
GenericSocket<Streams::SSL>::GenericSocket(Streams::TCP&& socket) = delete;

template<>
GenericSocket<Streams::SSL>::GenericSocket(Streams::TCP&& asioSocket,
		boost::asio::ssl::context& sslContext,
		boost::system::error_code& err) : socket(IoContext(), sslContext) {
	socket.next_layer() = std::forward<Streams::TCP>(asioSocket);
	socket.handshake(boost::asio::ssl::stream_base::server, err);
}

#endif

