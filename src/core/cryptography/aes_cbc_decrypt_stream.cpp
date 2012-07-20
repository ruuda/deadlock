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

#include "aes_cbc_decrypt_stream.h"
#include "../errors.h"

using namespace deadlock::core::cryptography;
using namespace deadlock::core::cryptography::detail;
using namespace deadlock::core;
using deadlock::core::crypt_error;

aes_cbc_decrypt_streambuffer::aes_cbc_decrypt_streambuffer(std::basic_istream<char>& istr, const cryptography::key& dkey)
	: key(dkey), input_stream(istr)
{
	// No IV yet
	iv_read = false;

	// Set buffer pointers
	setp(0, 0);
	setg(plaintext, plaintext, plaintext + 0);

	// Set up the crypt key
	int err;
	if ((err = aes_setup(key.get_key(), key.key_size, 0, &skey)) != CRYPT_OK)
		throw crypt_error("Could not initialise AES algorithm: " + std::string(error_to_string(err)));
}

aes_cbc_decrypt_streambuffer::~aes_cbc_decrypt_streambuffer()
{
	// Zero the buffers, so no data remains in memory
	data::detail::secure_memzero(plaintext, block_size);
	data::detail::secure_memzero(iv,        block_size);
	data::detail::secure_memzero(buffer,    block_size);

	// Finalise the crypt key and zero it
	aes_done(&skey); // Do not check return value because throwing from a destructor would make things worse anyway
	data::detail::secure_memzero(&skey, sizeof(symmetric_key));
}

aes_cbc_decrypt_streambuffer::int_type aes_cbc_decrypt_streambuffer::underflow()
{
	// If the underlying stream ended, this one is EOF as well
	if (!input_stream.good()) return traits_type::eof();

	// First of all, read the IV if this is the first block
	if (!iv_read)
	{
		input_stream.read(iv, block_size);
		iv_read = true;

		// Then read the first block of ciphertext
		input_stream.read(ciphertext, block_size);
	}

	// Read one block to the buffer
	input_stream.read(buffer, block_size);
	bool read_last_block = !input_stream.good();

	//  Decrypt one block
	int err;
	if ((err = aes_ecb_decrypt(reinterpret_cast<std::uint8_t*>(ciphertext),
							  reinterpret_cast<std::uint8_t*>(plaintext), &skey)) != CRYPT_OK)
		throw crypt_error("Could not decrypt block: " + std::string(error_to_string(err)));

	// Undo the xor with the IV
	for (size_t i = 0; i < block_size; i++)
	{
		plaintext[i] ^= iv[i]; // TODO: use 4-byte xor
	}

	// Set next IV and ciphertext
	std::memcpy(iv, ciphertext, block_size); // TODO: is memcpy safe?
	std::memcpy(ciphertext, buffer, block_size);

	int out_length = block_size;

	// Remove padding from last block
	if (read_last_block)
	{
		size_t padding_size = plaintext[block_size - 1];
		out_length = block_size - padding_size;

		// Validate that the padding is correct
		for (size_t i = out_length; i < block_size; i++)
		{
			if (plaintext[i] != padding_size)
				throw crypt_error("Encountered invalid padding.");
		}
	}

	// Set the get pointer
	setg(plaintext, plaintext + 0, plaintext + out_length);

	// Return the first character of the new buffer, or eof if the stream ends
	if (out_length > 0) return plaintext[0];
	return traits_type::eof();
}

aes_cbc_decrypt_stream::aes_cbc_decrypt_stream(std::basic_istream<char>& istr, const cryptography::key& dkey)
	: streambuffer(istr, dkey), basic_istream<char>(&streambuffer)
{
		
}

aes_cbc_decrypt_stream::~aes_cbc_decrypt_stream()
{

}
