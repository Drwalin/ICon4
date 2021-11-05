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

#include <boost/asio.hpp>

#include "GenericSocket.hpp"
#include "Socket.hpp"

Socket::Socket(Endpoint endpoint, bool enableHeader, Type type) {
	userPtr = NULL;
	received = 0;
	header = 0;
	this->type = type;
	this->enableHeader = enableHeader;
	ptr = NULL;
	boost::system::error_code err;
	switch(type) {
		case UDP:
			udp = new GenericSocketUdp(endpoint, err);
			if(err) {
				delete udp;
				udp = NULL;
				return;
			}
			break;
		case TCP:
			tcp = new GenericSocketTcp(endpoint, err);
			if(err) {
				delete tcp;
				tcp = NULL;
				return;
			}
			break;
		case SSL:
		default:
			type = Invalid;
			return;
	}
	this->type = type;
	this->endpoint = endpoint;
}

Socket::Socket(Endpoint endpoint, bool enableHeader, const char* rootCertFile) {
	userPtr = NULL;
	received = 0;
	header = 0;
	this->enableHeader = enableHeader;
	boost::system::error_code err;
	ssl = new GenericSocketSsl(endpoint, rootCertFile, err);
	if(err) {
		delete ssl;
		ssl = NULL;
		return;
	}
	type = SSL;
	this->endpoint = endpoint;
}

Socket::~Socket() {
	if(ptr) {
		switch(type) {
		case UDP: delete udp; break;
		case TCP: delete tcp; break;
		case SSL: delete ssl; break;
		case Invalid: break;
		}
		ptr = NULL;
	}
}

bool Socket::Send(const void* buffer, size_t bytes) {
	return false;
}

void Socket::QueueFetch() {
}

void Socket::SetOnError(
		void(*function)(Socket*, const boost::system::error_code&)) {
	callback.onError = function;
}

void Socket::SetOnReceive(void(*function)(Socket*, void*, size_t)) {
	if(callback.onReceive == NULL) {
		callback.onReceive = function;
		if(ptr) {
			callback.onReceiveFunc = std::bind(
					enableHeader ? &Socket::InternalOnReceiveWithHeader :
					&Socket::InternalOnReceiveWithoutHeader, this,
					std::placeholders::_1, std::placeholders::_2);
			RecallOnReceive();
			if(enableHeader) {
				fprintf(stderr, " Not implemented: %s:%d\n", __FILE__,
						__LINE__);
				fflush(stderr);
			} else {
				header = maxSingleBuffer;
				buffer.resize(header);
				received = 0;
			}
		}
	}
}

void Socket::RecallOnReceive() {
	switch(type) {
		case UDP:
			udp->QueueFetch(buffer, received, callback.onReceiveFunc);
			break;
		case TCP:
			tcp->QueueFetch(buffer, received, callback.onReceiveFunc);
			break;
		case SSL:
			ssl->QueueFetch(buffer, received, callback.onReceiveFunc);
			break;
		case Invalid:
		default:
			break;
	}
}

void Socket::InternalOnReceiveWithHeader(const boost::system::error_code& err,
		size_t bytes) {
	fprintf(stderr, " Not implemented: %s:%d\n", __FILE__, __LINE__);
	fflush(stderr);
}

void Socket::InternalOnReceiveWithoutHeader(const boost::system::error_code& err,
		size_t bytes) {
	if(err) {
		if(callback.onError)
			callback.onError(this, err);
	} else {
		callback.onReceive(this, &(buffer[0]), bytes);
		RecallOnReceive();
	}
}

