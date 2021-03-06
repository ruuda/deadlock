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

#ifndef _DEADLOCK_CORE_DATA_PASSWORD_H_
#define _DEADLOCK_CORE_DATA_PASSWORD_H_

#include <cstdint>

#include "secure_string.h"

namespace deadlock
{
  namespace core
  {
    namespace data
    {
      /// Keeps a password and the date-time it was stored
      class password
      {
      protected:

        /// The time the password was stored, as the number of seconds since epoch (1970-01-01 00:00)
        std::int64_t store_time;

        /// The actual password data
        secure_string_ptr password_string;

        static password empty_password;

      public:

        /// Re-constructs a password based on the given data
        /// Should be used for loading only
        password(const secure_string& password_data, std::int64_t stored_time);

        /// Constructs a new password with its store_time set to the current time
        password(const secure_string& password_data);

        /// Copy constructor
        password(const password& other);

        /// Returns the time at which the password was stored
        inline std::int64_t get_stored_time() const { return store_time; }

        /// Returns the password
        inline const secure_string& get_password() const { return *password_string; }

        /// Returns the empty password
        static const password& empty();
      };
    }
  }
}

#endif