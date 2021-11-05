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
	
	Socket(Endpoint endpoint, bool enableHeader, Type type);
	Socket(Endpoint endpoint, bool enableHeader, const char* rootCertFile);
	~Socket();
	
	bool Send(const void* buffer, size_t bytes);
	void QueueFetch();
	void OnError(void* function);
	
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
	
	void SetOnError(void(*function)(Socket*, const boost::system::error_code&));
	void SetOnReceive(void(*function)(Socket*, void*, size_t));
	
private:
	
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

