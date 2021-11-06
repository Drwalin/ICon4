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

#ifndef GENERIC_LISTENER_HPP
#define GENERIC_LISTENER_HPP

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/basic_socket.hpp>
#include <cinttypes>
#include <string>
#include <functional>

#include "ASIO.hpp"
#include "GenericSocket.hpp"
#include "Socket.hpp"

template<typename T>
class GenericListenerCore;

template<typename T>
class GenericListener {
public:
	
	GenericListener(Endpoint endpoint, boost::system::error_code& err);
	GenericListener(Endpoint endpoint, const char* certChainFile,
			const char* privateKeyFile, const char* dhFile,
			std::string password, boost::system::error_code& err);
	~GenericListener();
	
	void SetOnAccept(std::function<bool(Endpoint, Socket*)> function);
	void SetOnError(
			std::function<void(const boost::system::error_code&)> function);
	
public:
	
	GenericListenerCore<T> core;
};

#include "GenericListenerImpl.hpp"
#include "GenericListenerTcpImpl.hpp"
#include "GenericListenerSslImpl.hpp"

#endif

