/*
 *  This file is part of ICon4. Please see README for details.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
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
class GenericSocket {
public:
	
	GenericSocket() :
		socket(NULL) {
	}
	
	GenericSocket(T* socket) :
		socket(socket) {
	}
	
	~GenericSocket() {
		Close();
	}
	
	inline boost::system::error_code Connect(Endpoint endpoint,
			bool enableSizeInHeader) {
		boost::system::error_code err;
		Close();
		socket = new T(IoContext());
		socket->connect(endpoint, err);
		if(err) {
			socket->close();
			delete socket;
			socket = NULL;
			return err;
		}
		return boost::system::error_code(0);
	}
	
	inline boost::system::error_code ConnectSsl(Endpoint endpoint,
			bool enableSizeInHeader, const char* rootCertFile);
	
	inline void Close() {
		if(socket) {
			socket->cancel();
			socket->close();
			delete socket;
		}
	}
	
	inline boost::system::error_code Send(const void* data, size_t bytes) {
		boost::system::error_code err;
		if(socket) {
			for(uint64_t i=0; i<bytes;) {
				size_t written = socket->write_some(
						boost::asio::buffer((uint8_t*)data+i, bytes-i), err);
				i += written;
				if(err) {
					return err;
				} else if(written == 0) {
					return boost::system::error_code(0);
				}
			}
			return err;
		}
		return ;
	}
	
	template<typename Func>
	inline void QueueFetch(std::vector<uint8_t>& buffer, size_t offset,
			Func function) {
		if(socket) {
			socket->async_read_some(boost::asio::buffer(
					&(buffer[offset]), buffer.size()-offset),
					function);
		}
	}
	
	inline bool Valid() const {
		return socket!=NULL;
	}
	
public:
	
	T* socket;
};

template<>
inline boost::system::error_code GenericSocket<Streams::SSL>::Connect(
		Endpoint endpoint, bool enableSizeInHeader) = delete;

template<>
inline boost::system::error_code GenericSocket<Streams::SSL>::ConnectSsl(
		Endpoint endpoint, bool enableSizeInHeader, const char* rootCertFile) {
	boost::system::error_code err;
	boost::asio::ssl::context sslContext(boost::asio::ssl::context::sslv23);
	sslContext.load_verify_file(rootCertFile);
	socket = new Streams::SSL(IoContext(), sslContext);
	socket->lowest_layer().connect(endpoint.TcpEndpoint(), err);
	if(err) {
		delete socket;
		socket = NULL;
		return err;
	}
	socket->handshake(boost::asio::ssl::stream_base::client, err);
	if(err) {
		socket->close();
		delete socket;
		socket = NULL;
		return err;
	}
	return boost::system::error_code(0);
}
	
using GenericSocketUdp = GenericSocket<Streams::UDP>;
using GenericSocketTcp = GenericSocket<Streams::TCP>;
using GenericSocketSsl = GenericSocket<Streams::SSL>;

#endif

