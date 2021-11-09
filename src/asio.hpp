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

#ifndef ICON4_ASIO_HPP
#define ICON4_ASIO_HPP

#include <cinttypes>
#include <vector>

#include <boost/system/error_code.hpp>
#include <boost/asio/error.hpp>

namespace boost {
	namespace asio {
		namespace ip {
#ifndef BOOST_ASIO_IP_ADDRESS_HPP
			class address;
#endif
#ifndef BOOST_ASIO_IP_TCP_HPP
			class tcp {
				public:
					class endpoint;
			};
#endif
#ifndef BOOST_ASIO_IP_UDP_HPP
			class udp {
				public:
					class endpoint;
			};
#endif
		}
#ifndef BOOST_ASIO_IO_CONTEXT_HPP
			class io_context;
#endif
	}
}

namespace net {
	using error_code = boost::system::error_code;
	
	class endpoint {
	public:
		enum type_type {
			TCP,
			UDP
		};
		
		endpoint();
		endpoint(const endpoint& other);
		endpoint(endpoint&& other);
		~endpoint();
		
		static endpoint make_tcp(const char* ip, uint16_t port);
		static endpoint make_udp(const char* ip, uint16_t port);
		
		static endpoint resolve_any_udp(const char* address, const char* service);
		static endpoint resolve_any_tcp(const char* address, const char* service);
		static bool resolve_all_udp(const char* address, const char* service,
				std::vector<endpoint>& endpoints);
		static bool resolve_all_tcp(const char* address, const char* service,
				std::vector<endpoint>& endpoints);
		
		endpoint& operator=(const endpoint& other);
		endpoint& operator=(endpoint&& other);
		
		uint16_t port() const;
		bool port(uint16_t p);
		
		void destroy();
		
		inline operator bool() const { return ptr; }
		
		boost::asio::ip::tcp::endpoint* get_tcp();
		boost::asio::ip::udp::endpoint* get_udp();
		const boost::asio::ip::tcp::endpoint* get_tcp() const;
		const boost::asio::ip::udp::endpoint* get_udp() const;
		
		bool is_v4() const;
		bool is_v6() const;
		
		std::string to_string() const;
		
	private:
		endpoint(boost::asio::ip::tcp::endpoint* endp);
		endpoint(boost::asio::ip::udp::endpoint* endp);
		union {
			boost::asio::ip::tcp::endpoint* tcp;
			boost::asio::ip::udp::endpoint* udp;
			void* ptr;
		};
		type_type type;
	};
	
	using address = boost::asio::ip::address;
	using io_context = boost::asio::io_context;
	
	io_context* default_io_context();
	error_code poll_one();
	error_code poll();
	error_code run();
	void stop();
}

#endif

