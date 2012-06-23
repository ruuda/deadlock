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

#include "deobfuscated_string.h"

#include <cstring>

using deadlock::core::circular_buffer_512;
using namespace deadlock::core::data;

deobfuscated_string::deobfuscated_string(const std::vector<std::uint8_t>& buffer_of_obfuscated_bytes, circular_buffer_512& obfuscation_buffer)
{
	// The length is equal to the string length plus one for the null terminator
	length = buffer_of_obfuscated_bytes.size() + 1;

	// Allocate a new buffer for the characters
	deobfuscated_data = new char[length];

	// Reset buffer, because the beginning must line up
	obfuscation_buffer.reset();

	// Copy over the characters while de-obfuscating them using the buffer
	for (size_t i = 0; i < length - 1; i++)
	{
		deobfuscated_data[i] = buffer_of_obfuscated_bytes[i] ^ obfuscation_buffer.next();
	}

	// Last character is a null character
	deobfuscated_data[length - 1] = '\0';
}

deobfuscated_string::deobfuscated_string(const deobfuscated_string& other)
{
	// Copy length
	length = other.length;

	// Allocate memory for the characters
	deobfuscated_data = new char[length];

	// Copy the string
	std::strcpy(deobfuscated_data, other.deobfuscated_data);
}

deobfuscated_string& deobfuscated_string::operator=(const deobfuscated_string& other)
{
	if (&other == this) return *this;

	// Copy length
	length = other.length;

	// Allocate memory for the characters
	deobfuscated_data = new char[length];

	// Copy the string
	std::strcpy(deobfuscated_data, other.deobfuscated_data);

	return *this;
}

deobfuscated_string::~deobfuscated_string()
{
	// Overwrite memory with zeroes to erase the deobfuscated string from memory
	for (size_t i = 0; i < length; i++)
	{
		deobfuscated_data[i] = 0;
	}

	// TODO: could caches cause any harm here?

	// Delete the string
	delete [] deobfuscated_data;
}
