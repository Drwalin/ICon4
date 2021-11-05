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

#ifndef GENERIC_SOCKET_IMPL_HPP
#define GENERIC_SOCKET_IMPL_HPP

#include "GenericSocket.hpp"

template<typename T>
inline boost::system::error_code GenericSocket<T>::Send(const void* data, size_t bytes) {
	boost::system::error_code err;
	if(socket) {
		for(uint64_t i=0; i<bytes;) {
			size_t written = socket->write_some(
					boost::asio::buffer((uint8_t*)data+i, bytes-i), err);
			i += written;
			if(err) {
				return err;
			} else if(written == 0) {
				return boost::system::error_code(
						boost::asio::error::broken_pipe);
			}
		}
		return err;
	}
	return boost::system::error_code();
}

#endif

