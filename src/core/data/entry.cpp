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

#include "entry.h"

#include "../errors.h"
#include "hexadecimal_convert.h"

using namespace deadlock::core;
using namespace deadlock::core::data;

entry::entry()
{
	key = make_secure_string();
	username = make_secure_string();
	additional_data = make_secure_string();
}

entry::entry(const entry& other)
	:
	// Copy the values; shared pointers are only used to manage per-instance storage,
	// not to share ownership between instances.
	key(make_secure_string(other.get_key())),
	username(make_secure_string(other.get_username())),
	additional_data(make_secure_string(other.get_additional_data())),
	passwords(other.passwords_begin(), other.passwords_end())
{
}

const password& entry::get_password() const
{
	// If the collection is empty, return a new, empty password
	if (passwords.empty()) return password::empty();

	// Otherwise return the password that is first in the list (this should be the most recent one)
	return passwords.front();
}

void entry::set_password(const secure_string& new_password)
{
	// Add the new password to be the first password in the list, because it will have the latest timestamp (now)
	passwords.insert(passwords.begin(), password(new_password));
}

void entry::deserialise(const serialisation::json_value::object_t& json_data)
{
	// At least, a key and passwords should be present
	if (json_data.find("key") == json_data.end() &&
		json_data.find("key_hexadecimal") == json_data.end())
		throw format_error("No key present for entry.");

	if (json_data.find("passwords") == json_data.end())
		throw format_error("No passwords present in entry.");

	// Read the key
	if (json_data.find("key") != json_data.end())
	{
		key = make_secure_string(json_data.at("key"));
	}
	else // Read hexadecimal key
	{
		// No need to make a copy here, this instance simply takes over ownership from the temporary
		key = from_hexadecimal_string(json_data.at("key_hexadecimal"));
	}

	// Read the username (if present)
	if (json_data.find("username") != json_data.end())
	{
		username = make_secure_string(json_data.at("username"));
	}
	else if (json_data.find("username_hexadecimal") != json_data.end()) // Read hexadecimal password
	{
		// No need to make a copy here, this instance simply takes over ownership from the temporary
		username = from_hexadecimal_string(json_data.at("username_hexadecimal"));
	}

	// Read additional data (if present)
	if (json_data.find("additional_data") != json_data.end())
	{
		additional_data = data::make_secure_string(json_data.at("additional_data"));
	}
	else if (json_data.find("additional_data_hexadecimal") != json_data.end()) // Read hexadecimal password
	{
		// No need to make a copy here, this instance simply takes over ownership from the temporary
		additional_data = from_hexadecimal_string(json_data.at("additional_data_hexadecimal"));
	}

	// Loop through the passwords and add them
	const serialisation::json_value::array_t& password_array = json_data.at("passwords");
	for (size_t i = 0; i < password_array.size(); i++)
	{
		const serialisation::json_value::object_t& psswd = password_array[i];

		// Make sure password and timestamp are present
		if (psswd.find("store_time") == psswd.end())
			throw format_error("No timestamp present in password.");
		if (psswd.find("password") == psswd.end() &&
			psswd.find("password_hexadecimal") == psswd.end())
			throw format_error("No password data present in password.");
		
		secure_string_ptr password_str;

		if (psswd.find("password") != psswd.end()) // Read normal password
		{
			password_str = make_secure_string(psswd.at("password"));
		}
		else // Read hexadecimal string password
		{
			// No need to make a copy here, this instance simply takes over ownership from the temporary
			password_str = from_hexadecimal_string(psswd.at("password_hexadecimal"));
		}

		// Add the password to the list
		passwords.push_back(password(*password_str, psswd.at("store_time")));
	}
}

void entry::serialise(serialisation::serialiser& serialiser, bool obfuscation)
{
	serialiser.write_begin_object();
	{
		if (obfuscation)
		{
			// Write the key
			serialiser.write_object_key("key_hexadecimal");
			serialiser.write_string(*to_hexadecimal_string(*key));

			// Write the username (if present)
			if (!username->empty())
			{
				serialiser.write_object_key("username_hexadecimal");
				serialiser.write_string(*to_hexadecimal_string(*username));
			}

			// Write additional data (if present)
			if (!additional_data->empty())
			{
				serialiser.write_object_key("additional_data_hexadecimal");
				serialiser.write_string(*to_hexadecimal_string(*additional_data));
			}
		}
		else
		{
			// Write the key
			serialiser.write_object_key("key");
			serialiser.write_string(*key);

			// Write the username (if present)
			if (!username->empty())
			{
				serialiser.write_object_key("username");
				serialiser.write_string(*username);
			}

			// Write additional data (if present)
			if (!additional_data->empty())
			{
				serialiser.write_object_key("additional_data");
				serialiser.write_string(*additional_data);
			}
		}

		// Now write all passwords
		serialiser.write_object_key("passwords");
		serialiser.write_begin_array();
		{
			// Loop though the passwords
			for (size_t i = 0; i < passwords.size(); i++)
			{
				// Write the password as an object
				serialiser.write_begin_object();
				{
					if (obfuscation)
					{
						// Write the hexadecimal password to "password_hexadecimal"
						serialiser.write_object_key("password_hexadecimal");
						serialiser.write_string(*to_hexadecimal_string(passwords[i].get_password()));
					}
					else
					{
						// Write the password as-is
						serialiser.write_object_key("password");
						serialiser.write_string(passwords[i].get_password());
					}

					// Write the timestamp associated with the password
					serialiser.write_object_key("store_time");
					serialiser.write_number(passwords[i].get_stored_time());
				}
				serialiser.write_end_object();
			}
		}
		serialiser.write_end_array();
	}
	serialiser.write_end_object();
}