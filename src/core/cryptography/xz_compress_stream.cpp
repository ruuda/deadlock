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

#include "xz_compress_stream.h"

using namespace deadlock::core::cryptography;

xz_compress_streambuffer::xz_compress_streambuffer(std::basic_ostream<char>& ostr, int compression_level)
	: output_stream(ostr)
{
	// Initialise the stream
	lzma_stream xz_stream_init = LZMA_STREAM_INIT;
	xz_stream = xz_stream_init;

	// Use CRC32 as the check
	lzma_check check = LZMA_CHECK_CRC32;

	// Create the compressor
	xz_result = lzma_easy_encoder(&xz_stream, compression_level, check);
	if (xz_result != LZMA_OK)
	{
		throw xz_error("Could not initialise compressor.", xz_result);
	}

	// Set buffer pointers
	_Init();
	setp(in_buffer, in_buffer, in_buffer + buffer_size);

	// Not done yet
	input_done = false;
	output_done = false;
	in_length = 0;
	out_length = 0;
}

xz_compress_streambuffer::~xz_compress_streambuffer()
{
	// Clear the compressor
	lzma_end(&xz_stream);

	// Zero the buffers, so no data remains in memory
	std::memset(in_buffer, 0, buffer_size);
	std::memset(out_buffer, 0, buffer_size);
}

void xz_compress_streambuffer::finalise()
{
	// When overflow is called with eof, it will finalise
	overflow();

	// If output is done, flush the output stream as well
	output_stream.flush();
}

xz_compress_streambuffer::int_type xz_compress_streambuffer::overflow(int_type new_char)
{
	// Retrieve the number of characters in the buffer
	in_length = pptr() - pbase();

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
}

xz_compress_stream::xz_compress_stream(std::basic_ostream<char>& ostr, int compression_level)
	: streambuffer(ostr, compression_level), basic_ostream<char>(&streambuffer)
{
		
}

xz_compress_stream::~xz_compress_stream()
{

}

xz_compress_stream& xz_compress_stream::flush()
{
	// Do whatever the base would do on flush
	std::basic_ostream<char>::flush();

	// Finalise compression
	streambuffer.finalise();

	return *this;
}