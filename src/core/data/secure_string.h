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

#ifndef _DEADLOCK_CORE_DATA_SECURE_STRING_H_
#define _DEADLOCK_CORE_DATA_SECURE_STRING_H_

#include <memory>
#include <string>
#include <sstream>
#include <istream>
#include <ostream>
#include "secure_allocator.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			/// A string that zeroes its memory upon deallocation
			typedef std::basic_string<char, std::char_traits<char>, detail::secure_allocator<char>> secure_string;

			/// A stringstream that zeroes its memory upon deallocation
			typedef std::basic_stringstream<char, std::char_traits<char>, detail::secure_allocator<char>> secure_stringstream;

			/// Shared pointer to a secure string
			typedef std::shared_ptr<secure_string> secure_string_ptr;

			/// Shared pointer to a secure stringstream
			typedef std::shared_ptr<secure_stringstream> secure_stringstream_ptr;

			/// Constructs an empty secure string
			inline secure_string_ptr make_secure_string()
			{
				return std::allocate_shared<secure_string>(detail::secure_allocator<secure_string>());
			}

			/// Constructs a secure string from a character array
			inline secure_string_ptr make_secure_string(const char* ptr)
			{
				return std::allocate_shared<secure_string>(detail::secure_allocator<secure_string>(), ptr);
			}

			/// Creates a shared pointer that points to a copy of the secure string
			inline secure_string_ptr make_secure_string(const secure_string& str)
			{
				return std::allocate_shared<secure_string>(detail::secure_allocator<secure_string>(), str);
			}

			/// Creates a secure string that contains one character
			inline secure_string_ptr make_secure_string(char c)
			{
				return std::allocate_shared<secure_string>(detail::secure_allocator<secure_string>(), 1, c);
			}

			/// Prepends the character to the string and returns the shared pointer to the string
			/// This modifies the stored string; it does not act on a copy
			inline secure_string_ptr combine_secure_string(char c, secure_string_ptr str)
			{
				str->insert(str->begin(), c); return str;
			}

			/// Appends the character to the string and returns the shared pointer to the string
			/// This modifies the stored string; it does not act on a copy
			inline secure_string_ptr combine_secure_string(secure_string_ptr str, char c)
			{
				str->push_back(c); return str;
			}

			/// Constructs an empty secure stringstream
			inline secure_stringstream_ptr make_secure_stringstream()
			{
				return std::allocate_shared<secure_stringstream>(detail::secure_allocator<secure_stringstream>());
			}

			/// Constructs a secure stringstream with the specified contents
			inline secure_stringstream_ptr make_secure_stringstream(const secure_string& str)
			{
				return std::allocate_shared<secure_stringstream>(detail::secure_allocator<secure_stringstream>(), str);
			}
		}
	}
}

#endif