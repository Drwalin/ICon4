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
inline boost::system::error_code GenericSocket<Streams::SSL>::Connect(
		Endpoint endpoint, bool enableHeader) = delete;

template<>
inline boost::system::error_code GenericSocket<Streams::SSL>::Connect(
		Endpoint endpoint, bool enableHeader, const char* rootCertFile) {
	boost::system::error_code err;
	boost::asio::ssl::context sslContext(boost::asio::ssl::context::sslv23);
	sslContext.load_verify_file(rootCertFile);
	socket = new Streams::SSL(IoContext(), sslContext);
	socket->lowest_layer().connect(endpoint.TcpEndpoint(), err);
	if(err) {
		delete socket;
		socket = NULL;
		return err;
	}
	socket->handshake(boost::asio::ssl::stream_base::client, err);
	if(err) {
		delete socket;
		socket = NULL;
		return err;
	}
	return boost::system::error_code();
}

#endif

