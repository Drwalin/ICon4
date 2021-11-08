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
		printf(" GenericListener::Core::TCP::Close()\n");
		fflush(stdout);
		acceptor.cancel();
		acceptor.close();
	}

	boost::asio::ip::tcp::acceptor acceptor;
};



template<>
inline GenericListener<Streams::TCP>::GenericListener(Endpoint endpoint,
		bool enableHeader, boost::system::error_code& err) :
	enableHeader(enableHeader), core(endpoint, err) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, err.message().c_str());
}

template<>
inline GenericListener<Streams::TCP>::GenericListener(Endpoint endpoint,
		const char* certChainFile, const char* privateKeyFile,
		const char* dhFile, std::string password, bool enableHeader,
		boost::system::error_code& err) = delete;

template<>
inline GenericListener<Streams::TCP>::~GenericListener() {
}

template<>
inline void GenericListener<Streams::TCP>::InternalAccept(
		const boost::system::error_code& err,
		Streams::TCP asioSocket) {
	printf(" Internal Accept in GenericListener %s:%i\n", __FILE__, __LINE__);
	if(err) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, err.message().c_str());
		callback.onError(err);
		return;
	}
	GenericSocketTcp *genericSocket =
		new GenericSocketTcp(std::forward<Streams::TCP>(asioSocket));
	Socket* socket = new Socket(genericSocket, enableHeader);
	printf(" %s() %s:%i\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);
	if(callback.onAccept(socket) == false) {
	printf(" %s() %s:%i     invalidating socket\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);
		delete socket;
	}
}

#endif

