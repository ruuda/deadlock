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