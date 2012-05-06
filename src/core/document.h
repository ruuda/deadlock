#ifndef _DEADLOCK_CORE_DOCUMENT_H_
#define _DEADLOCK_CORE_DOCUMENT_H_

#include <istream>
#include <ostream>

#include "version.h"
#include "circular_buffer.h"
#include "data/entry_collection.h"
#include "serialisation/deserialiser.h"
#include "serialisation/serialiser.h"

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
			circular_buffer<64> obfuscation_buffer;

			/// The collection of entries that the document stores
			data::entry_collection entries;

			/// Reconstructs the document given the JSON data
			void deserialise(const serialisation::value& json_data);

			/// Writes the document to the serialiser
			/// If no_obfuscation is true, it will de-obfuscate the strings before writing them,
			/// and it will not write the obfuscation buffer.
			void serialise(serialisation::serialiser& serialiser, bool no_obfuscation);

			/// Reads the password collection from a stream of JSON
			void import_json(std::istream& json_stream);

			/// This exports the most recent version of the Deadlock JSON structure.
			/// If no_obfuscation is true, it will de-obfuscate the strings before writing them,
			/// and it will not write the obfuscation buffer.
			void export_json(std::ostream& json_stream, bool no_obfuscation);

		public:

			/// Creates an empty document
			document();

			/// Reads the password collection from a file
			void import_json(const std::string& filename);

			/// Writes the password collection as JSON to a file
			/// This always writes the most recent version of the Deadlock JSON structure.
			/// If no_obfuscation is true, it will de-obfuscate the strings before writing them,
			/// and it will not write the obfuscation buffer.
			void export_json(const std::string& filename, bool no_obfuscation);
		};
	}
}

#endif