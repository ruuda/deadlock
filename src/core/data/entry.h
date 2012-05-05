#ifndef _DEADLOCK_CORE_DATA_ENTRY_H_
#define _DEADLOCK_CORE_DATA_ENTRY_H_

#include <vector>
#include "password.h"

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
			class entry
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
			};
		}
	}
}

#endif