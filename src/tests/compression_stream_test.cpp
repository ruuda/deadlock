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

#include "compression_stream_test.h"
#include "../core/core.h"
#include "../core/cryptography/xz_compress_stream.h"
#include "../core/cryptography/xz_decompress_stream.h"

#include <stdexcept>
#include <sstream>

using namespace deadlock::core;
using namespace deadlock::tests;

std::string compression_stream_test::get_name()
{
	return "compression_stream";
}

void compression_stream_test::run()
{
	// Create two initial vaults, one for import, one for export
	vault first, second;

	// Store two entries in the first vault
	data::entry etr1;
	etr1.set_username("Guybrush Threepwood");
	etr1.set_key("Fictional Key 1");
	etr1.set_password(data::obfuscated_string(first.obfuscate_string("correct horse battery staple")));
	etr1.set_additional_data("nothing");
	first.add_entry(etr1);

	data::entry etr2;
	etr2.set_username("Gordon Freeman");
	etr2.set_key("Fictional Key 2");
	etr2.set_password(data::obfuscated_string(first.obfuscate_string("the cake is a lie")));
	first.add_entry(etr2);

	// A stream to hold the compressed data
	std::stringstream compressed_data_stream;
	
	// A stream that compresses data
	cryptography::xz_compress_stream compression_stream(compressed_data_stream, 6);

	// Export and compress
	// stringstream << compression_stream << vault
	first.export_json(compression_stream, true);
	compression_stream.flush();

	// Seek to the beginning of the compressed data
	compressed_data_stream.seekg(0, std::ios_base::beg);

	// A stream that decompresses data
	cryptography::xz_decompress_stream decompression_stream(compressed_data_stream);

	// Decompress and import
	// stringstream >> compression_stream >> vault
	second.import_json(decompression_stream);

	// TODO: validate the data in the second vault
}
