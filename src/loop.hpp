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

#include <vector>
#include <mutex>
#include <atomic>
#include <cinttypes>

struct us_loop_t;

namespace net {
	class loop {
	public:

		void enqueue();
		void run();
		void stop();
		void wakeup();
		
	private:
		~loop();
		loop();
		
		static void _internal_on_pre(us_loop_t* loop);
		static void _internal_on_post(us_loop_t* loop);
		static void _internal_on_wakeup(us_loop_t* loop);
		
		void _on_pre();
		void _on_post();
		void _on_wakeup();
		
		static void operator delete(void* p);
		static void* operator new(size_t size);
		
	private:
		us_loop_t* impl;
	};
}

