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
#include <memory>

#include "password.h"

#include "../circular_buffer.h"
#include "../serialisation/value.h"
#include "../serialisation/serialiser.h"
#include "secure_allocator.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			class entry;

			typedef std::shared_ptr<entry> entry_ptr;

			/// An entry for the collection, containing a password and username
			/// This contains one single item for the collection (identified by a key),
			/// A collection of passwords associated with it,
			/// a username and additional data
			class entry
			{

			public:

				typedef std::vector<password, data::detail::secure_allocator<password>> password_collection;
				typedef password_collection::const_iterator password_iterator;

			protected:

				/// The string used to identify this entry.
				/// This will be the name of a website or service in most cases.
				/// The identifier need not be unique.
				secure_string_ptr id;

				/// A list of passwords associated with the key
				/// The most recent one is the "best" passwords, but older passwords are stored by means of a backup.
				password_collection passwords;

				/// The username associated with the password.
				secure_string_ptr username;

				/// Can be used to store additional information with the key.
				secure_string_ptr additional_data;

			public:

				/// Constructs an entry with empty password and other values.
				entry();

				/// Copy constructor
				entry(const entry& other);

				/// Returns the identifier associated with this entry
				inline const secure_string& get_id() const { return *id; }

				/// Modifies the identifier associated with this entry
				inline void set_id(const secure_string& new_id) { id = make_secure_string(new_id); }

				/// Returns an iterator to the first password
				inline password_iterator passwords_begin() const { return passwords.begin(); }

				/// Returns an iterator past the last password
				inline password_iterator passwords_end() const { return passwords.end(); }

				/// Returns the current (most recent) password
				const password& get_password() const;

				/// Appends a new password to the list of passwords, making it the current password
				void set_password(const secure_string& new_password);

				/// Returns the username associated with the key
				inline const secure_string& get_username() const { return *username; }

				/// Mofifies the username associated with the key
				inline void set_username(const secure_string& new_username) { username = make_secure_string(new_username); }

				/// Returns additional data associated with the key
				inline const secure_string& get_additional_data() const { return *additional_data; }

				/// Modifies additional data associated with the key
				inline void set_additional_data(const secure_string& new_data) { additional_data = make_secure_string(new_data); }

				/// Reconstructs the entries given the JSON data
				void deserialise(const serialisation::json_value::object_t& json_data);

				/// Writes the entry to the serialiser as object.
				/// If obfuscation is true, this will write the data as hexadecimal strings.
				/// If it is false, it will write the data "as-is".
				void serialise(serialisation::serialiser& serialiser, bool obfuscation);
			};

			/// Constructs an empty entry with the secure allocator
			inline entry_ptr make_entry()
			{
				return std::allocate_shared<entry>(detail::secure_allocator<entry>());
			}

			/// Copies an entry with the secure allocator
			inline entry_ptr make_entry(const entry& other)
			{
				return std::allocate_shared<entry>(detail::secure_allocator<entry>(), other);
			}
		}
	}
}

#endif
