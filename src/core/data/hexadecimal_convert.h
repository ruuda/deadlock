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

#ifndef _DEADLOCK_CORE_DATA_HEXADECIMAL_CONVERT_H_
#define _DEADLOCK_CORE_DATA_HEXADECIMAL_CONVERT_H_

#include <cstdint>
#include <iomanip>

#include "secure_string.h"

namespace deadlock
{
  namespace core
  {
    namespace data
    {
      /// Converts a string byte-by-byte into a hexadecimal string
      inline secure_string_ptr to_hexadecimal_string(const secure_string& plaintext)
      {
        // Create a stream to stream the bytes to
        secure_stringstream_ptr strstr = make_secure_stringstream();

        // Loop through the characters
        for (size_t i = 0; i < plaintext.length(); i++)
        {
          // Convert to 8-bit unsigned integer (characters are signed, which results in problematic behaviour here).
          std::uint8_t byte = plaintext[i];

          // Write to the stream as an integer, so that the byte is not interpreted as a character.
          (*strstr) << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }

        return make_secure_string(strstr->str());
      }
      
      /// Converts a string of hexadecimal text to a string containing the original bytes
      inline secure_string_ptr from_hexadecimal_string(const secure_string& hextext)
      {
        // Create an input stream and output string
        secure_stringstream_ptr strstr = make_secure_stringstream(hextext);
        secure_string_ptr plaintext = make_secure_string();
        secure_string_ptr byte_string = make_secure_string();

        // Read two characters from the stream
        while ((*strstr) >> std::setw(2) >> (*byte_string))
        {
          // Create a new stringstream with the two characters
          secure_stringstream_ptr byte_stream = make_secure_stringstream(*byte_string);
          // Read the integer value (to avoid interpreting the hexadecimal characters byte as two characters)
          int byte; (*byte_stream) >> std::hex >> byte;
          // Convert to a character
          std::uint8_t character = static_cast<std::uint8_t>(byte);
          // And append to the string
          combine_secure_string(plaintext, static_cast<char>(character));
        }

        return plaintext;
      }
    }
  }
}

#endif