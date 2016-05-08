// Deadlock – fast search-based password manager
// Copyright (C) 2016 Ruud van Asseldonk

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

#ifndef _DEADLOCK_CORE_CONFIG_H_
#define _DEADLOCK_CORE_CONFIG_H_

#include <string>

namespace deadlock
{
  namespace core
  {
    /// Deals with the config file.
    class config
    {
    public:
      /// Returns the filename of the config file. On Unix platforms this is
      /// likely ~/.config/deadlock.conf.
      static std::string get_config_file();

      /// Sets filename to the contents of the config file, if it exists,
      /// and returns true. Returns false if no vault is configured.
      static bool get_vault_file(std::string& filename);
    };
  }
}

#endif
