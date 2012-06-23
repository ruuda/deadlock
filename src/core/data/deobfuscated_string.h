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

#ifndef _DEADLOCK_CORE_DATA_DEOBFUSCATED_STRING_H_
#define _DEADLOCK_CORE_DATA_DEOBFUSCATED_STRING_H_

#include <vector>
#include <cstdint>

#include "../win32_export.h"
#include "../circular_buffer.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			/// A container that stores the deobfuscated string as a C-style string
			/// It clears the memory upon destruction, such that the string does not
			/// remain in memory.
			/// \todo: what about CPU-caches?
			class _export deobfuscated_string
			{
				friend class obfuscated_string;

			protected:

				/// Character array for the string
				char* deobfuscated_data;

				/// The number of characters in the string (including null terminator)
				size_t length;

				/// Creates a deobfuscated string given the obfuscated string and a buffer
				deobfuscated_string(
					const std::vector<std::uint8_t>& buffer_of_obfuscated_bytes,
					circular_buffer_512& obfuscation_buffer
				);

			public:

				/// Copy constructor
				deobfuscated_string(const deobfuscated_string& other);

				/// Assignment operator
				deobfuscated_string& operator=(const deobfuscated_string& other);

				/// Destructor (clears memory before deleting)
				~deobfuscated_string();

				/// Returns the deobfuscated string as a C-style string
				inline const char* c_str() const { return deobfuscated_data; }
			};
		}
	}
}

#endif