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

#include "ssl_acceptor_impl.hpp"

namespace net {
	namespace ssl {
		acceptor_impl::acceptor_impl(error_code& err, const endpoint& endpoint,
			bool enable_header, const char* certChainFile,
			const char* privateKeyFile, const char* dhFile,
			const char* password) : acceptor(enable_header),
			asio_acceptor(*default_io_context(), *endpoint.get_tcp()),
			ssl_context(boost::asio::ssl::context::tlsv13) {
				this->password = password;
			ssl_context.set_options(
					boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2
					| boost::asio::ssl::context::single_dh_use);
			ssl_context.set_password_callback(
					std::bind(&acceptor_impl::on_request_password, this,
						std::placeholders::_1, std::placeholders::_2));
			ssl_context.use_certificate_chain_file(certChainFile);
			ssl_context.use_private_key_file(privateKeyFile,
					boost::asio::ssl::context::pem);
			ssl_context.use_tmp_dh_file(dhFile);
				asio_acceptor.listen(256, err);
		}

		acceptor_impl::~acceptor_impl() {
			close();
		}

		bool acceptor_impl::is_open() const {
			return asio_acceptor.is_open();
		}
		
		void acceptor_impl::close() {
			cancel();
			error_code err;
			asio_acceptor.close(err);
		}
		
		void acceptor_impl::cancel() {
			error_code err;
			asio_acceptor.cancel(err);
		}

		void acceptor_impl::set_on_accept(
				std::function<void(acceptor*, socket*)> callback) {
			acceptor::set_on_accept(callback);
		}
		
		void acceptor_impl::start_listening() {
			accept_next();
		}

		void acceptor_impl::accept_next() {
			socket_impl* sock = new socket_impl(enable_header, ssl_context);
			asio_acceptor.async_accept(sock->sock.lowest_layer(),
					std::bind(&acceptor_impl::internal_on_accept, this, sock,
						std::placeholders::_1));
		}
		
		void acceptor_impl::internal_on_accept(socket_impl* sock,
				const error_code& err) {
			if(err) {
				delete sock;
				if(!on_error_callback(err))
					return;
			} else {
				error_code error;
				sock->sock.handshake(boost::asio::ssl::stream_base::server,
						error);
				if(error) {
					on_error_callback(error);
					delete sock;
				} else {
					on_accept_callback(this, sock);
				}
			}
			accept_next();
		}
		
		std::string acceptor_impl::on_request_password(size_t max_length,
				boost::asio::ssl::context::password_purpose purpose) {
			return password;
		}
	}
}

