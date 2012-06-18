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

#ifndef _DEADLOCK_CORE_DATA_OBFUSCATED_STRING_H_
#define _DEADLOCK_CORE_DATA_OBFUSCATED_STRING_H_

#include <vector>
#include <cstdint>
#include <string>

#include "../win32_export.h"
#include "../circular_buffer.h"
#include "deobfuscated_string.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			/// A container that stores a string in an obfuscated way
			/// In this manner the actual string does not have to be in memory (or on a disk);
			/// The string can be passed around without a human discovering the password.
			/// Of course, since the string can be obfuscated, it can always be de-obfuscated and be made visible.
			class _export obfuscated_string
			{
			protected:

				/// The string of obfuscated bytes
				std::vector<std::uint8_t> obfuscated_data;

				/// A static instance of the empty string
				static obfuscated_string empty_obfuscated_string;

			public:

				/// Creates a new string given the already obfuscated data
				obfuscated_string(const std::vector<std::uint8_t>& buffer_of_obfuscated_bytes);

				/// Creates a new string given the already obfuscated data in hexadecimal form
				obfuscated_string(const std::string& hexadecimal_string);

				/// Returns the empty obfuscated string
				static const obfuscated_string& empty();

				/// Returns the hexadecimal representation of the obfuscated data
				std::string get_hexadecimal_string() const;

				/// Creates a deobfuscated string that contains the same data as this string,
				/// but not obfuscated this time
				inline deobfuscated_string deobfuscate(circular_buffer_512& obfuscation_buffer) const
				{
					return deadlock::core::data::deobfuscated_string(obfuscated_data, obfuscation_buffer);
				}
			};
		}
	}
}

#endif