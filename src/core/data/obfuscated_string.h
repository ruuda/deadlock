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