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

#ifndef ICON4_ACCEPTOR_HPP
#define ICON4_ACCEPTOR_HPP

#include <vector>
#include <functional>

#include "asio.hpp"
#include "socket.hpp"

namespace net {
	class acceptor {
	public:

		static acceptor* make_tcp(error_code& err, const endpoint& endpoint,
				bool enableHeader);
		static acceptor* make_ssl(error_code& err, const endpoint& endpoint,
				bool enableHeader, const char* certChainFile,
				const char* privateKeyFile, const char* dhFile,
				const char* password);

		virtual ~acceptor();

		inline void* user_pointer() { return user_ptr; }
		inline void user_pointer(void* ptr) { user_ptr = ptr; }

		virtual bool is_open() const=0;
		virtual void close()=0;
		
		void set_on_error(
				std::function<bool(acceptor*, const error_code&)> callback);
		virtual void set_on_accept(
				std::function<void(acceptor*, socket*)> callback);
		virtual void start_listening()=0;

	protected:
		acceptor(bool enableHeader);
		
		virtual void accept_next()=0;

	protected:

		std::function<bool(const error_code&)> on_error_callback;
		std::function<void(acceptor*, socket*)> on_accept_callback;
		void* user_ptr;
		const bool enableHeader;
	};
}

#endif

