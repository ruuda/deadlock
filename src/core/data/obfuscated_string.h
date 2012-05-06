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

#include <string>

#include "../win32_export.h"

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
				std::string obfuscated_data;

				static obfuscated_string empty_obfuscated_string;

			public:

				/// Creates a new string given the already obfuscated data
				obfuscated_string(const std::string& string_of_obfuscated_bytes);

				/// Returns the empty obfuscated string
				static const obfuscated_string& empty();
			};
		}
	}
}

#endif