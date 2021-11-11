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

#ifndef ICON4_SSL_ACCEPTOR_IMPL_HPP
#define ICON4_SSL_ACCEPTOR_IMPL_HPP

#include <boost/asio.hpp>

#include "asio.hpp"
#include "socket.hpp"
#include "acceptor.hpp"
#include "ssl_socket_impl.hpp"

namespace net {
	namespace ssl {
		class acceptor_impl : public acceptor {
		public:
			acceptor_impl(error_code& err, const endpoint& endpoint,
				bool enable_header, const char* certChainFile,
				const char* privateKeyFile, const char* dhFile,
				const char* password);

			virtual ~acceptor_impl();

			virtual bool is_open() const override;
			virtual void close() override;
			virtual void cancel() override;

			virtual void set_on_accept(
					std::function<void(acceptor*, socket*)> callback) override;
			virtual void start_listening() override;

		protected:

			virtual void accept_next() override;
			
			void internal_on_accept(socket_impl* sock, const error_code& err);
			
			std::string on_request_password(size_t max_length,
					boost::asio::ssl::context::password_purpose purpose);

		protected:

			std::function<void(const error_code&, boost::asio::ip::tcp::socket)>
				on_accept_internal_callback;
			boost::asio::ip::tcp::acceptor asio_acceptor;
			
			boost::asio::ssl::context ssl_context;
			std::string password;
		};
	}
}

#endif

