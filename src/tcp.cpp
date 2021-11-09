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

#include "tcp.hpp"

#include "asio.hpp"
#include "socket.hpp"

namespace net {
	namespace tcp {
		socket_impl::socket_impl(error_code& err, const endpoint& endpoint,
				bool enableHeader) :
			socket(enableHeader),
			sock(*default_io_context()) {
				sock.connect(*endpoint.get_tcp(), err);
		}
		
		socket_impl::~socket_impl() {
			close();
		}

		bool socket_impl::is_open() const {
			return sock.is_open();
		}
		
		void socket_impl::close() {
			sock.cancel();
			sock.close();
		}

		error_code socket_impl::internal_send(const void* buffer, size_t bytes) {
			error_code err;
			while(bytes && !err) {
				size_t read = sock.write_some(
						boost::asio::buffer(buffer, bytes), err);
				bytes -= read;
				(uint8_t*&)buffer += read;
			}
			return err;
		}

		void socket_impl::async_receive() {
			sock.async_read_some(
					boost::asio::buffer(
						&(buffer[received]), buffer.size()-received),
					on_receive_internal_callback);
		}
	}
}

