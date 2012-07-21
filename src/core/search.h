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

#ifndef _DEADLOCK_CORE_SEARCH_H_
#define _DEADLOCK_CORE_SEARCH_H_

#include <queue>

#include "win32.h"
#include "data/secure_string.h"
#include "data/entry.h"

namespace deadlock
{
	namespace core
	{
		namespace detail
		{
			// A helper structure for the search algorithm
			struct _export entry_match
			{
				data::entry_ptr entry;
				int probability;
			};

			// By defining this operator, std::less works, and this allows the priority queue to work
			inline bool operator<(const entry_match& m1, const entry_match& m2)
			{
				return m1.probability < m2.probability;
			}
		}

		/// Contains the search logic that intelligently
		/// identifies entries based on a search string
		class _export search
		{
			protected:

				/// Returns a one-word string that is the acronym of the words provided
				data::secure_string_ptr make_acronym(const data::secure_string& words) const;

				/// Returns the lowercase version of the string
				inline data::secure_string_ptr tolower(const data::secure_string& str) const
				{
					data::secure_string_ptr lower = data::make_secure_string(str);
					// Transform every character to lowercase
					// TODO: UTF-8 tolower, and what about the locale?
					// Is there a better way to do this?
					std::for_each(lower->begin(), lower->end(), [](char& c) { c = std::tolower(c, std::locale()); });

					return lower;
				}

			public:

				/// Creates a new search algorithm
				search();

				template <typename IndirectIterator, typename OutputIterator>
				void find_matches(const data::secure_string& query, IndirectIterator begin, IndirectIterator end, OutputIterator output_iterator) const
				{
					std::priority_queue<detail::entry_match> matches;

					// First of all, make the search string lowercase
					data::secure_string_ptr query_lower = tolower(query);

					// Now search through the entries
					for (IndirectIterator i = begin; i != end; i++)
					{
						// Start with 0 probability that this is the entry the user meant.
						int probability = 0;

						// Make the identifier lowercase as well
						data::secure_string_ptr id_lower = tolower(i->get_id());

						// If the match is perfect, set a high probability
						if (*id_lower == *query_lower)
						{
							probability += 50;

							// If the match is case-sensistive, the probability is even higher
							if (i->get_id() == query) probability += 50;
						}

						// If a match was found, add it to the queue
						if (probability > 0)
						{
							detail::entry_match match;
							match.entry = (*i.base());
							match.probability = probability;

							matches.push(match);
						}
					}

					// Copy the found matches in the correct order
					while (!matches.empty())
					{
						output_iterator = matches.top().entry;
						output_iterator++;
						matches.pop();
					}
				}
		};
	}
}

#endif