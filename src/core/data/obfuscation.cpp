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

#include "obfuscation.h"

using namespace deadlock::core::data;
using deadlock::core::circular_buffer_512;

void detail::xor_transform(secure_string_ptr string, circular_buffer_512& transformation_buffer)
{
	// Beginnings must line up, so reset the buffer
	transformation_buffer.reset();

	// Apply xor to every byte in the buffer
	for (size_t i = 0; i < string->size(); i++)
	{
		string->at(i) ^= transformation_buffer.next();
	}
}
