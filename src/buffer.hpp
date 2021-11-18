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

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cinttypes>
#include <vector>
#include <atomic>

namespace net {
	
	class buffer {
	public:
		buffer();
		~buffer();
		buffer(const buffer&) = delete;
		buffer(buffer&) = delete;
		buffer(buffer&&o) { _data=o._data; o._data=NULL; }
		
		buffer& operator=(const buffer&) = delete;
		buffer& operator=(buffer&) = delete;
		buffer& operator=(buffer&&);
		
		inline uint8_t& operator[](size_t id) { return _data->operator[](id); }
		inline const uint8_t operator[](size_t id) const {return _data->at(id); }
		
		inline void resize(size_t s) { _data->resize(s); }
		inline void reserve(size_t s) { _data->reserve(s); }
		inline size_t size() const { return _data->size(); }
		inline uint8_t* data() { return _data->data(); }
		
	private:
		
		static std::vector<uint8_t>* allocate();
		static void free(std::vector<uint8_t>* data);
		
		std::vector<uint8_t>* _data;
	};
}

#endif

