#ifndef _DEADLOCK_CORE_DOCUMENT_H_
#define _DEADLOCK_CORE_DOCUMENT_H_

#include <istream>
#include <ostream>

#include "version.h"
#include "circular_buffer.h"
#include "data/entry_collection.h"

#include "win32_export.h"

namespace deadlock
{
	namespace core
	{
		/// Represents one document of passwords
		/// The document contains the collection of passwords,
		/// properties, and can be written and loaded.
		class _export document
		{
		protected:

			/// A buffer of 512 bits used to obfuscate/de-obfuscate data
			circular_buffer<64> secret;

			/// The collection of entries that the document stores
			data::entry_collection entries;

		public:

			/// Creates an empty document
			document();

			/// Reads the password collection from a stream of JSON
			/// The version specifies the version number of the JSON structure.
			/// (This is the version of Deadlock, can be used to read older files.)
			void read_json(version v, std::istream& json_stream);

			/// Writes the password collection as JSON to a stream
			/// This always writes the most recent version of the Deadlock JSON structure.
			void write_json(std::ostream& json_stream);
		};
	}
}

#endif