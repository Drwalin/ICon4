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

#pragma once

#ifndef GENERIC_SOCKET_UDP_IMPL_HPP
#define GENERIC_SOCKET_UDP_IMPL_HPP

#include "GenericSocket.hpp"

template<>
inline GenericSocket<Streams::UDP>::GenericSocket(Endpoint endpoint,
		boost::system::error_code& err) :
	socket(IoContext(), endpoint.UdpEndpoint()) {
		if(endpoint.Port() == 0) {
			fprintf(stderr, "\nUDP Brodcast socket not implemented yet: %s:%i\n",
					__FILE__, __LINE__);
			fflush(stderr);
		} else {
			socket.connect(endpoint.UdpEndpoint(), err);
		}
}

template<>
inline boost::system::error_code GenericSocket<Streams::UDP>::Send(
		const void* data, size_t bytes) {
	boost::system::error_code err;
	size_t written = socket.send(boost::asio::buffer(data, bytes), 0, err);
	if(err)
		return err;
	if(written != bytes)
		return boost::system::error_code(
				boost::asio::error::broken_pipe);
	return err;
}

template<>
inline void GenericSocket<Streams::UDP>::QueueFetch(
		std::vector<uint8_t>& buffer, size_t offset,
		std::function<void(boost::system::error_code, size_t)> function) {
	boost::asio::ip::udp::endpoint endpoint;
	socket.async_receive_from(
			boost::asio::buffer(&(buffer[offset]), buffer.size()-offset),
			endpoint, function);
}

#endif

