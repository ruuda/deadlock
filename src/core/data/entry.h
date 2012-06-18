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

#ifndef _DEADLOCK_CORE_DATA_ENTRY_H_
#define _DEADLOCK_CORE_DATA_ENTRY_H_

#include <vector>

#include "password.h"

#include "../win32_export.h"
#include "../circular_buffer.h"
#include "../serialisation/value.h"
#include "../serialisation/serialiser.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			/// An entry for the collection, containing a password and username
			/// This contains one single item for the collection (identified by a key),
			/// A collection of passwords associated with it,
			/// a username and additional data
			class _export entry
			{
			public:

				typedef std::vector<password> password_collection;
				typedef password_collection::const_iterator password_iterator;

			protected:

				/// The key used to identify this entry.
				/// This will be the name of a website or service in most cases
				std::string key;

				/// A list of passwords associated with the key
				/// The most recent one is the "best" passwords, but older passwords are stored by means of a backup
				password_collection passwords;

				/// The username associated with the password
				std::string username;

				/// Can be used to store additional information with the key
				std::string additional_data;

				/// Serialises all values common to obfuscated and de-obfuscated serialisation
				/// This does not write the enclosing object
				void serialise_common(serialisation::serialiser& serialiser);

				/// Deserialises all values common to obfuscated and de-obfuscated serialisation
				void deserialise_common(const serialisation::json_value::object_t& json_data);

			public:

				/// Returns the key associated with this entry
				inline const std::string& get_key() const { return key; }

				/// Modifies the key associated with this entry
				inline void set_key(const std::string& new_key) { key = new_key; }

				/// Returns an iterator to the first password
				inline password_iterator passwords_begin() const { return passwords.begin(); }

				/// Returns an iterator past the last password
				inline password_iterator passwords_end() const { return passwords.end(); }

				/// Returns the current (most recent) password
				inline const password& get_password() const;

				/// Appends a new password to the list of passwords, making it the current password
				void set_password(const obfuscated_string& new_password);

				/// Returns the username associated with the key
				inline const std::string& get_username() const { return username; }

				/// Mofifies the username associated with the key
				inline void set_username(const std::string& new_username) { username = new_username; }

				/// Returns additional data associated with the key
				inline const std::string& get_additional_data() const { return additional_data; }

				/// Modifies additional data associated with the key
				inline void set_additional_data(const std::string& new_data) { additional_data = new_data; }

				/// Reconstructs the entries given the JSON data, assuming obfuscated data
				void deserialise_obfuscated(const serialisation::json_value::object_t& json_data, circular_buffer_512& tranformation_buffer);

				/// Reconstructs the entries given the JSON data, assuming unobfuscated data
				/// The data will then be stored in an obfuscated way using the provided buffer
				void deserialise_deobfuscated(const serialisation::json_value::object_t& json_data, circular_buffer_512& obfuscation_buffer);

				/// Writes the entries to the serialiser as an array of objects
				/// This will write the passwords as hexadecimal strings of the obfuscated data
				void serialise_obfuscated(serialisation::serialiser& serialiser);

				/// Writes the entries to the serialiser as an array of objects
				/// This will write the passwords "as-is", after deobfuscation with the buffer
				void serialise_deobfuscated(serialisation::serialiser& serialiser, circular_buffer_512& obfuscation_buffer);
			};
		}
	}
}

#endif