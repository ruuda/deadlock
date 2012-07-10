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

#ifndef _DEADLOCK_CORE_DATA_OBFUSCATION_H_
#define _DEADLOCK_CORE_DATA_OBFUSCATION_H_

#include <vector>
#include <cstdint>

#include "secure_string.h"
#include "../win32_export.h"
#include "../circular_buffer.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			namespace detail
			{
				/// Obfuscates or de-obfuscates a string using the given obfuscation buffer
				/// This applies a simple byte-by-byte xor
				_export void xor_transform(secure_string_ptr string, circular_buffer_512& obfuscation_buffer);
			}

			/// Obfuscates the string given the obfuscation buffer
			inline void obfuscate(secure_string_ptr string, circular_buffer_512& obfuscation_buffer)
			{
				// Note that internally obfuscation and de-obfuscation both use xor, since it is symmetric.
				// By using a separate function, this can be replaced by an other obfuscation method
				// (like a cipher) easily
				detail::xor_transform(string, obfuscation_buffer);
			}

			/// De-obfuscates the string given the obfuscation buffer
			inline void deobfuscate(secure_string_ptr string, circular_buffer_512& obfuscation_buffer)
			{
				// Note that internally obfuscation and de-obfuscation both use xor, since it is symmetric.
				// By using a separate function, this can be replaced by an other obfuscation method
				// (like a cipher) easily
				detail::xor_transform(string, obfuscation_buffer);
			}
		}
	}
}

#endif
