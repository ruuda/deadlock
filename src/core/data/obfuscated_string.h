#ifndef _DEADLOCK_CORE_DATA_OBFUSCATED_STRING_H_
#define _DEADLOCK_CORE_DATA_OBFUSCATED_STRING_H_

#include <string>

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
			class obfuscated_string
			{
			protected:

				/// The string of obfuscated bytes
				std::string obfuscated_string;

			public:


			};
		}
	}
}

#endif