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
#include <memory>

#include "entry.h"
#include "../serialisation/value.h"
#include "../serialisation/serialiser.h"
#include "secure_allocator.h"
#include "../win32.h"

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
				std::vector<std::shared_ptr<entry>> entries;

			public:

				typedef std::shared_ptr<entry> entry_ptr;
				typedef std::vector<entry_ptr>::iterator entry_iterator;
				typedef std::vector<entry_ptr>::const_iterator const_entry_iterator;

				/// Reconstructs the entries given the JSON data
				void deserialise(const serialisation::json_value::array_t& json_data);

				/// Writes the entries to the serialiser as array of objects
				/// If obfuscation is true, this will write the passwords as hexadecimal strings of of the password bytes.
				/// Otherwise, it will write the passwords as-is.
				void serialise(serialisation::serialiser& serialiser, bool obfuscation);

				/// Adds a new entry to the collection
				void push_back(entry_ptr entry);

				/// Returns an iterator to the first entry
				inline entry_iterator begin() { return entries.begin(); }

				/// Returns an iterator past the last entry
				inline entry_iterator end() { return entries.end(); }

				/// Returns an iterator to the first entry
				inline const_entry_iterator begin() const { return entries.begin(); }

				/// Returns an iterator past the last entry
				inline const_entry_iterator end() const { return entries.end(); }
			};
		}
	}
}

#endif