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

#include "version.h"

using namespace deadlock::core;

version::version()
{
  major = 1;
  minor = 0;
  revision = 0;
  build = 0;
}

version::version(std::int8_t major_number, std::int8_t minor_number, std::int8_t revision_number, std::int8_t build_number)
{
  major = major_number;
  minor = minor_number;
  revision = revision_number;
  build = build_number;
}

bool deadlock::core::operator<(version v1, version v2)
{
  if (v1.major < v2.major) return true; if (v1.major > v2.major) return false;
  if (v1.minor < v2.minor) return true; if (v1.minor > v2.minor) return false;
  if (v1.revision < v2.revision) return true; if (v1.revision > v2.revision) return false;
  if (v1.build < v2.build) return true; if (v1.build > v2.build) return true;

  // Equality is not less-than
  return false;
}

bool deadlock::core::operator<=(version v1, version v2)
{
  if (v1.major < v2.major) return true; if (v1.major > v2.major) return false;
  if (v1.minor < v2.minor) return true; if (v1.minor > v2.minor) return false;
  if (v1.revision < v2.revision) return true; if (v1.revision > v2.revision) return false;
  if (v1.build < v2.build) return true; if (v1.build > v2.build) return true;

  // Equality is less-than or equal
  return true;
}
