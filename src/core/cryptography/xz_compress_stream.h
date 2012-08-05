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

#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_XZ_COMPRESS_STREAM_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_XZ_COMPRESS_STREAM_H_

#include <cstdint>
#include <stdexcept>
#include <streambuf>
#include <ostream>

extern "C"
{
	#include <lzma.h>
}

namespace deadlock
{
	namespace core
	{
		namespace cryptography
		{
			namespace detail
			{
				/// The XZ streamuffer that compresses data
				class xz_compress_streambuffer : public std::basic_streambuf<char>
				{
				protected:

					/// The stream that all compressed data will be streamed to
					std::basic_ostream<char>& output_stream;

					/// The size of the temporary buffers for commpression (4 kiB)
					static const size_t buffer_size = 4096;

					/// Buffer for data that is received (data to be compressed)
					char in_buffer[buffer_size];
	
					/// Buffer for compressed data
					char out_buffer[buffer_size];

					/// Number of bytes of the input buffer that are used
					size_t in_length;

					/// Number of bytes of the output buffer that are used
					size_t out_length;

					/// Whether input is finished
					bool input_done;

					/// Whether output is finished
					bool output_done;

					/// The return value for XZ
					lzma_ret xz_result;

					/// What the compressor should do
					lzma_action xz_action;

					/// The actual compressor
					lzma_stream xz_stream; 

				public:

					/// Creates a streambuffer that streams its output to the given stream,
					/// and compresses with the given compression level (ranging from 0 to 9, see XZ documentation)
					xz_compress_streambuffer(std::basic_ostream<char>& ostr, int compression_level);

					/// Cleans the lzma compressor and zeroes the buffers
					virtual ~xz_compress_streambuffer();

					/// Tells the XZ compressor to stop accepting new data, and writes the remaining data to the underlying stream
					virtual void finalise();

				protected:

					/// Once the buffer is full, compresses the data with XZ and writes it to the output stream
					virtual int_type overflow(int_type new_char = traits_type::eof());
				};
			}

			/// A stream that compresses the data written to it, and writes that data to the underlying stream
			class xz_compress_stream : public std::basic_ostream<char>
			{
			protected:

				/// The streambuffer responsible for the actual compression
				detail::xz_compress_streambuffer streambuffer;

			public:

				/// Creates a compression stream that streams its output to the given stream,
				/// and compresses with the given compression level (ranging from 0 to 9, see xz documentation)
				xz_compress_stream(std::basic_ostream<char>& ostr, int compression_level);

				~xz_compress_stream();

				/// Marks the stream as done, and flushes the remaining data
				/// This should only be called once.
				void close();
			};
		}
	}
}

#endif