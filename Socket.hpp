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

#include "ASIO.hpp"

template<typename T>
struct GenericSocket;

class Socket {
public:
	
	enum Type {
		UDP,
		TCP,
		SSL
	};
	
	Socket(Endpoint endpoint, bool enableHeader, Type type);
	~Socket();
	
	bool Send(const void* buffer, size_t bytes);
	void QueueFetch();
	void OnError(void* function);
	
public:
	
	union {
#ifdef GENERIC_SOCKET_HPP
		GenericSocket<Streams::UDP>* udp;
		GenericSocket<Streams::TCP>* tcp;
		GenericSocket<Streams::SSL>* ssl;
#endif
		void* ptr;
	};
	
	std::vector<uint8_t> buffer;
	size_t received, header;
	Endpoint endpoint;
	Type type;
	bool enabledHeader;
};


#endif

