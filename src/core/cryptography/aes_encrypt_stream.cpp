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

#include "aes_encrypt_stream.h"

using namespace deadlock::core::cryptography;
using namespace deadlock::core::cryptography::detail;

aes_encrypt_streambuffer::aes_encrypt_streambuffer(std::basic_ostream<char>& ostr, key_generator& deobfuscated_key)
	: key(deobfuscated_key), output_stream(ostr)
{
	// Generate an initialisation vector
	iv_written = false;
	// TODO: generate a cryptographically random IV
	// Create a random engine and distribution for generating random bytes
	std::mt19937 random_engine;
	random_engine.seed(std::time(nullptr));
	std::uniform_int_distribution<std::uint8_t> random_byte(0x00, 0xff);

	// Simply copy the buffer
	for (size_t i = 0; i < block_size; i++)
	{
		iv[i] = random_byte(random_engine);
	}

	// Set buffer pointers
	_Init();
	setp(plaintext, plaintext, plaintext + block_size);
}

aes_encrypt_streambuffer::~aes_encrypt_streambuffer()
{
	// Zero the buffers, so no data remains in memory
	std::memset(plaintext, 0, block_size);
	std::memset(iv,        0, block_size);
}

void aes_encrypt_streambuffer::finalise()
{
	// When overflow is called with eof, it will finalise
	overflow();

	// If output is done, flush the output stream as well
	output_stream.flush();
}

aes_encrypt_streambuffer::int_type aes_encrypt_streambuffer::overflow(int_type new_char)
{
	// Retrieve the number of characters in the buffer
	int in_length = pptr() - pbase();
	// TODO
	/*
	// Set xz input properties
	xz_stream.next_in = reinterpret_cast<std::uint8_t*>(in_buffer);
	xz_stream.avail_in = in_length;

	// If done, finalise the stream, otherwise run with more input
	input_done = input_done || (new_char == traits_type::eof());
	xz_action = input_done ? LZMA_FINISH : LZMA_RUN;

	do
	{
		// Set xz stream properties
		xz_stream.next_out = reinterpret_cast<std::uint8_t*>(out_buffer);
		xz_stream.avail_out = buffer_size;

		// Applies the actual compression
		xz_result = lzma_code(&xz_stream, xz_action);

		if ((xz_result != LZMA_OK) && (xz_result != LZMA_STREAM_END))
		{
			throw xz_error("Could not compress data.", xz_result);
		} 
		else // Write the result to the output stream
		{
			// Get how many bytes of the buffer were filled
			out_length = buffer_size - xz_stream.avail_out;

			// Write the bytes to the output stream
			output_stream.write(out_buffer, out_length);
		}

		output_done = output_done || (xz_result == LZMA_STREAM_END);
	}
	// Repeat while the buffer is full
	while (xz_stream.avail_out == 0 && !output_done);

	// If the passed character was not eof, reset the input buffer
	if (!input_done)
	{
		// Put the character in the buffer
		in_buffer[0] = new_char;
		// Reset the pointer (note that the next pointer is in_buffer + 1 because there already is a character now)
		setp(in_buffer, in_buffer + 1, in_buffer + buffer_size);
	}
	*/
}

aes_encrypt_stream::aes_encrypt_stream(std::basic_ostream<char>& ostr, key_generator& deobfuscated_key)
	: streambuffer(ostr, deobfuscated_key), basic_ostream<char>(&streambuffer)
{
		
}

aes_encrypt_stream::~aes_encrypt_stream()
{

}

aes_encrypt_stream& aes_encrypt_stream::flush()
{
	// Do whatever the base would do on flush
	std::basic_ostream<char>::flush();

	// Finalise encryption
	streambuffer.finalise();

	return *this;
}