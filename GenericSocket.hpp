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

#ifndef GENERIC_SOCKET_HPP
#define GENERIC_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ssl.hpp>
#include <cinttypes>
#include <string>
#include <functional>

#include "ASIO.hpp"

namespace Streams {
	using TCP = boost::asio::ip::tcp::socket;
	using UDP = boost::asio::ip::udp::socket;
	using SSL = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
}

template<typename T=Streams::SSL>
struct GenericSocket {
public:
	
	GenericSocket(T* socket);
	GenericSocket(Endpoint endpoint, boost::system::error_code& err);
	GenericSocket(Endpoint endpoint, const char* rootCertFile,
			boost::system::error_code& err);
	
	~GenericSocket() { Close(); }
	
	inline void Close();
	
	inline boost::system::error_code Send(const void* data, size_t bytes);
	inline void QueueFetch(std::vector<uint8_t>& buffer, size_t offset,
			std::function<void(boost::system::error_code, size_t)> function);
	
	inline bool Valid() const { return socket!=NULL; }
	
public:
	
	T socket;
};

#include "GenericSocketImpl.hpp"
#include "GenericSocketUdpImpl.hpp"
#include "GenericSocketTcpImpl.hpp"
#include "GenericSocketSslImpl.hpp"

using GenericSocketUdp = GenericSocket<Streams::UDP>;
using GenericSocketTcp = GenericSocket<Streams::TCP>;
using GenericSocketSsl = GenericSocket<Streams::SSL>;

#endif

