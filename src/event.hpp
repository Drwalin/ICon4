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

#ifndef EVENT_HPP
#define EVENT_HPP

namespace net {
	class event {
	public:
		
		enum type {
			CLOSE_LOOP,
			CLOSE_SOCKET,
			CLOSE_CONTEXT,
			SEND_BUFFER,
			BROADCAST,
			CUSTOM
		};
		
		~event();
		
		void run();
		
	private:
		event();
		
		union {
			void* data;
			class buffer* buffer;
		};
		
		union {
			class socket* recipient;
		};
		
		type type;
	};
}

#endif

