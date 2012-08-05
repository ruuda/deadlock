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

#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_XZ_DECOMPRESS_STREAM_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_XZ_DECOMPRESS_STREAM_H_

#include <cstdint>
#include <streambuf>
#include <istream>

#include "xz_compress_stream.h"

namespace deadlock
{
	namespace core
	{
		namespace cryptography
		{
			namespace detail
			{

				/// The XZ streambuffer that decompresses data
				class xz_decompress_streambuffer : public std::basic_streambuf<char>
				{
				protected:

					/// The stream to read the compressed data from
					std::basic_istream<char>& input_stream;

					/// The size of the temporary buffers for decommpression (4 kiB)
					static const size_t buffer_size = 4096;

					/// Buffer for compressed data
					char in_buffer[buffer_size];
	
					/// Buffer for uncompressed data
					char out_buffer[buffer_size];

					/// Number of bytes of the input buffer that are used
					size_t in_length;

					/// Number of bytes of the output buffer that are used
					size_t out_length;

					/// Whether an eof is encountered on the input stream
					bool input_done;

					/// Whether all output has been read
					bool output_done;

					/// The return value for XZ
					lzma_ret xz_result;

					/// What the decompressor should do
					lzma_action xz_action;

					/// The actual decompressor
					lzma_stream xz_stream; 

				public:

					/// Creates a streambuffer that reads compressed data from the given stream
					xz_decompress_streambuffer(std::basic_istream<char>& istr);

					/// Cleans the lzma compressor and zeroes the buffers
					virtual ~xz_decompress_streambuffer();

				protected:

					/// If the input buffer is empty, decompress some more
					virtual int_type underflow();
				};
			}

			/// A stream that reads compressed data from the underlying stream, and provides the decompressed data
			class  xz_decompress_stream : public std::basic_istream<char>
			{
			protected:

				/// The streambuffer responsible for the actual decompression
				detail::xz_decompress_streambuffer streambuffer;

			public:

				/// Creates a decompression reads compressed dat afrom the given stream, and provides decompressed data
				xz_decompress_stream(std::basic_istream<char>& istr);

				~xz_decompress_stream();
			};
		}
	}
}

#endif