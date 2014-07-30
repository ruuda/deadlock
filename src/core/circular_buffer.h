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

#ifndef _DEADLOCK_CORE_CIRCULAR_BUFFER_H_
#define _DEADLOCK_CORE_CIRCULAR_BUFFER_H_

#include <cstdint>
#include <random>
#include <ctime>
#include <iomanip>

#include "data/secure_string.h"

namespace deadlock
{
  namespace core
  {
    /// Holds a certain amount of bytes, and provides bytes on request.
    /// When the last byte is read, it starts again at the beginning.
    template <size_t buffer_size> class circular_buffer
    {
    protected:

      /// The raw data
      std::uint8_t buffer[buffer_size];

      /// Index into the byte array.
      /// Points before the next byte that will be read
      size_t cursor;

    public:

      /// Does not initialise the buffer, instantiates it with undefined content
      circular_buffer()
      {
        cursor = 0;
      }

      /// Fills the buffer with random bytes
      inline void fill_random()
      {
        // TODO: read from /dev/urandom where possible, or maybe use other system-provided entropy on other platforms
        // TODO: get a better random seed, or at least share with the key generator
        std::mt19937 random_engine;
        random_engine.seed(std::time(nullptr));
        std::uniform_int_distribution<std::uint8_t> random_byte(0x00, 0xff);

        for (size_t i = 0; i < buffer_size; i++)
        {
          buffer[i] = random_byte(random_engine);
        }
      }

      /// Turns the buffer into a transformation buffer that transforms from its current content to the other buffer
      inline void transform(circular_buffer<buffer_size>& other)
      {
        // Reset other, so beginnings line up
        other.reset();

        // Loop through the bytes and apply xor
        for (size_t i = 0; i < buffer_size; i++)
        {
          buffer[i] ^= other.next();
        }
      }

      /// Returns the buffer size
      size_t size() const { return buffer_size; }

      /// Resets the cursor to the beginning of the buffer
      void reset()
      {
        cursor = 0;
      }

      /// Returns the next byte from the buffer and advances the cursor
      std::uint8_t next()
      {
        std::uint8_t byte = buffer[cursor];
          
        cursor++; if (cursor >= buffer_size) cursor = 0;

        return byte;
      }

      /// Returns a hexadecimal representation of the buffer
      data::secure_string_ptr get_hexadecimal_string() const
      {
        data::secure_stringstream_ptr hex_string = data::make_secure_stringstream();

        // Write all bytes as hexadecimal characters
        for (size_t i = 0; i < buffer_size; i++)
        {
          (*hex_string) << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(buffer[i]);
        }

        return data::make_secure_string(hex_string->str());
      }

      /// Fills the buffer with data from a hexadecimal string
      void set_hexadecimal_string(const data::secure_string& hexadecimal_string)
      {
        data::secure_stringstream_ptr hex_string = data::make_secure_stringstream(hexadecimal_string);

        data::secure_string_ptr byte_string = data::make_secure_string();

        size_t i = 0;

        // Read two characters from the stream
        while (((*hex_string) >> std::setw(2) >> (*byte_string)) && (i < buffer_size))
        {
          // Create a new stringstream with the two characters
          data::secure_stringstream_ptr byte_stream = data::make_secure_stringstream(*byte_string);
          // Read the integer value
          int byte; (*byte_stream) >> std::hex >> byte;
          buffer[i] = static_cast<std::uint8_t>(byte);
          i++;
        }
      }
    };

    /// A 512-bit circular buffer
    typedef circular_buffer<64> circular_buffer_512;
  }
}

#endif
