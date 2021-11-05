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

#ifndef GENERIC_SOCKET_TCP_IMPL_HPP
#define GENERIC_SOCKET_TCP_IMPL_HPP

#include "GenericSocket.hpp"

template<>
inline boost::system::error_code GenericSocket<Streams::TCP>::Connect(
		Endpoint endpoint, bool enableHeader) {
	boost::system::error_code err;
	Close();
	socket = new Streams::TCP(IoContext());
	socket->connect(endpoint.TcpEndpoint(), err);
	if(err) {
		Close();
		return err;
	}
	this->endpoint = endpoint;
	return boost::system::error_code();
}

#endif

