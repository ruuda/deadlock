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

#include "config.h"

#include <cstdlib>

using namespace deadlock::core;

std::string get_env(const char* varname)
{
  const char* value = std::getenv(varname);
  return value ? std::string(value) : std::string();
}

#ifdef _WIN32

std::string config::get_config_file()
{
  std::string home = get_env("USERPROFILE");
  return home + "\\.config\\deadlock.conf";
}

#else

std::string config::get_config_file()
{
  std::string xdg_config = get_env("XDG_CONFIG_HOME");
  std::string home = get_env("HOME");
  std::string config = !xdg_config.empty() ? xdg_config : (home + "/.config");
  return config + "/deadlock.conf";
}

#endif
