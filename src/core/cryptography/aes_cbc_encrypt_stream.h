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

#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_AES_CBC_ENCRYPT_STREAM_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_AES_CBC_ENCRYPT_STREAM_H_

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
				/// The encryption streamuffer that encrypts data using 256-bit key AES in CBC mode with PKCS7 padding
				class aes_cbc_encrypt_streambuffer : public std::basic_streambuf<char>
				{
				protected:

					/// The stream that all compressed data will be streamed to
					std::basic_ostream<char>& output_stream;

					/// AES has a block size of 16 bytes
					static const size_t block_size = 16;

					/// Buffer for the initialisation vector, or previous block when initialisation is done
					/// Should be of block size (16 bytes)
					char iv[block_size];

					/// Buffer for non-encrypted data
					char plaintext[block_size];

					/// The key used for encryption
					const cryptography::key& key;

					/// Whether the initialisation vector has been written already
					bool iv_written;

					/// The LibTomCrypt scheduled key
					symmetric_key skey;

				public:

					/// Creates a streambuffer that streams its output to the given stream,
					/// and encrypts with the given key and initialisation vector
					aes_cbc_encrypt_streambuffer(std::basic_ostream<char>& ostr, const cryptography::key& dkey);

					/// Zeroes the buffers
					virtual ~aes_cbc_encrypt_streambuffer();

					/// Adds padding to the current block and encrypts and writes that block
					virtual void finalise();

				protected:

					/// Once the buffer (one block) is full, encrypts the data and writes it to the underlying buffer
					virtual int_type overflow(int_type new_char = traits_type::eof());
				};
			}

			/// A stream that encrypts the data written to it, and writes that data to the underlying stream
			/// The stream uses 256-bit key AES in CBC mode with a random initialisation vector
			class aes_cbc_encrypt_stream : public std::basic_ostream<char>
			{
			protected:

				/// The streambuffer responsible for the actual encryption
				detail::aes_cbc_encrypt_streambuffer streambuffer;

			public:

				/// Creates a compression stream that streams its output to the given stream,
				/// with the given key. The key must outlive the stream object.
				aes_cbc_encrypt_stream(std::basic_ostream<char>& ostr, const cryptography::key& dkey);

				~aes_cbc_encrypt_stream();

				/// Adds padding, and flushes the remaining block(s)
				void close();
			};
		}
	}
}

#endif