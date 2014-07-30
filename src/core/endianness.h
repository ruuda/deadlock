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

#ifndef _DEADLOCK_CORE_ENDIANNESS_H_
#define _DEADLOCK_CORE_ENDIANNESS_H_

// With Visual C++, use intrin.h functions for byte swapping
#ifdef _MSC_VER

#include <intrin.h>

#define DEADLOCK_BYTESWAP_32(x) _byteswap_ulong(x);

#endif // _MSC_VER

// With GCC use the built-in intrinsics
#ifdef __GNUC__

#define DEADLOCK_BYTESWAP_32(x) __builtin_bswap32(x);

#endif // __GNUC__

#ifdef __clang__

// TODO: what about clang?

#endif // __clang__

#include <cstdint>

namespace deadlock
{
  namespace core
  {
    /// Converts an integer to big-endian notatation
    /// On big-endian systems, this does nothing.
    /// On little-endian systems, this swaps around the bytes.
    inline std::uint32_t internal_to_portable(std::uint32_t internal_integer)
    {
      // TODO: can the run-time check be replaced with compile-time macros?
      // Optionally integrate with Crypto++ endianness detection as well
      union
      {
        std::uint32_t i;
        std::uint8_t  c[4];
      }
      endianness = { 0x01020304 };

      bool big_endian = (endianness.c[0] == 0x01);

      return big_endian ? internal_integer : DEADLOCK_BYTESWAP_32(internal_integer);
    }

    /// Converts a big-endian integer to internal notatation
    /// On big-endian systems, this does nothing.
    /// On little-endian systems, this swaps around the bytes.
    inline std::uint32_t portable_to_internal(std::uint32_t portable_integer)
    {
      // TODO: can the run-time check be replaced with compile-time macros?
      // Optionally integrate with Crypto++ endianness detection as well
      union
      {
        std::uint32_t i;
        std::uint8_t  c[4];
      }
      endianness = { 0x01020304 };

      bool big_endian = (endianness.c[0] == 0x01);

      return big_endian ? portable_integer : DEADLOCK_BYTESWAP_32(portable_integer);
    }
  }
}

#endif