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

#ifndef ICON4_SSL_SOCKET_IMPL_HPP
#define ICON4_SSL_SOCKET_IMPL_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "asio.hpp"
#include "socket.hpp"

namespace net {
	namespace ssl {
		class socket_impl : public socket {
		public:
			socket_impl(bool enable_header,
					boost::asio::ssl::context& ssl_context);
			socket_impl(error_code& err, const endpoint& endpoint,
					bool enable_header, boost::asio::ssl::context& ssl_context);
			virtual ~socket_impl() override;
			
			virtual bool is_open() const override;
			virtual void close() override;
			virtual void cancel() override;

			virtual size_t read_some_raw(void* buffer, size_t bytes,
					error_code& err) override;

			friend class acceptor_impl;
			
		protected:
			virtual error_code internal_send(const void* buffer,
					size_t bytes) override;

			virtual void async_receive() override;

		protected:
			
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sock;
		};
	}
}

#endif

