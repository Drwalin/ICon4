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

#ifndef SOCKET_CPP
#define SOCKET_CPP

#include <boost/asio.hpp>

#include "GenericSocket.hpp"
#include "Socket.hpp"

struct NetByteOrder {
	uint8_t b[2];
	
	inline void Set(uint16_t value) {
		b[0] = value&0xFF;
		b[1] = (value>>8)&0xFF;
	}
	
	inline uint16_t Get() const {
		return b[0] | ((uint16_t)b[1])<<8;
	}
	
	inline static size_t Read(const void* buffer) {
		return ((const NetByteOrder*)buffer)->Get();
	}
};

Socket::Socket(GenericSocketTcp* tcp, bool enableHeader) {
	this->tcp = tcp;
	type = TCP;
	userPtr = NULL;
	received = 0;
	header = 0;
	this->enableHeader = enableHeader;
	endpoint = tcp->socket.remote_endpoint();
}

Socket::Socket(GenericSocketSsl* ssl, bool enableHeader) {
	this->ssl = ssl;
	type = SSL;
	userPtr = NULL;
	received = 0;
	header = 0;
	this->enableHeader = enableHeader;
	endpoint = ssl->socket.next_layer().remote_endpoint();
}

Socket::Socket(Endpoint endpoint, boost::system::error_code& err,
		bool enableHeader, Type type) {
	userPtr = NULL;
	received = 0;
	header = 0;
	this->type = Invalid;
	this->enableHeader = false;
	ptr = NULL;
	if(type == TCP) {
		InternalCreate(endpoint, err, enableHeader, TCP);
	} else {
		err = boost::asio::error::bad_descriptor;
	}
}

Socket::Socket(Endpoint endpoint, boost::system::error_code& err, Type type) {
	userPtr = NULL;
	received = 0;
	header = 0;
	this->type = Invalid;
	this->enableHeader = false;
	ptr = NULL;
	if(type == UDP) {
		InternalCreate(endpoint, err, enableHeader, UDP);
	} else {
		err = boost::asio::error::bad_descriptor;
	}
}

void Socket::InternalCreate(Endpoint endpoint, boost::system::error_code& err,
	bool enableHeader, Type type) {
	userPtr = NULL;
	received = 0;
	header = 0;
	this->type = type;
	this->enableHeader = enableHeader;
	ptr = NULL;
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

Socket::Socket(Endpoint endpoint, boost::system::error_code& err,
		bool enableHeader, const char* rootCertFile) {
	userPtr = NULL;
	received = 0;
	header = 0;
	this->enableHeader = enableHeader;
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
	printf(" Socket::Send(void*, %lu)\n", bytes);
	fflush(stdout);
	if(buffer==NULL || bytes==0)
		return false;
	if(enableHeader) {
		if(bytes > maxSingleBuffer)
			return false;
		NetByteOrder size;
		size.Set(bytes-1);
		InternalSend(size.b, 2);
	}
	return InternalSend(buffer, bytes);
}

inline bool Socket::InternalSend(const void* buffer, size_t bytes) {
	boost::system::error_code err;
	switch(type) {
		case UDP:
			err = udp->Send(buffer, bytes);
			break;
		case TCP:
			err = tcp->Send(buffer, bytes);
			break;
		case SSL:
			err = ssl->Send(buffer, bytes);
			break;
		case Invalid:
		default:
			break;
	}
	if(err) {
		callback.onError(this, err);
		return false;
	}
	return true;
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
				received = 0;
				header = 0;
				buffer.resize(2);
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
	fprintf(stderr, " on receive with header: %lu b\n", bytes);
	fflush(stderr);
	if(err || bytes==0) {
		if(callback.onError)
			callback.onError(this, err);
	} else {
		if(received==header && header) {
			callback.onReceive(this, &(buffer[2]), buffer.size()-2);
			received = 0;
			header = 0;
			buffer.resize(2);
		} else {
			if(received==2 && header==0 && buffer.size()==2) {
				header = NetByteOrder::Read(buffer.data())+3;
				buffer.resize(header);
			} else if(received < 2) {
				buffer.resize(2);
				header = 0;
			}
		}
	}
	RecallOnReceive();
}

void Socket::InternalOnReceiveWithoutHeader(const boost::system::error_code& err,
		size_t bytes) {
	fprintf(stderr, " on receive without header: %lu b\n", bytes);
	fflush(stderr);
	if(err) {
		if(callback.onError)
			callback.onError(this, err);
	} else {
		callback.onReceive(this, &(buffer[0]), bytes);
	}
	RecallOnReceive();
}

#endif

