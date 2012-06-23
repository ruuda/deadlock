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

using namespace deadlock::core;
using namespace deadlock::core::data;

const password& entry::get_password() const
{
	// If the collection is empty, return a new, empty password
	if (passwords.empty()) return password::empty();

	// Otherwise return the password that is first in the list (this should be the most recent one)
	return passwords.front();
}

void entry::set_password(const obfuscated_string& new_password)
{
	// Add the new password to be the first password in the list, because it will have the latest timestamp (now)
	passwords.insert(passwords.begin(), password(new_password));
}

void entry::deserialise_common(const serialisation::json_value::object_t& json_data)
{
	// At least, a key and passwords should be present
	if (json_data.find("key") == json_data.end()) throw format_error("No key present for entry.");
	if (json_data.find("passwords") == json_data.end()) throw format_error("No passwords present in entry.");

	// Read the key
	key = static_cast<std::string&>(json_data.at("key"));

	// Read the username (if present)
	if (json_data.find("username") != json_data.end())
	{
		username = static_cast<std::string&>(json_data.at("username"));
	}

	// Read additional data (if present)
	if (json_data.find("additional_data") != json_data.end())
	{
		additional_data = static_cast<std::string&>(json_data.at("additional_data"));
	}
}

void entry::deserialise_obfuscated(const serialisation::json_value::object_t& json_data, circular_buffer_512& transformation_buffer)
{
	// First, read common values
	// This will also make sure a password array is present
	deserialise_common(json_data);

	// Loop through the obfuscated passwords and add them
	const serialisation::json_value::array_t& password_array = json_data.at("passwords");
	for (size_t i = 0; i < password_array.size(); i++)
	{
		const serialisation::json_value::object_t& psswd = password_array[i];

		// Make sure password and timestamp are present
		if (psswd.find("obfuscated_password") == psswd.end()) throw format_error("No obfuscated password data present in password.");
		if (psswd.find("store_time") == psswd.end()) throw format_error("No timestamp present in password.");

		// Read the obfuscated string and transform it to the new obfuscation buffer
		obfuscated_string obfs_psswd(password_array[i]["obfuscated_password"]);
		obfs_psswd.transform(transformation_buffer);

		// Add the password to the list, using obfuscated data and the timestamp
		passwords.push_back(password(obfs_psswd, psswd.at("store_time")));

	}
}

void entry::deserialise_deobfuscated(const serialisation::json_value::object_t& json_data, circular_buffer_512& obfuscation_buffer)
{
	// First, read common values
	// This will also make sure a password array is present
	deserialise_common(json_data);

	// Loop through the unobfuscated passwords and add them
	const serialisation::json_value::array_t& password_array = json_data.at("passwords");
	for (size_t i = 0; i < password_array.size(); i++)
	{
		const serialisation::json_value::object_t& psswd = password_array[i];

		// Make sure password and timestamp are present
		if (psswd.find("password") == psswd.end()) throw format_error("No password data present in password.");
		if (psswd.find("store_time") == psswd.end()) throw format_error("No timestamp present in password.");

		// Add the password to the list, using obfuscated data and the timestamp
		// Note that passwords are present in memory during deserialisation, and the memory is not explicitly cleared either.
		// The passwords were not stored obfuscated in the first place, so that should not be a problem in this case.
		passwords.push_back(password(obfuscated_string(password_array[i]["password"], obfuscation_buffer), psswd.at("store_time")));
	}
}

void entry::serialise_common(serialisation::serialiser& serialiser)
{
	// Write the key
	serialiser.write_object_key("key");
	serialiser.write_string(key);

	// Write the username (if present)
	if (!username.empty())
	{
		serialiser.write_object_key("username");
		serialiser.write_string(username);
	}

	// Write additional data (if present)
	if (!additional_data.empty())
	{
		serialiser.write_object_key("additional_data");
		serialiser.write_string(additional_data);
	}
}

void entry::serialise_obfuscated(serialisation::serialiser& serialiser)
{
	// Write the collection as an object
	serialiser.write_begin_object();
	{
		// Write common values
		serialise_common(serialiser);

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
					// Write the obfuscated password as a hexadecimal string
					serialiser.write_object_key("obfuscated_password");
					serialiser.write_string(passwords[i].get_password().get_hexadecimal_string());

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

void entry::serialise_deobfuscated(serialisation::serialiser& serialiser, circular_buffer_512& obfuscation_buffer)
{
	// Write the collection as an object
	serialiser.write_begin_object();
	{
		// Write common values
		serialise_common(serialiser);

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
					// Write the obfuscated password as a hexadecimal string
					serialiser.write_object_key("password");

					{
						// Retrieve the actual timestamp
						deobfuscated_string password = passwords[i].get_password().deobfuscate(obfuscation_buffer);

						// TODO: could this leave the password in memory (because of std::string temporary)?
						serialiser.write_string(password.c_str());
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
