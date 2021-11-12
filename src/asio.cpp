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
#include <mutex>

#include "asio.hpp"

namespace net {
	static io_context* __default_io_context = new io_context();
	io_context* default_io_context() {
		return __default_io_context;
	}
	
	error_code poll_one() {
		error_code err;
		default_io_context()->poll_one(err);
		return err;
	}
	
	error_code poll() {
		error_code err;
		default_io_context()->poll(err);
		return err;
	}
	
	error_code run() {
		error_code err;
		default_io_context()->run(err);
		return err;
	}
	
	void reset() {
		default_io_context()->reset();
	}
	
	void stop() {
		default_io_context()->stop();
	}
	
	
	endpoint::endpoint() : ptr(NULL) {}
	endpoint::endpoint(const endpoint& other) : ptr(NULL) { (*this) = other; }
	endpoint::endpoint(endpoint&& other) : ptr(other.ptr), type(other.type) {}
	endpoint::~endpoint() { destroy(); }
	void endpoint::destroy() {
		if(ptr) {
			if(type == TCP) delete tcp;
			else            delete udp;
			ptr = NULL;
		}
	}
	
	endpoint& endpoint::operator=(const endpoint& other) {
		destroy();
		if(other.ptr) {
			if(other.type == TCP)
				tcp = new boost::asio::ip::tcp::endpoint(*other.tcp);
			else
				udp = new boost::asio::ip::udp::endpoint(*other.udp);
			type = other.type;
		}
		return *this;
	}
	
	endpoint& endpoint::operator=(endpoint&& other) {
		ptr = other.tcp;
		type = other.type;
		other.ptr = NULL;
		return *this;
	}
	
	endpoint endpoint::make_tcp(const char* ip, uint16_t port) {
		return endpoint(new boost::asio::ip::tcp::endpoint(
				boost::asio::ip::make_address(ip), port));
	}
	endpoint endpoint::make_udp(const char* ip, uint16_t port) {
		return endpoint(new boost::asio::ip::udp::endpoint(
				boost::asio::ip::make_address(ip), port));
	}
	endpoint endpoint::make_local_tcp(uint16_t port) {
		return endpoint(
				new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(),
					port));
	}
	endpoint endpoint::make_local_udp(uint16_t port) {
		return endpoint(
				new boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(),
					port));
	}
	
	endpoint::endpoint(boost::asio::ip::tcp::endpoint* endp) :
		tcp(endp), type(TCP) {}
	endpoint::endpoint(boost::asio::ip::udp::endpoint* endp) :
		udp(endp), type(UDP) {}
		
	boost::asio::ip::tcp::endpoint* endpoint::get_tcp() {
		if(type == TCP)
			return tcp;
		return NULL;
	}
	boost::asio::ip::udp::endpoint* endpoint::get_udp() {
		if(type == UDP)
			return udp;
		return NULL;
	}
	const boost::asio::ip::tcp::endpoint* endpoint::get_tcp() const {
		if(type == TCP)
			return tcp;
		return NULL;
	}
	const boost::asio::ip::udp::endpoint* endpoint::get_udp() const {
		if(type == UDP)
			return udp;
		return NULL;
	}
	
	uint16_t endpoint::port() const {
		if(ptr) {
			if(type == TCP) return tcp->port();
			else            return udp->port();
		}
		return 0;
	}
	bool endpoint::port(uint16_t p) {
		if(ptr) {
			if(type == TCP) tcp->port(p);
			else            udp->port(p);
			return true;
		}
		return false;
	}
	
	bool endpoint::is_v4() const {
		if(ptr) {
			if(type == TCP) return tcp->address().is_v4();
			else            return udp->address().is_v4();
		}
		return false;
	}
	
	bool endpoint::is_v6() const {
		if(ptr) {
			if(type == TCP) return tcp->address().is_v6();
			else            return udp->address().is_v6();
		}
		return false;
	}
	
	std::string endpoint::to_string() const {
		if(ptr) {
			std::string addr;
			if(type == TCP) addr = tcp->address().to_string();
			else            addr = udp->address().to_string();
			if(is_v6())
				addr = std::string("[") + addr + "]";
			return addr + ":" + std::to_string(port());
		}
		return "[NULL]:0";
	}
	
		
	endpoint endpoint::resolve_any_tcp(const char* address,
			const char* service) {
		boost::asio::ip::tcp::resolver resolver(*default_io_context());
		error_code err;
		auto results = resolver.resolve(address, service, err);
		resolver.cancel();
		if(!err)
			for(auto r : results)
				return endpoint(
						new boost::asio::ip::tcp::endpoint(r.endpoint()));
		return endpoint();
	}
	
	endpoint endpoint::resolve_any_udp(const char* address,
			const char* service) {
		boost::asio::ip::udp::resolver resolver(*default_io_context());
		error_code err;
		auto results = resolver.resolve(address, service, err);
		resolver.cancel();
		if(!err)
			for(auto r : results)
				return endpoint(
						new boost::asio::ip::udp::endpoint(r.endpoint()));
		return endpoint();
	}
	
	bool endpoint::resolve_all_tcp(const char* address, const char* service,
			std::vector<endpoint>& endpoints) {
		boost::asio::ip::tcp::resolver resolver(*default_io_context());
		error_code err;
		auto results = resolver.resolve(address, service, err);
		resolver.cancel();
		if(err)
			return false;
		for(auto r : results)
			endpoints.emplace_back(endpoint(
					new boost::asio::ip::tcp::endpoint(r.endpoint())));
		return true;
	}
	
	bool endpoint::resolve_all_udp(const char* address, const char* service,
			std::vector<endpoint>& endpoints) {
		boost::asio::ip::udp::resolver resolver(*default_io_context());
		error_code err;
		auto results = resolver.resolve(address, service, err);
		resolver.cancel();
		if(err)
			return false;
		for(auto r : results)
			endpoints.emplace_back(endpoint(
					new boost::asio::ip::udp::endpoint(r.endpoint())));
		return true;
	}
}

