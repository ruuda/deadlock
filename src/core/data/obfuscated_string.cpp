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

#include "obfuscated_string.h"

#include <sstream>
#include <iomanip>

#include "../circular_buffer.h"

using namespace deadlock::core::data;
using deadlock::core::circular_buffer_512;

// <see password.cpp for implementation of the empty obfuscated string>

obfuscated_string::obfuscated_string(const std::vector<std::uint8_t>& buffer_of_obfuscated_bytes) :
	obfuscated_data(buffer_of_obfuscated_bytes.begin(), buffer_of_obfuscated_bytes.end())
{

}

obfuscated_string::obfuscated_string(const std::string& hexadecimal_string)
{
	std::stringstream hex_string(hexadecimal_string);

	std::string byte_string;

	// Read two characters from the stream
	while (hex_string >> std::setw(2) >> byte_string)
	{
		// Create a new stringstream with the two characters
		std::stringstream byte_stream(byte_string);
		// Read the integer value
		int byte; byte_stream >> std::hex >> byte;
		obfuscated_data.push_back(static_cast<std::uint8_t>(byte));
	}
}

obfuscated_string::obfuscated_string(const std::string& unobfuscated_string, circular_buffer_512& obfuscation_buffer)
{
	// Allocate enough bytes to store the string
	obfuscated_data.reserve(unobfuscated_string.size());

	// Reset the buffer, because starting positions must align
	obfuscation_buffer.reset();

	// Loop through the characters and store them
	for (size_t i = 0; i < unobfuscated_string.size(); i++)
	{
		// Obfuscate using (original_char xor buffer_byte)
		// This is a simple and reversible method of obfuscating
		obfuscated_data.push_back(unobfuscated_string[i] ^ obfuscation_buffer.next());
	}
}

const obfuscated_string& obfuscated_string::empty()
{
	return empty_obfuscated_string;
}

std::string obfuscated_string::get_hexadecimal_string() const
{
	std::stringstream hex_string;

	// Write all bytes as hexadecimal characters
	for (size_t i = 0; i < obfuscated_data.size(); i++)
	{
		hex_string << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(obfuscated_data[i]);
	}

	return hex_string.str();
}

void obfuscated_string::transform(circular_buffer_512& transformation_buffer)
{
	// Beginnings must line up, so reset the buffer
	transformation_buffer.reset();

	// Apply xor to every byte in the buffer
	for (size_t i = 0; i < obfuscated_data.size(); i++)
	{
		obfuscated_data[i] ^= transformation_buffer.next();
	}
}