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

#ifndef ICON4_SOCKET_HPP
#define ICON4_SOCKET_HPP

#include <vector>
#include <functional>

#include "asio.hpp"

namespace net {
	class socket {
	public:
		inline const static size_t MAX_BUFFER_SIZE = 64*1024;

		static socket* make_tcp(error_code& err, const endpoint& endpoint,
				bool enableHeader);
		static socket* make_ssl(error_code& err, const endpoint& endpoint,
				bool enableHeader, const char* certFile);
		static socket* make_udp(error_code& err, const endpoint& endpoint);

		virtual ~socket();

		error_code send(const void* buffer, size_t bytes);
		size_t read_raw(void* buffer, size_t bytes, error_code& err);
		virtual size_t read_some_raw(void* buffer, size_t bytes, error_code& err)=0;

		virtual void set_on_receive(
				std::function<void(socket*, void*, size_t)> callback);
		void set_on_error(std::function<bool(socket*, error_code)> callback);

		inline void* user_pointer() { return user_ptr; }
		inline void user_pointer(void* ptr) { user_ptr = ptr; }

		virtual bool is_open() const=0;
		virtual void close()=0;

	protected:
		socket(bool enableHeader);

		void internal_receive_with_header_callback(const error_code& err,
				size_t bytes);
		void internal_receive_without_header_callback(const error_code& err,
				size_t bytes);
		virtual error_code internal_send(const void* buffer, size_t bytes) =0;

		virtual void async_receive()=0;

	protected:

		std::function<bool(const error_code&)> on_error_callback;
		std::function<void(socket*, void*, size_t)> on_receive_callback;
		const std::function<void(const error_code&, size_t)>
			on_receive_internal_callback;
		std::vector<uint8_t> buffer;
		size_t received;
		void* user_ptr;
		const bool enableHeader;
	};
}

#endif

