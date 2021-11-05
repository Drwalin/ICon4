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

#ifndef ASIO_HPP
#define ASIO_HPP

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include <cinttypes>

#include <boost/asio/ip/address.hpp>

namespace boost {
	namespace asio {
		class io_context;
	}
}

void IoContextPollOne();
class boost::asio::io_context& IoContext();

struct Endpoint {
public:
	
	Endpoint() : port(0) {
	}
	
	Endpoint(const char* ip, uint16_t port) :
		address(boost::asio::ip::make_address(ip)), port(port) {
	}
	
	Endpoint(boost::asio::ip::address address, uint16_t port) :
		address(address), port(port) {
	}
	
#ifdef BOOST_ASIO_IP_UDP_HPP
	inline boost::asio::ip::udp::endpoint UdpEndpoint() const {
		return boost::asio::ip::udp::endpoint(address, port);
	}
#endif
	
#ifdef BOOST_ASIO_IP_TCP_HPP
	inline boost::asio::ip::tcp::endpoint TcpEndpoint() const {
		return boost::asio::ip::tcp::endpoint(address, port);
	}
#endif

	inline bool operator == (const Endpoint& other) const {
		return address==other.address && port==other.port;
	}

	inline bool operator < (const Endpoint& other) const {
		return address<other.address ||
			(address==other.address && port<other.port);
	}

	inline std::string ToString() const {
		return address.to_string() + ":" + std::to_string(port);
	}
	
public:
	
	boost::asio::ip::address address;
	uint16_t port;
};

namespace std {
	inline std::string to_string(const Endpoint& endpoint) {
		return endpoint.ToString();
	}
}

#endif

