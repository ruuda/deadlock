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

#include "entry_collection.h"

using namespace deadlock::core;
using namespace deadlock::core::data;

void entry_collection::push_back(const entry& new_entry)
{
	entries.push_back(new_entry);

	// TODO: update acceleration structure
}

void entry_collection::deserialise_obfuscated(const serialisation::json_value::array_t& json_data, circular_buffer_512& transformation_buffer)
{
	// Loop through the data and read entries
	for (size_t i = 0; i < json_data.size(); i++)
	{
		// Add a new, blank entry to the collection
		entries.push_back(entry());
		// Load the correct data into it
		entries.back().deserialise_obfuscated(json_data[i], transformation_buffer);
	}

	// TODO: generate acceleration structure
}

void entry_collection::deserialise_deobfuscated(const serialisation::json_value::array_t& json_data, circular_buffer_512& obfuscation_buffer)
{
	// Loop through the data and read entries
	for (size_t i = 0; i < json_data.size(); i++)
	{
		// Add a new, blank entry to the collection
		entries.push_back(entry());
		// Load the correct data into it
		entries.back().deserialise_deobfuscated(json_data[i], obfuscation_buffer);
	}

	// TODO: generate acceleration structure
}

void entry_collection::serialise_obfuscated(serialisation::serialiser& serialiser)
{
	// Write the collection as an array
	serialiser.write_begin_array();
	{
		// Loop through the entries and write them
		for (size_t i = 0; i < entries.size(); i++)
		{
			entries[i].serialise_obfuscated(serialiser);
		}
	}
	serialiser.write_end_array();
}

void entry_collection::serialise_deobfuscated(serialisation::serialiser& serialiser, circular_buffer_512& obfuscation_buffer)
{
	// Write the collection as an array
	serialiser.write_begin_array();
	{
		// Loop through the entries and write them
		for (size_t i = 0; i < entries.size(); i++)
		{
			entries[i].serialise_deobfuscated(serialiser, obfuscation_buffer);
		}
	}
	serialiser.write_end_array();
}

