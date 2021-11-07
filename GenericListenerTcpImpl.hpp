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

#ifndef GENERIC_LISTENER_TCP_IMPL_HPP
#define GENERIC_LISTENER_TCP_IMPL_HPP

#include "GenericListener.hpp"

template<>
class GenericListenerCore<Streams::TCP> {
public:
	
	GenericListenerCore(Endpoint endpoint, boost::system::error_code& err) :
			acceptor(IoContext(), endpoint.TcpEndpoint()) {
		acceptor.listen(256, err);
	}

	~GenericListenerCore() { Close(); }

	inline void Close() {
		acceptor.cancel();
		acceptor.close();
	}

	boost::asio::ip::tcp::acceptor acceptor;
};



template<>
GenericListener<Streams::TCP>::GenericListener(Endpoint endpoint,
		boost::system::error_code& err) :
	core(endpoint, err) {
}

template<>
GenericListener<Streams::TCP>::GenericListener(Endpoint endpoint,
		const char* certChainFile, const char* privateKeyFile,
		const char* dhFile, std::string password,
		boost::system::error_code& err) = delete;

template<>
GenericListener<Streams::TCP>::~GenericListener() {
}



template<>
inline void GenericListener<Streams::TCP>::InternalAccept(
		const boost::system::error_code& err,
		Streams::TCP&& asioSocket) {
	GenericSocketTcp *genericSocket =
		new GenericSocketTcp(std::forward<Streams::TCP>(asioSocket));
	Socket* socket = new Socket(genericSocket);
	callback.onAccept(socket);
}

#endif

