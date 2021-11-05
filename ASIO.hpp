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
#include <boost/asio/ip/detail/endpoint.hpp>
#include <boost/system/error_code.hpp>

namespace boost {
	namespace asio {
		class io_context;
	}
}

#include <boost/asio.hpp>

void IoContextPollOne();
class boost::asio::io_context& IoContext();

struct Endpoint {
public:
	
	Endpoint() {
	}
	
	Endpoint(const char* ip, uint16_t port) :
		endpoint(boost::asio::ip::make_address(ip), port) {
	}
	
	Endpoint(boost::asio::ip::address address, uint16_t port) :
		endpoint(address, port) {
	}
	
	Endpoint(const Endpoint& other) : endpoint(other.endpoint) {
	}
	
	Endpoint(Endpoint&& other) : endpoint(other.endpoint) {
	}
	
	Endpoint& operator=(const Endpoint& other) {
		endpoint = other.endpoint;
		return *this;
	}
	
	Endpoint& operator=(Endpoint&& other) {
		endpoint = other.endpoint;
		return *this;
	}
	
#ifdef BOOST_ASIO_IP_UDP_HPP
	inline boost::asio::ip::udp::endpoint UdpEndpoint() const {
		return endpointUdp;
	}
	inline boost::asio::ip::udp::endpoint& UdpEndpoint() {
		return endpointUdp;
	}
#endif
	
#ifdef BOOST_ASIO_IP_TCP_HPP
	inline boost::asio::ip::tcp::endpoint TcpEndpoint() const {
		return endpointTcp;
	}
	inline boost::asio::ip::tcp::endpoint& TcpEndpoint() {
		return endpointTcp;
	}
#endif

	inline bool operator == (const Endpoint& other) const {
		return endpoint == other.endpoint;
	}

	inline bool operator < (const Endpoint& other) const {
		return endpoint < other.endpoint;
	}

	inline std::string ToString() const {
		return endpoint.to_string();
	}
	
public:
	
	union {
		boost::asio::ip::detail::endpoint endpoint;
#ifdef BOOST_ASIO_IP_UDP_HPP
		boost::asio::ip::udp::endpoint endpointUdp;
#endif
#ifdef BOOST_ASIO_IP_TCP_HPP
		boost::asio::ip::tcp::endpoint endpointTcp;
#endif
	};
};

namespace std {
	inline std::string to_string(const Endpoint& endpoint) {
		return endpoint.ToString();
	}
}

#endif

