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

#include "xz_decompress_stream.h"
#include "../errors.h"

using namespace deadlock::core::cryptography;
using namespace deadlock::core::cryptography::detail;
using deadlock::core::xz_error;

xz_decompress_streambuffer::xz_decompress_streambuffer(std::basic_istream<char>& istr)
	: input_stream(istr)
{
	// Initialise the stream
	lzma_stream xz_stream_init = LZMA_STREAM_INIT;
	xz_stream = xz_stream_init;

	// Decompression flags
	// TODO: is LZMA_CONCATENATED useful here?
	const std::uint32_t flags = LZMA_TELL_UNSUPPORTED_CHECK | LZMA_CONCATENATED;
	// No memory limit
	const std::uint64_t memory_limit = std::numeric_limits<std::uint64_t>::max();

	// Create the compressor
	xz_result = lzma_stream_decoder(&xz_stream, memory_limit, flags);
	if (xz_result != LZMA_OK)
	{
		throw xz_error("Could not initialise decompressor.", xz_result);
	}

	// Set buffer pointers
	_Init();
	// The input buffer is empty to begin with
	setg(out_buffer, out_buffer, out_buffer + 0);

	// Not done yet
	input_done = false;
	output_done = false;
	in_length = buffer_size;
	out_length = 0;
}

xz_decompress_streambuffer::~xz_decompress_streambuffer()
{
	// Clear the compressor
	lzma_end(&xz_stream);

	// Zero the buffers, so no data remains in memory
	std::memset(in_buffer, 0, buffer_size);
	std::memset(out_buffer, 0, buffer_size);
}

xz_decompress_streambuffer::int_type xz_decompress_streambuffer::underflow()
{
	// If everything has been decompressed already, it must be the end of the stream
	if (output_done) return traits_type::eof();

	out_length = 0;

	// Run one iteration of decoding until there is something in the buffer
	while (!output_done && out_length == 0)
	{
		// Attempt to read from the input buffer
		input_stream.read(in_buffer + (buffer_size - in_length), in_length);

		// Retrieve the number of characters in the buffer
		in_length = (buffer_size - in_length) + input_stream.gcount();

		// Set xz input properties
		xz_stream.next_in = reinterpret_cast<std::uint8_t*>(in_buffer);
		xz_stream.avail_in = in_length;

		// If done, finalise the stream (decompress the remaining data), otherwise run with more input
		input_done = input_stream.eof();
		xz_action = input_done ? LZMA_FINISH : LZMA_RUN;

		// Set xz stream properties
		xz_stream.next_out = reinterpret_cast<std::uint8_t*>(out_buffer);
		xz_stream.avail_out = buffer_size;

		// Applies the actual decompression
		xz_result = lzma_code(&xz_stream, xz_action);

		if ((xz_result != LZMA_OK) && (xz_result != LZMA_STREAM_END))
		{
			throw xz_error("Could not decompress data.", xz_result);
		} 
		else // We now have new data
		{
			// Get how many bytes of the buffer were filled
			out_length = buffer_size - xz_stream.avail_out;

			// Set the get pointer
			setg(out_buffer, out_buffer + 0, out_buffer + out_length);

			// Move data that was not consumed to the beginning of the buffer
			in_length = (buffer_size - xz_stream.avail_in);
			for (size_t i = in_length, j = 0; i < buffer_size; i++, j++)
			{
				in_buffer[j] = in_buffer[i];
			}
		}

		output_done = output_done || (xz_result == LZMA_STREAM_END);
	}

	// Return the first character of the new buffer, or eof if the stream ends
	if (out_length > 0) return out_buffer[0];
	return traits_type::eof();
}

xz_decompress_stream::xz_decompress_stream(std::basic_istream<char>& istr)
	: streambuffer(istr), basic_istream<char>(&streambuffer)
{
	
}

xz_decompress_stream::~xz_decompress_stream()
{

}