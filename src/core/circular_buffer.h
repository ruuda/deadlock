// Deadlock – fast search-based password manager
// Copyright (C) 2012 Ruud van Asseldonk

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _DEADLOCK_CORE_CIRCULAR_BUFFER_H_
#define _DEADLOCK_CORE_CIRCULAR_BUFFER_H_

#include <cstdint>

#include "win32_export.h"

namespace deadlock
{
	namespace core
	{
		/// Holds a certain amount of bytes, and provides bytes on request.
		/// When the last byte is read, it starts again at the beginning.
		template <size_t buffer_size> class _export circular_buffer
		{
		protected:

			/// The raw data
			std::uint8_t buffer[buffer_size];

			/// Index into the byte array.
			/// Points before the next byte that will be read
			size_t cursor;

		public:

			/// Does not initialise the buffer, instantiates it with undefined content
			circular_buffer()
			{
				cursor = 0;
			}

			/// Initialises the buffer by copying the given bytes
			/// The provided array must have enough bytes to fill the buffer
			circular_buffer(std::uint8_t* buffer_to_copy)
			{
				for (size_t i = 0; i < buffer_size; i++)
				{
					buffer[i] = buffer_to_copy[i];
				}

				/// Start reading at the first byte
				cursor = 0;
			}

			/// Returns the buffer size
			size_t size() const { return buffer_size; }

			/// Resets the cursor to the beginning of the buffer
			void reset()
			{
				cursor = 0;
			}

			/// Returns the next byte from the buffer and advances the cursor
			std::uint8_t next()
			{
				std::uint8_t byte = buffer[cursor];
					
				cursor++; if (cursor >= buffer_size) cursor = 0;

				return byte;
			}

			/// Returns the byte at the specified position
			std::uint8_t operator[](size_t index) const
			{
				return buffer[index];
			}

			/// Returns the byte at the specified position
			std::uint8_t& operator[](size_t index)
			{
				return buffer[index];
			}
		};

		/// A 512-bit circular buffer
		typedef circular_buffer<64> circular_buffer_512;
	}
}

#endif