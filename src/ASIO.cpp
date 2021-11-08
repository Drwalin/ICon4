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

#include "ASIO.hpp"

boost::asio::io_context& IoContext() {
	static std::mutex mutex;
	static boost::asio::io_context* ioContext = NULL;
	std::lock_guard<std::mutex> lock(mutex);
	if(ioContext == NULL)
		ioContext = new boost::asio::io_context;
	return *ioContext;
}

void IoContextPollOne() {
	boost::system::error_code err;
	IoContext().poll_one(err);
	if(err) {
		printf(" boost::asio::io_context::poll_one() error: %s\n",
				err.message().c_str());
	}
}

void IoContextPoll() {
	boost::system::error_code err;
	IoContext().poll(err);
	if(err) {
		printf(" boost::asio::io_context::poll() error: %s\n",
				err.message().c_str());
	}
}

