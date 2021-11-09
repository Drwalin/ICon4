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

#include "tcp_acceptor_impl.hpp"

namespace net {
	namespace tcp {
		acceptor_impl::acceptor_impl(error_code& err, const endpoint& endpoint,
			bool enableHeader) : acceptor(enableHeader),
			asio_acceptor(*default_io_context(), *endpoint.get_tcp()) {
			asio_acceptor.listen(16, err);
		}

		acceptor_impl::~acceptor_impl() {
			close();
		}

		bool acceptor_impl::is_open() const {
			return asio_acceptor.is_open();
		}
		
		void acceptor_impl::close() {
			asio_acceptor.cancel();
			asio_acceptor.close();
		}

		void acceptor_impl::set_on_accept(
				std::function<void(acceptor*, socket*)> callback) {
			acceptor::set_on_accept(callback);
		}
		
		void acceptor_impl::start_listening() {
			accept_next();
		}

		void acceptor_impl::accept_next() {
			socket_impl* sock = new socket_impl(enableHeader);
			asio_acceptor.async_accept(sock->sock,
					std::bind(&acceptor_impl::internal_on_accept, this, sock,
						std::placeholders::_1));
		}
		
		void acceptor_impl::internal_on_accept(socket* sock,
				const error_code& err) {
			if(err) {
				delete sock;
				if(!on_error_callback(err))
					return;
			} else {
				on_accept_callback(this, sock);
			}
			accept_next();
		}
	}
}

