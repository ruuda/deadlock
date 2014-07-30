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
  {

    // Create two initial vaults, one for import, one for export
    vault first, second;

    // Store two entries in the first vault
    data::entry_ptr etr1 = data::make_entry();
    etr1->set_username("Guybrush Threepwood"); // No need to use make_secure_string; security fails already due to hard-coded constants
    etr1->set_id("Fictional Identifier 1");
    etr1->set_password("correct horse battery staple");
    etr1->set_additional_data("nothing");
    first.add_entry(etr1);

    data::entry_ptr etr2 = data::make_entry();
    etr2->set_username("Gordon Freeman");
    etr2->set_id("Fictional Identifier 2");
    etr2->set_password("the cake is a lie");
    first.add_entry(etr2);

    // A stream to hold the compressed data
    std::stringstream compressed_data_stream;
  
    // A stream that compresses data
    cryptography::xz_compress_stream compression_stream(compressed_data_stream, 6);

    // Export and compress
    // stringstream << compression_stream << vault
    first.export_json(compression_stream, true);
    compression_stream.close();

    // Seek to the beginning of the compressed data
    compressed_data_stream.seekg(0, std::ios_base::beg);

    // A stream that decompresses data
    cryptography::xz_decompress_stream decompression_stream(compressed_data_stream);

    // Decompress and import
    // stringstream >> compression_stream >> vault
    second.import_json(decompression_stream);

  
    vault::const_entry_iterator it = second.begin();

    // Validate the first entry
    if (it->get_username() != etr1->get_username()) throw std::runtime_error("Username not retrieved correctly.");
    if (it->get_password().get_password() != etr1->get_password().get_password()) throw std::runtime_error("Password not retrieved correctly.");
    if (it->get_password().get_stored_time() != etr1->get_password().get_stored_time()) throw std::runtime_error("Password timestamp not retrieved correctly.");
    if (it->get_id() != etr1->get_id()) throw std::runtime_error("Identifier not retrieved correctly.");
    if (it->get_additional_data() != etr1->get_additional_data()) throw std::runtime_error("Additional data not retrieved correctly.");

    it++;

    // Validate the second entry
    if (it->get_username() != etr2->get_username()) throw std::runtime_error("Username not retrieved correctly.");
    if (it->get_password().get_password() != etr2->get_password().get_password()) throw std::runtime_error("Password not retrieved correctly.");
    if (it->get_password().get_stored_time() != etr2->get_password().get_stored_time()) throw std::runtime_error("Password timestamp not retrieved correctly.");
    if (it->get_id() != etr2->get_id()) throw std::runtime_error("Identifier not retrieved correctly.");
    if (it->get_additional_data() != etr2->get_additional_data()) throw std::runtime_error("Additional data not retrieved correctly.");

    it++;
    if (it != second.end()) throw std::runtime_error("Incorrect number of entries encountered.");

  }

  {
    // Test little data

    // A stream to hold the compressed data
    std::stringstream compressed_data_stream;
  
    // A stream that compresses data
    cryptography::xz_compress_stream compression_stream(compressed_data_stream, 6);

    // Write something and flush
    compression_stream << "hullo, world";
    compression_stream.close();

    // Seek to the beginning of the compressed data
    compressed_data_stream.seekg(0, std::ios_base::beg);

    // A stream that decompresses data
    cryptography::xz_decompress_stream decompression_stream(compressed_data_stream);

    // Decompress
    std::string result; std::getline(decompression_stream, result);
    if (result != "hullo, world") throw std::runtime_error("Compression or decompression failed for little data.");
  }
}
