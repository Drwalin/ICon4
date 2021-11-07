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

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <vector>
#include <functional>

#include "ASIO.hpp"

template<typename T>
struct GenericSocket;

class Socket {
public:
	
	inline const static size_t maxSingleBuffer = 64*1024;
	
	enum Type {
		UDP,
		TCP,
		SSL,
		Invalid
	};
	
#ifdef GENERIC_SOCKET_HPP
	Socket(GenericSocketUdp* udp);
	Socket(GenericSocketTcp* tcp);
	Socket(GenericSocketSsl* ssl);
#endif
	
	Socket(Endpoint endpoint, boost::system::error_code& err,
			bool enableHeader, Type type=TCP);
	Socket(Endpoint endpoint, boost::system::error_code& err, Type type=UDP);
	Socket(Endpoint endpoint, boost::system::error_code& err,
			bool enableHeader, const char* rootCertFile);
	~Socket();
	
	bool Send(const void* buffer, size_t bytes);
	void OnError(void* function);
	
	void SetOnError(void(*function)(Socket*, const boost::system::error_code&));
	void SetOnReceive(void(*function)(Socket*, void*, size_t));
	
public:
	
	union {
#ifdef GENERIC_SOCKET_HPP
		GenericSocketUdp* udp;
		GenericSocketTcp* tcp;
		GenericSocketSsl* ssl;
#endif
		void* ptr;
	};
	
	std::vector<uint8_t> buffer;
	size_t received, header;
	Endpoint endpoint;
	Type type;
	bool enableHeader;
	void* userPtr;
	
private:
	
	void InternalCreate(Endpoint endpoint, boost::system::error_code& err,
			bool enableHeader, Type type);
	
	inline bool InternalSend(const void* buffer, size_t bytes);
	
	void RecallOnReceive();
	void InternalOnReceiveWithHeader(const boost::system::error_code& err,
			size_t bytes);
	void InternalOnReceiveWithoutHeader(const boost::system::error_code& err,
			size_t bytes);
	
	struct {
		void (*onError)(Socket*, const boost::system::error_code&);
		void (*onReceive)(Socket*, void*, size_t);
		std::function<void(const boost::system::error_code&, size_t)>
			onReceiveFunc;
	} callback;
};

#endif

