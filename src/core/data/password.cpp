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

#include "password.h"

#include <ctime>

using namespace deadlock::core::data;

// Initialise empty obfuscated string here to avoid static initialisation fiasco
obfuscated_string obfuscated_string::empty_obfuscated_string = obfuscated_string(std::vector<std::uint8_t>());
password password::empty_password = password(obfuscated_string::empty(), 0x0);

password::password(obfuscated_string password_data, std::int64_t stored_time) :
	obfuscated_password(password_data), store_time(stored_time)
{

}

password::password(obfuscated_string password_data) :
	obfuscated_password(password_data)
{
	// Store with the current time
	// TODO: make sure this is GMT time, independent of the local time (for portability)
	store_time = std::time(nullptr);
}

const password& password::empty()
{
	return empty_password;
}