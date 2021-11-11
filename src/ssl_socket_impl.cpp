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

#include "ssl_socket_impl.hpp"

#include <utility>

#include "asio.hpp"

namespace net {
	namespace ssl {
		socket_impl::socket_impl(bool enable_header,
				boost::asio::ssl::context& ssl_context) : socket(enable_header),
			sock(*default_io_context(), ssl_context) {
		}
		
		socket_impl::socket_impl(error_code& err, const endpoint& endpoint,
				bool enable_header, boost::asio::ssl::context& ssl_context) :
			socket(enable_header), sock(*default_io_context(), ssl_context) {
				sock.lowest_layer().connect(*endpoint.get_tcp(), err);
				if(err)
					return;
				sock.handshake(boost::asio::ssl::stream_base::client, err);
				if(err) {
					close();
					return;
				}
		}
		
		socket_impl::~socket_impl() {
			close();
		}

		bool socket_impl::is_open() const {
			return sock.lowest_layer().is_open();
		}
		
		void socket_impl::close() {
			cancel();
			error_code err;
			sock.next_layer().close(err);
		}
		
		void socket_impl::cancel() {
			error_code err;
			sock.next_layer().cancel(err);
		}
		
		size_t socket_impl::read_some_raw(void* buffer, size_t bytes,
				error_code& err) {
			return sock.read_some(boost::asio::buffer(buffer, bytes), err);
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

