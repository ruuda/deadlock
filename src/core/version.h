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

#ifndef _DEADLOCK_CORE_VERSION_H_
#define _DEADLOCK_CORE_VERSION_H_

#include <cstdint>
#include <ostream>
#include <istream>

#include "win32_export.h"

namespace deadlock
{
	namespace core
	{
		/// Represents a version as a 32-bit integer
		/// 8-bits are reserved for the major number, minor number, revision and build number.
		/// (That means there should not be more than 255 builds of one revision)
		struct _export version
		{
			std::uint8_t major;

			std::uint8_t minor;

			std::uint8_t revision;

			std::uint8_t build;

			/// Empty constructor creates version 1
			version();

			/// Creates a fully specified version
			version(std::int8_t major_number, std::int8_t minor_number, std::int8_t revision_number, std::int8_t build_number);

			/// Writes a version to a stream
			static inline friend std::ostream& operator<<(std::ostream& ostr, version v)
			{
				ostr << static_cast<int>(v.major) << "." << static_cast<int>(v.minor) << "."
					 << static_cast<int>(v.revision) << "." << static_cast<int>(v.build);
				return ostr;
			}

			/// Reads a version from a stream
			static inline friend std::istream& operator>>(std::istream& istr, version& v)
			{
				// Store in int, because we should not deserialise characters, but integers
				int mjr, mnr, rev, bld;

				// Read numbers, skip dots in between
				istr >> mjr; istr.get();
				istr >> mnr; istr.get();
				istr >> rev; istr.get();
				istr >> bld;

				v.major = mjr;
				v.minor = mnr;
				v.revision = rev;
				v.build = bld;

				return istr;
			}
		};

		/// Compares two versions
		_export bool operator<(version v1, version v2);

		/// Compares two versions
		_export bool operator<=(version v1, version v2);
	}
}

#endif