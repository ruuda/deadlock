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

#ifndef _DEADLOCK_CORE_DATA_ENTRY_COLLECTION_H_
#define _DEADLOCK_CORE_DATA_ENTRY_COLLECTION_H_

#include <vector>

#include "entry.h"
#include "../serialisation/value.h"
#include "../serialisation/serialiser.h"

#include "../win32_export.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			/// A container that stores all entries.
			/// It also contains an acceleration structure, used to do fast searches,
			/// and it handles finding possible matches for a given search string.
			class _export entry_collection
			{
			protected:

				/// The list of entries
				std::vector<entry> entries;

			public:

				/// Reconstructs the entries given the JSON data, assuming obfuscated data
				void deserialise_obfuscated(const serialisation::json_value::array_t& json_data, circular_buffer_512& transformation_buffer);

				/// Reconstructs the entries given the JSON data, assuming unobfuscated data
				/// The data will then be stored in an obfuscated way using the provided buffer
				void deserialise_deobfuscated(const serialisation::json_value::array_t& json_data, circular_buffer_512& obfuscation_buffer);

				/// Writes the entries to the serialiser as an array of objects
				/// This will write the passwords as hexadecimal strings of the obfuscated data
				void serialise_obfuscated(serialisation::serialiser& serialiser);

				/// Writes the entries to the serialiser as an array of objects
				/// This will write the passwords "as-is", after deobfuscation with the buffer
				void serialise_deobfuscated(serialisation::serialiser& serialiser, circular_buffer_512& obfuscation_buffer);

				/// Adds a new entry to the collection
				void push_back(const entry& new_entry);
			};
		}
	}
}

#endif