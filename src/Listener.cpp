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

#ifndef LISTENER_CPP
#define LISTENER_CPP

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/asio/ssl.hpp>
#include <cinttypes>
#include <string>
#include <functional>
#include <utility>

#include "ASIO.hpp"
#include "GenericSocket.hpp"
#include "GenericListener.hpp"

#include "Listener.hpp"

Listener::Listener() {
	ptr = NULL;
	type = Invalid;
}

Listener::~Listener() {
	Close();
}

void Listener::Close() {
	if(ptr) {
		switch(type) {
			case TCP:
				delete tcp;
				break;
			case SSL:
				delete ssl;
				break;
			default:
				break;
		}
		ptr = NULL;
		type = Invalid;
	}
}

void Listener::StopListening() {
	if(ptr) {
		switch(type) {
			case TCP:
				tcp->core.acceptor.cancel();
				break;
			case SSL:
				ssl->core.acceptor.cancel();
				break;
			default:
				break;
		}
	}
}

void Listener::StartAccepting(Endpoint endpoint,
		std::function<bool(Socket*)> function, bool enableHeader,
		boost::system::error_code& err) {
	Close();
	printf(" %s() %s:%i\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);
	tcp = new GenericListenerTcp(endpoint, enableHeader, err);
	if(err) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, err.message().c_str());
		delete tcp;
		ptr = NULL;
		return;
	}
	type = TCP;
	tcp->StartListening(function);
}

void Listener::StartAccepting(Endpoint endpoint, const char* certChainFile,
		const char* privateKeyFile, const char* dhFile, std::string password,
		std::function<bool(Socket*)> function, bool enableHeader,
		boost::system::error_code& err) {
	Close();
	ssl = new GenericListenerSsl(endpoint, certChainFile, privateKeyFile,
			dhFile, password, enableHeader, err);
	if(err) {
	printf(" %s() %s:%i     error(%s)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, err.message().c_str());
		delete ssl;
		ptr = NULL;
		for(size_t i=0; i<password.size(); ++i)
			password[i] = ' ';
		return;
	}
	type = SSL;
	ssl->StartListening(function);
}

void Listener::SetOnError(
		std::function<void(const boost::system::error_code&)> function) {
	if(ptr) {
		switch(type) {
			case TCP:
				tcp->SetOnError(function);
				break;
			case SSL:
				ssl->SetOnError(function);
				break;
			default:
				break;
		}
	}
}

#endif

