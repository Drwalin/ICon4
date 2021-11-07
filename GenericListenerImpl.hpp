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

#ifndef GENERIC_LISTENER_IMPL_HPP
#define GENERIC_LISTENER_IMPL_HPP

#include "GenericListener.hpp"

template<typename T>
inline void GenericListener<T>::SetOnError(
		std::function<void(const boost::system::error_code&)> function) {
	callback.onError = function;
}

template<typename T>
inline void GenericListener<T>::StartListening(
		std::function<bool(Socket*)> function) {
	callback.onAcceptInternal = std::bind(&GenericListener<T>::InternalAccept,
			this, std::placeholders::_1, std::placeholders::_2);
	callback.onAccept = function;
	InternalAsyncListening();
}

template<typename T>
inline void GenericListener<T>::InternalAsyncListening() {
	core.acceptor.async_accept(callback.onAcceptInternal);

}

#endif

