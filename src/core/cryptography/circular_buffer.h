#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_CIRCULAR_BUFFER_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_CIRCULAR_BUFFER_H_

#include <cstdint>

namespace deadlock
{
	namespace core
	{
		namespace cryptography
		{
			/// Holds a certain amount of bytes, and provides bytes on request.
			/// When the last byte is read, it starts again at the beginning.
			template <size_t size> class circular_buffer
			{
			protected:

				/// The raw data
				std::uint8_t buffer[size];

				/// Index into the byte array.
				/// Points before the next byte that will be read
				size_t cursor;

			public:

				/// Initialises the buffer by copying the given bytes
				/// The provided array must have enough bytes to fill the buffer
				circular_buffer(std::uint8_t* buffer_to_copy)
				{
					for (size_t i = 0; i < size; i++)
					{
						buffer[i] = buffer_to_copy[i];
					}

					/// Start reading at the first byte
					cursor = 0;
				}

				/// Resets the cursor to the beginning of the buffer
				void reset()
				{
					cursor = 0;
				}

				/// Returns the next byte from the buffer and advances the cursor
				std::uint8_t next()
				{
					std::uint8_t byte = buffer[cursor];
					
					cursor++; if (cursor >= size) cursor = 0;

					return byte;
				}
			};
		}
	}
}