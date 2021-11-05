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

#ifndef GENERIC_LISTENER_HPP
#define GENERIC_LISTENER_HPP

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ssl.hpp>
#include <cinttypes>
#include <string>
#include <functional>

#include "ASIO.hpp"
#include "GenericSocket.hpp"

namespace Streams {
	using TCP = boost::asio::ip::tcp::socket;
	using UDP = boost::asio::ip::udp::socket;
	using SSL = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
}

template<typename T=Streams::SSL>
class GenericListener {
public:
	
	GenericListener() :
		socket(NULL) {
	}
	
	GenericListener(T* socket) :
		socket(socket) {
	}
	
	~GenericListener() {
		Close();
	}
	
	inline boost::system::error_code Connect(Endpoint endpoint,
			bool enableSizeInHeader) {
		boost::system::error_code err;
		Close();
		socket = new T(IoContext());
		socket->connect(endpoint, err);
		if(err) {
			Close();
			return err;
		}
		return boost::system::error_code();
	}
	
	inline boost::system::error_code Connect(Endpoint endpoint,
			bool enableSizeInHeader, const char* rootCertFile);
	
	inline void Close() {
		if(socket) {
			socket->cancel();
			socket->close();
			delete socket;
			socket = NULL;
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
					return boost::system::error_code(
							boost::asio::error::broken_pipe);
				}
			}
			return err;
		}
		return boost::system::error_code();
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
inline boost::system::error_code GenericListener<Streams::SSL>::Connect(
		Endpoint endpoint, bool enableSizeInHeader) = delete;

template<>
inline boost::system::error_code GenericListener<Streams::SSL>::Connect(
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
		delete socket;
		socket = NULL;
		return err;
	}
	return boost::system::error_code();
}
	
using GenericListenerUdp = GenericListener<Streams::UDP>;
using GenericListenerTcp = GenericListener<Streams::TCP>;
using GenericListenerSsl = GenericListener<Streams::SSL>;

#endif

