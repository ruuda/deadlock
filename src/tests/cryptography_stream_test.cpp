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

#include "cryptography_stream_test.h"
#include "../core/core.h"
#include "../core/cryptography/aes_cbc_encrypt_stream.h"
//#include "../core/cryptography/xz_decompress_stream.h"

#include <stdexcept>
#include <sstream>

using namespace deadlock::core;
using namespace deadlock::tests;

std::string cryptography_stream_test::get_name()
{
	return "cryptography_stream";
}

void cryptography_stream_test::run()
{
	// Create a key
	cryptography::key_generator key;
	key.set_salt_random();
	std::string passphrase = "correct horse battery staple";
	size_t iterations = key.get_required_iterations(passphrase.length(), 0.1);
	key.generate_key(passphrase, iterations);

	// Pass 1: 16 bytes IV + 8 characters + 8 bytes padding
	{
		// The output stream
		std::stringstream strstream;
		// The cryptostream
		cryptography::aes_cbc_encrypt_stream enc_stream(strstream, key);
		// Write some data
		enc_stream << "12345678";
		enc_stream.flush();

		// Validate length
		std::string ciphertext = strstream.str();
		if (ciphertext.length() != 32)
			throw std::runtime_error("Unexpected output size; should be 32.");

		// TODO: decrypt, validate content
	}
	
	// Pass 2: 16 bytes IV + 16 characters + 16 bytes padding
	{
		// The output stream
		std::stringstream strstream;
		// The cryptostream
		cryptography::aes_cbc_encrypt_stream enc_stream(strstream, key);
		// Write some data
		enc_stream << "abcdefghijklmnop";
		enc_stream.flush();

		// Validate length
		std::string ciphertext = strstream.str();
		if (ciphertext.length() != 48)
			throw std::runtime_error("Unexpected output size; should be 48.");

		// TODO: decrypt, validate content
	}

	// Pass 3: 16 bytes IV + 250 characters + 6 bytes padding
	{
		// The output stream
		std::stringstream strstream;
		// The cryptostream
		cryptography::aes_cbc_encrypt_stream enc_stream(strstream, key);
		// Write some data
		for (std::uint8_t i = 0; i < 250; i++)
		{
			enc_stream.put(i);
		}
		enc_stream.flush();

		// Validate length
		std::string ciphertext = strstream.str();
		if (ciphertext.length() != 272)
			throw std::runtime_error("Unexpected output size; should be 272.");

		// TODO: decrypt, validate content
	}
}
