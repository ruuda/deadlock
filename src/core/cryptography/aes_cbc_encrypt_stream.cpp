// Deadlock – fast search-based password manager
// Copyright (C) 2012–2014 Ruud van Asseldonk

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

#include "aes_cbc_encrypt_stream.h"
#include "../errors.h"

using namespace deadlock::core::cryptography;
using namespace deadlock::core::cryptography::detail;
using namespace deadlock::core;
using deadlock::core::crypt_error;

aes_cbc_encrypt_streambuffer::aes_cbc_encrypt_streambuffer(std::basic_ostream<char>& ostr, const cryptography::key& dkey)
	: key(dkey), output_stream(ostr)
{
	// Generate an initialisation vector
	iv_written = false;
	// TODO: generate a cryptographically random IV
	// Create a random engine and distribution for generating random bytes
	std::random_device random_engine;
	std::uniform_int_distribution<std::uint8_t> random_byte(0x00, 0xff);

	// Simply copy the buffer
	for (size_t i = 0; i < block_size; i++)
	{
		iv[i] = random_byte(random_engine);
	}

	// Set buffer pointers
	setg(0, 0, 0);	
	setp(plaintext, plaintext + block_size);

	// Set up the crypt key
	int err;
	if ((err = aes_setup(key.get_key(), key.key_size, 0, &skey)) != CRYPT_OK)
		throw crypt_error("Could not initialise AES algorithm: " + std::string(error_to_string(err)));
}

aes_cbc_encrypt_streambuffer::~aes_cbc_encrypt_streambuffer()
{
	// Zero the buffers, so no data remains in memory
	data::detail::secure_memzero(plaintext, block_size);
	data::detail::secure_memzero(iv,        block_size);

	// Finalise the crypt key and zero it
	aes_done(&skey); // Do not check return value because throwing from a destructor would make things worse anyway
	data::detail::secure_memzero(&skey, sizeof(symmetric_key));
}

void aes_cbc_encrypt_streambuffer::finalise()
{
	// When overflow is called with eof, it will finalise
	overflow();
}

aes_cbc_encrypt_streambuffer::int_type aes_cbc_encrypt_streambuffer::overflow(int_type new_char)
{
	// First of all, store the IV if this is the first block
	if (!iv_written)
	{
		output_stream.write(iv, block_size);
		iv_written = true;
	}

	enum
	{
		encrypt_run, // Encrypt a full block
		encrypt_finish_pad, // Pad the current buffer to fill the block, and encrypt
		encrypt_finish_pad_block // Encrypt the current block, append a full block of padding, and encrypt
	} action = encrypt_run;

	// Retrieve the number of characters in the buffer
	auto in_length = pptr() - pbase();

	// Determine what to do based on the number of bytes in the buffer and the given character
	if (in_length != block_size)
	{
		if (new_char != traits_type::eof()) throw std::logic_error("A non-block length should only be encountered in the last block.");
		action = encrypt_finish_pad;
	}
	else if (new_char == traits_type::eof())
	{
		action = encrypt_finish_pad_block;
	}

	// Apply PKCS7 padding (if applicable)
	if (action == encrypt_finish_pad)
	{
		size_t missing_bytes = block_size - in_length;
		for (size_t i = 0; i < missing_bytes; i++)
		{
			plaintext[in_length + i] = static_cast<char>(missing_bytes);
		}

		in_length = block_size;
	}

	// Apply CBC
	for (size_t i = 0; i < block_size; i++) // TODO: can iterate over 32 or 64 bits at once instead of 8
	{
		plaintext[i] ^= iv[i];
	}

	// Encrypt to IV (cyphertext of this block is the IV of the next block
	int err;
	if ((err = aes_ecb_encrypt(reinterpret_cast<std::uint8_t*>(plaintext),
							  reinterpret_cast<std::uint8_t*>(iv), &skey)) != CRYPT_OK)
		throw crypt_error("Could not encrypt block: " + std::string(error_to_string(err)));

	// Write to stream
	output_stream.write(iv, block_size);

	// If appending one more block, do so now
	if (action == encrypt_finish_pad_block)
	{
		// Create the PKCS7 padding block
		size_t missing_bytes = block_size;
		for (size_t i = 0; i < missing_bytes; i++)
		{
			plaintext[i] = static_cast<char>(missing_bytes);
		}
		in_length = block_size;

		// Apply CBC
		for (size_t i = 0; i < block_size; i++) // TODO: can iterate over 32 or 64 bits at once instead of 8
		{
			plaintext[i] ^= iv[i];
		}

		// Encrypt to IV (cyphertext of this block is the IV of the next block
		int err;
		if ((err = aes_ecb_encrypt(reinterpret_cast<std::uint8_t*>(plaintext),
								  reinterpret_cast<std::uint8_t*>(iv), &skey)) != CRYPT_OK)
			throw crypt_error("Could not encrypt block: " + std::string(error_to_string(err)));

		// Write to stream
		output_stream.write(iv, block_size);
	}

	// If not done, store the next character in the buffer
	if (action == encrypt_run)
	{
		// Put the character in the buffer
		plaintext[0] = new_char;
		// Reset the pointer (note that the next pointer is plaintext + 1 because there already is a character now)
		setp(plaintext, plaintext + block_size);
		pbump(1);
	}
	
	// Return 0 to indicate success
	return 0;
}

aes_cbc_encrypt_stream::aes_cbc_encrypt_stream(std::basic_ostream<char>& ostr, const cryptography::key& dkey)
	: streambuffer(ostr, dkey), basic_ostream<char>(&streambuffer)
{
		
}

aes_cbc_encrypt_stream::~aes_cbc_encrypt_stream()
{

}

void aes_cbc_encrypt_stream::close()
{
	// Finalise encryption
	streambuffer.finalise();
}
