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

#include "search.h"

#include <cctype>

using namespace deadlock::core;

search::search()
{
	
}

data::secure_string_ptr search::make_acronym(const data::secure_string& words) const
{
	// Start with an empty string
	data::secure_string_ptr acronym = data::make_secure_string();

	bool inside_word = false;
	// Loop through the characters
	for (auto i = words.begin(); i != words.end(); i++)
	{
		// TODO: what about UTF-8 etc?
		bool alnum = std::isalnum(*i);

		// At the start of a word, append the character to the acronym.
		if (alnum && !inside_word)
		{
			data::combine_secure_string(acronym, *i);
		}

		inside_word = alnum;
	}

	return acronym;
}