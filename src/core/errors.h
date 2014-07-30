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

#ifndef _DEADLOCK_CORE_ERRORS_H_
#define _DEADLOCK_CORE_ERRORS_H_

#include <stdexcept>

extern "C"
{
  #include <lzma.h>
}

namespace deadlock
{
  namespace core
  {
    /// Indicates a problem with reading a vault
    class format_error: public std::runtime_error
    {
    public:
      format_error(std::string const& msg) : std::runtime_error(msg) {}
    };

    /// Indicates a version mismatch
    class version_error: public std::runtime_error
    {
    public:
      version_error(std::string const& msg) : std::runtime_error(msg) {}
    };

    /// Indicates a cryptographic problem
    class crypt_error : public std::runtime_error
    {
    public:
      crypt_error(std::string const& msg): std::runtime_error(msg) {}
    };

    /// Indicates a problem with the key
    class key_error: public crypt_error
    {
    public:
      key_error(std::string const& msg) : crypt_error(msg) {}
    };

    /// Indicates that the used key is not the key that was used to encrypt data.
    class incorrect_key_error : public key_error
    {
    public:
      incorrect_key_error(std::string const& msg) : key_error(msg) {}
    };

    /// A problem related to compression or decompression
    class xz_error : public std::runtime_error
    {
    public:

      /// The XZ return value
      lzma_ret error_code;

      inline xz_error(const std::string& msg) : std::runtime_error(msg) {}

      inline xz_error(const std::string& msg, lzma_ret err) : std::runtime_error(msg)
      {
        error_code = err;
      }
    };
  }
}

#endif