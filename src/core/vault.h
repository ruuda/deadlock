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

#ifndef _DEADLOCK_CORE_VAULT_H_
#define _DEADLOCK_CORE_VAULT_H_

#include <istream>
#include <ostream>

#include <boost/iterator/indirect_iterator.hpp>

#include "version.h"
#include "circular_buffer.h"
#include "data/entry_collection.h"
#include "serialisation/deserialiser.h"
#include "serialisation/serialiser.h"
#include "cryptography/key.h"
#include "cryptography/aes_cbc_decrypt_stream.h"
#include "cryptography/xz_decompress_stream.h"

namespace deadlock
{
  namespace core
  {
    /// Represents one 'vault' of passwords
    /// The vault contains the collection of passwords,
    /// properties, and can be written and loaded.
    class vault
    {
    protected:

      /// The version of the loaded file (if the vault was loaded from a file)
      version file_version;

      /// The collection of entries that the vault stores
      data::entry_collection entries;

      /// Reconstructs the vault given the JSON data
      void deserialise(const serialisation::json_value::object_t& json_data);

      /// Writes the vault to the serialiser
      /// If obfuscation is false, it will write data as-is.
      /// Otherwise, it will write the data as hexadecimal strings.
      void serialise(serialisation::serialiser& serialiser, bool obfuscation);

      /// Reads the password collection from a stream of JSON
      void deserialise(std::istream& json_stream);

      /// This exports the most recent version of the Deadlock JSON structure.
      /// If obfuscation is false, it will write data as-is.
      /// Otherwise, it will write the data as hexadecimal strings.
      void serialise(std::ostream& json_stream, bool obfuscation, bool human_readable);

    public:

      typedef boost::indirect_iterator<data::entry_collection::entry_iterator> entry_iterator;
      typedef boost::indirect_iterator<data::entry_collection::const_entry_iterator> const_entry_iterator;

      /// Creates an empty vault
      vault();

      /// Returns the file version
      inline const version& get_version() const { return file_version; }

      /// Adds a new entry to the collection
      void add_entry(data::entry_ptr new_entry);

      /// Returns an iterator to the first entry
      inline entry_iterator begin() { return entries.begin(); }

      /// Returns an iterator past the last entry
      inline entry_iterator end() { return entries.end(); }

      /// Returns an iterator to the first entry
      inline const_entry_iterator begin() const { return entries.begin(); }

      /// Returns an iterator past the last entry
      inline const_entry_iterator end() const { return entries.end(); }

      /// Reads the password collection from a file
      void import_json(const std::string& filename);

      /// Reads the password collection from a stream
      void import_json(std::istream& input_stream);

      /// Writes the password collection as JSON to a file
      /// This always writes the most recent version of the Deadlock JSON structure.
      /// If obfuscation is false, it will write data as-is.
      /// Otherwise, it will write the data as hexadecimal strings.
      void export_json(const std::string& filename, bool obfuscation);

      /// Writes the password collection as JSON to a stream
      /// This always writes the most recent version of the Deadlock JSON structure.
      /// If obfuscation is false, it will write data as-is.
      /// Otherwise, it will write the data as hexadecimal strings.
      void export_json(std::ostream& output_stream, bool obfuscation);

      /// Saves the vault encrypted to a binary file
      void save(const std::string& filename, const cryptography::key& key);

      /// Saves the vault encrypted to a binary stream
      void save(std::ostream& output_stream, const cryptography::key& key);

      /// Loads an encrypted binary vault from a file.
      /// This also generates the correct key.
      void load(const std::string& filename, cryptography::key& key, const data::secure_string& passphrase);

      /// Loads an encrypted binary vault from a stream.
      /// This also generates the correct key.
      void load(std::istream& input_stream, cryptography::key& key, const data::secure_string& passphrase);

      /// Builds a stream that reads a Deadlock vault from input_stream,
      /// and allows the plaintext data to be read from the resulting decompression stream.
      /// This will put the correct key in key, and version of the vault in vault_version.
      /// decrypt_stream will contain the decryption stream, which should be deleted after use, but not used directly.
      /// decompress_stream will contain the decompression stream from which the plaintext can be read.
      /// decompress_stream should also be deleted after use.
      static void build_decrypt_stream(std::istream& input_stream, version& vault_version, cryptography::key& key,
        cryptography::aes_cbc_decrypt_stream*& decrypt_stream,
        cryptography::xz_decompress_stream*& decompress_stream, const data::secure_string& passphrase);
    };
  }
}

#endif
