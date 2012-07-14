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

#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_KEY_GENERATOR_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_KEY_GENERATOR_H_

#include <cstdint>
#include <string>

#include "../win32.h"
#include "../circular_buffer.h"

namespace deadlock
{
	namespace core
	{
		namespace cryptography
		{
			/// Wraps the PKCS5 PBKDF2 key derivation function,
			/// and securely stores a cryptographic key
			class _export key
			{
			public:

				/// The number of bits in the key
				const static size_t key_bits = 256;

				/// The number of bytes in the key
				const static size_t key_size = key_bits / 8;

				/// The number of bytes of salt
				const static size_t salt_size = key_size;

			protected:

				/// The actual key generated
				std::uint8_t key_data[key_size];

				/// The salt used to generate the key
				std::uint8_t salt_data[salt_size];

				/// The number of iterations done to generate the key
				std::uint32_t number_of_iterations;

			public:

				key();

				/// The destructor clears the memory for the key
				~key();

				/// Generates a random salt value
				void set_salt_random();

				/// Returns the current salt
				inline const std::uint8_t* get_salt() const { return salt_data; }

				/// Returns the current salt
				inline std::uint8_t* get_salt() { return salt_data; }

				/// Returns the number of iterations done to generate the key
				inline std::uint32_t get_iterations() const { return number_of_iterations; }

				/// Generates the key using the specified number of iterations
				void generate_key(const data::secure_string& passphrase, std::uint32_t iterations);

				/// Returns the number of iterations required, such that deriving the key takes the specified amount of time (roughly)
				std::uint32_t get_required_iterations(size_t passphrase_length, double seconds);

				/// Returns the generated key
				inline const std::uint8_t* get_key() const { return key_data; }
			};
		}
	}
}

#endif