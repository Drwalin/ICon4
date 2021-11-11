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

#include "acceptor.hpp"

#include "tcp_acceptor_impl.hpp"
#include "ssl_acceptor_impl.hpp"

namespace net {
	acceptor* acceptor::make_tcp(error_code& err, const endpoint& endpoint,
			bool enable_header) {
		return new tcp::acceptor_impl(err, endpoint, enable_header);
	}
	
	acceptor* acceptor::make_ssl(error_code& err, const endpoint& endpoint,
			bool enable_header, const char* certChainFile,
				const char* privateKeyFile, const char* dhFile,
				const char* password) {
		return new ssl::acceptor_impl(err, endpoint, enable_header,
				certChainFile, privateKeyFile, dhFile, password);
	}

	acceptor::~acceptor() {
	}

	void acceptor::set_on_error(
			std::function<bool(acceptor*, const error_code&)> callback) {
		on_error_callback = std::bind(callback, this, std::placeholders::_1);
	}
	
	void acceptor::set_on_accept(
			std::function<void(acceptor*, socket*)> callback) {
		on_accept_callback = callback;
	}

	acceptor::acceptor(bool enable_header) :
		enable_header(enable_header) {
	}
}

