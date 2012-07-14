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

#include <queue>

using namespace deadlock::core;
using namespace deadlock::core::data;

void entry_collection::push_back(const entry& new_entry)
{
	entries.push_back(new_entry);

	// TODO: update acceleration structure
}

void entry_collection::deserialise(const serialisation::json_value::array_t& json_data)
{
	// Loop through the data and read entries
	for (size_t i = 0; i < json_data.size(); i++)
	{
		// Add a new, blank entry to the collection
		entries.push_back(entry());
		// Load the correct data into it
		entries.back().deserialise(json_data[i]);
	}

	// TODO: generate acceleration structure
}

void entry_collection::serialise(serialisation::serialiser& serialiser, bool obfuscation)
{
	// Write the collection as an array
	serialiser.write_begin_array();
	{
		// Loop through the entries and write them
		for (size_t i = 0; i < entries.size(); i++)
		{
			entries[i].serialise(serialiser, obfuscation);
		}
	}
	serialiser.write_end_array();
}

// A helper structure for the search algorithm
struct entry_match
{
	entry* entry;
	int probability;
};

// By defining this operator, std::less works, and this allows the priority queue to work
bool operator<(const entry_match& m1, const entry_match& m2)
{
	return m1.probability < m2.probability;
}

std::vector<entry*> entry_collection::find_entries(const data::secure_string& search) const
{
	std::priority_queue<entry_match> matches;

	// First of all, make the search string lowercase
	data::secure_string_ptr str = make_secure_string(search);
	// TODO: UTF-8 tolower, and what about the locale?
	// Is there a better way to do this?
	std::for_each(str->begin(), str->end(), [](char& c) { c = std::tolower(c, std::locale()); });

	// TODO: find results

	// A vector with the results
	std::vector<entry*> result;

	// Copy the found matches in the correct order
	while (!matches.empty())
	{
		result.push_back(matches.top().entry);
		matches.pop();
	}

	return result;
}