/*
 *  This file is part of ICon4. Please see README for details.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CPP_FILES_CPP
#define CPP_FILES_CPP
#endif

#include "ASIO.hpp"

boost::asio::io_context& IoContext() {
	static boost::asio::io_context* ioContext = NULL;
	if(ioContext == NULL)
		ioContext = new boost::asio::io_context;
	return *ioContext;
}

void IoContextPollOne() {
	IoContext().poll_one();
}

