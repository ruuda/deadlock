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
password password::empty_password = password("", 0x0);

password::password(const secure_string& password_str, std::int64_t stored_time) :
  password_string(make_secure_string(password_str)), store_time(stored_time)
{

}

password::password(const secure_string& password_str) :
  password_string(make_secure_string(password_str))
{
  // Store with the current time
  // TODO: make sure this is GMT time, independent of the local time (for portability)
  store_time = std::time(nullptr);
}

password::password(const password& other)
  :
  // Copy the data, because the shared_ptr is only used for secure memory erasing.
  password_string(make_secure_string(other.get_password())),
  store_time(other.get_stored_time())
{

}

const password& password::empty()
{
  return empty_password;
}