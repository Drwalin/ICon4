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

#include "socket.hpp"

#include <boost/asio/error.hpp>
#include <vector>
#include <functional>

#include "tcp.hpp"

namespace net {
	
	struct net_byte_order_impl {
		uint8_t b[2];
		
		inline void set(uint16_t value) {
			b[0] = value&0xFF;
			b[1] = (value>>8)&0xFF;
		}
		
		inline uint16_t get() const {
			return b[0] | ((uint16_t)b[1])<<8;
		}
		
		inline static size_t read(const void* buffer) {
			return ((const net_byte_order_impl*)buffer)->get();
		}
	};
	
	socket::~socket() {
	}
	
	error_code socket::send(const void* buffer, size_t bytes) {
		if(bytes > MAX_BUFFER_SIZE)
			return boost::asio::error::invalid_argument;
		if(enableHeader) {
			net_byte_order_impl b;
			b.set(bytes-1);
			error_code err = internal_send(b.b, 2);
			if(err)
				return err;
		}
		return internal_send(buffer, bytes);
	}
	
	size_t socket::read_raw(void* buffer, size_t bytes, error_code& err) {
		uint8_t* buf = (uint8_t*)buffer;
		size_t all_read=0;
		while(bytes) {
			size_t read = read_some_raw(buf+all_read, bytes, err);
			all_read += read;
			if(err)
				break;
			bytes -= read;
		}
		return all_read;
	}
	
	void socket::set_on_receive(
			std::function<void(socket*, void*, size_t)> callback) {
		on_receive_callback = callback;
		if(enableHeader) {
			buffer.resize(2);
		} else {
			buffer.resize(MAX_BUFFER_SIZE);
		}
		received = 0;
		async_receive();
	}
	
	void socket::set_on_error(
			std::function<bool(socket*, error_code)> callback) {
		on_error_callback = std::bind(callback, this, std::placeholders::_1);
	}
	
	socket::socket(bool enableHeader) :
		on_receive_internal_callback(std::bind(
					enableHeader ?
					&socket::internal_receive_with_header_callback :
					&socket::internal_receive_without_header_callback, this,
					std::placeholders::_1, std::placeholders::_2)),
		enableHeader(enableHeader) {
		received = 0;
		user_ptr = NULL;
	}
	
	void socket::internal_receive_with_header_callback(const error_code& err,
			size_t bytes) {
		if(err) {
			received = 0;
			buffer.resize(2);
			if(on_error_callback(err))
				async_receive();
			return;
		}
		received += bytes;
		if(received == buffer.size()) {
			if(buffer.size() == 2) {
				size_t size = net_byte_order_impl::read(&(buffer[0]))+1;
				buffer.resize(size+2);
			} else if(buffer.size() > 2) {
				on_receive_callback(this, &(buffer[2]), buffer.size()-2);
				received = 0;
				buffer.resize(2);
			} else {
				received = 0;
				buffer.resize(2);
				if(on_error_callback(boost::asio::error::fault))
					async_receive();
			}
		}
		async_receive();
	}
	
	void socket::internal_receive_without_header_callback(const error_code& err,
			size_t bytes) {
		if(err) {
			if(on_error_callback(err))
				async_receive();
			return;
		}
		on_receive_callback(this, &(buffer[0]), bytes);
		async_receive();
	}
	
	
	
	socket* socket::make_tcp(error_code& err, const endpoint& endpoint,
			bool enableHeader) {
		socket* sock = new tcp::socket_impl(err, endpoint, enableHeader);
		if(!err)
			return sock;
		delete sock;
		return NULL;
	}
}

