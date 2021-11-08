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

#ifndef GENERIC_LISTENER_SSL_IMPL_HPP
#define GENERIC_LISTENER_SSL_IMPL_HPP

#include "GenericListener.hpp"

template<>
class GenericListenerCore<Streams::SSL> {
public:

	GenericListenerCore(Endpoint endpoint, const char* certChainFile,
			const char* privateKeyFile, const char* dhFile,
			std::string password, boost::system::error_code& err) :
			acceptor(IoContext(), endpoint.TcpEndpoint()),
			sslContext(boost::asio::ssl::context::sslv23) {
		sslContext.set_options(
				boost::asio::ssl::context::default_workarounds
				| boost::asio::ssl::context::no_sslv2
				| boost::asio::ssl::context::single_dh_use);
		sslContext.set_password_callback(
				std::bind([](std::string p){return p;}, password));
		sslContext.use_certificate_chain_file(certChainFile);
		sslContext.use_private_key_file(privateKeyFile,
				boost::asio::ssl::context::pem);
		sslContext.use_tmp_dh_file(dhFile);
		acceptor.listen(256, err);
		for(size_t i=0; i<password.size(); ++i)
			password[i] = ' ';
	}

	~GenericListenerCore() { Close(); }

	inline void Close() {
		acceptor.cancel();
		acceptor.close();
	}

	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ssl::context sslContext;
};



template<>
inline GenericListener<Streams::SSL>::GenericListener(Endpoint endpoint,
		bool enableHeader, boost::system::error_code& err) = delete;

template<>
inline GenericListener<Streams::SSL>::GenericListener(Endpoint endpoint,
		const char* certChainFile, const char* privateKeyFile,
		const char* dhFile, std::string password, bool enableHeader,
		boost::system::error_code& err) :
	enableHeader(enableHeader),
	core(endpoint, certChainFile, privateKeyFile, dhFile, password, err) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, err.message().c_str());
	for(size_t i=0; i<password.size(); ++i)
		password[i] = ' ';
}

template<>
inline GenericListener<Streams::SSL>::~GenericListener() {
}

template<>
inline void GenericListener<Streams::SSL>::InternalAccept(
		const boost::system::error_code& err,
		Streams::TCP asioSocket) {
	if(err) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, err.message().c_str());
		callback.onError(err);
		return;
	}
	boost::system::error_code error;
	GenericSocketSsl *genericSocket =
		new GenericSocketSsl(std::forward<Streams::TCP>(asioSocket),
				core.sslContext, error);
	if(error) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, error.message().c_str());
		callback.onError(error);
		delete genericSocket;
		return;
	}
	Socket* socket = new Socket(genericSocket, enableHeader);
	callback.onAccept(socket);
}

#endif

