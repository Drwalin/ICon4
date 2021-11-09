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

#ifndef ICON4_TCP_ACCEPTOR_IMPL_HPP
#define ICON4_TCP_ACCEPTOR_IMPL_HPP

#include <boost/asio.hpp>

#include "asio.hpp"
#include "socket.hpp"
#include "acceptor.hpp"
#include "tcp_socket_impl.hpp"

namespace net {
	namespace tcp {
		class acceptor_impl : public acceptor {
		public:
			acceptor_impl(error_code& err, const endpoint& endpoint,
				bool enableHeader);

			virtual ~acceptor_impl();

			virtual bool is_open() const override;
			virtual void close() override;

			virtual void set_on_accept(
					std::function<void(acceptor*, socket*)> callback) override;
			virtual void start_listening() override;

		protected:

			virtual void accept_next() override;
			
			void internal_on_accept(socket* sock, const error_code& err);

		protected:

			std::function<void(const error_code&, boost::asio::ip::tcp::socket)>
				on_accept_internal_callback;
			boost::asio::ip::tcp::acceptor asio_acceptor;
		};
	}
}

#endif

