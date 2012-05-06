#include "entry.h"

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