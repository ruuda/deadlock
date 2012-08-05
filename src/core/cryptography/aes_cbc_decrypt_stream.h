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

#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_AES_CBC_DECRYPT_STREAM_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_AES_CBC_DECRYPT_STREAM_H_

#include <cstdint>
#include <stdexcept>
#include <streambuf>
#include <ostream>

extern "C"
{
	#include <tomcrypt.h>
}

#include "key.h"

namespace deadlock
{
	namespace core
	{
		namespace cryptography
		{
			namespace detail
			{
				/// The decryption streamuffer that encrypts data using 256-bit key AES in CBC mode with PKCS7 padding
				class aes_cbc_decrypt_streambuffer : public std::basic_streambuf<char>
				{
				protected:

					/// The stream that all encrypted data will be read from
					std::basic_istream<char>& input_stream;

					/// AES has a block size of 16 bytes
					static const size_t block_size = 16;

					/// Buffer for the initialisation vector, or previous block when initialisation is done
					char iv[block_size];

					/// Buffer for non-encrypted data
					char plaintext[block_size];

					/// Holds one block of ciphertext
					char ciphertext[block_size];

					/// A buffer of one block in advance, to detected EOF
					char buffer[block_size];

					/// The key used for encryption
					const cryptography::key& key;

					/// Whether the initialisation vector has been read already
					bool iv_read;

					/// The LibTomCrypt scheduled key
					symmetric_key skey;

				public:

					/// Creates a streambuffer that reads its input from to the given stream,
					/// and decrypts with the given key
					aes_cbc_decrypt_streambuffer(std::basic_istream<char>& istr, const cryptography::key& key);

					/// Zeroes the buffers
					virtual ~aes_cbc_decrypt_streambuffer();

				protected:

					/// Once the buffer (one block) is empty, reads a new block and decrypts the data
					virtual int_type underflow();
				};
			}

			/// A stream that decrypts the data read from it, and reads the encrypted data from the underlying stream
			/// The stream uses 256-bit key AES in CBC mode.
			class aes_cbc_decrypt_stream : public std::basic_istream<char>
			{
			protected:

				/// The streambuffer responsible for the actual decryption
				detail::aes_cbc_decrypt_streambuffer streambuffer;

			public:

				/// Creates a compression stream that reads it input from the given stream,
				/// with the given key. The key must outlive the stream object.
				aes_cbc_decrypt_stream(std::basic_istream<char>& istr, const cryptography::key& key);

				~aes_cbc_decrypt_stream();
			};
		}
	}
}

#endif