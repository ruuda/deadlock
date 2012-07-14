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

#include "vault.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "core.h"
#include "errors.h"
#include "endianness.h"
#include "cryptography/key_generator.h"
#include "cryptography/aes_cbc_decrypt_stream.h"
#include "cryptography/aes_cbc_encrypt_stream.h"
#include "cryptography/xz_compress_stream.h"
#include "cryptography/xz_decompress_stream.h"

using namespace deadlock::core;

vault::vault()
{

}

void vault::add_entry(const data::entry& tr)
{
	entries.push_back(tr);
}

void vault::deserialise(const serialisation::json_value::object_t& json_data)
{
	// At least, the data must contain version information and entries
	if (json_data.find("version") == json_data.end())
	{
		throw format_error("No version information present.");
	}
	if (json_data.find("entries") == json_data.end())
	{
		throw format_error("No entries present.");
	}

	// Read the version
	data::secure_stringstream_ptr version_string = data::make_secure_stringstream(static_cast<const data::secure_string&>(json_data.at("version")));
	version file_version;
	*version_string >> file_version;
	version application_version = assembly_information::get_version();

	// Version checks could be added here to parse old versions
	// For now, there is only one version, so it does not matter
	// Forward compatibility is not assumed, reading a newer version is an error.
	if (application_version < file_version)
	{
		throw version_error("The file was created with a newer version of the application.");
	}

	// Deserialise the entries
	entries.deserialise(json_data.at("entries"));
}

void vault::serialise(serialisation::serialiser& serialiser, bool obfuscation)
{
	// Root is an object
	serialiser.write_begin_object();
	{
		// Write version
		serialiser.write_object_key("version");
		data::secure_stringstream_ptr str_stream = data::make_secure_stringstream();
		(*str_stream) << assembly_information::get_version();
		serialiser.write_string(str_stream->str());

		// Write the entries
		serialiser.write_object_key("entries");
		entries.serialise(serialiser, obfuscation);
	}
	serialiser.write_end_object();
}

void vault::deserialise(std::istream& json_stream)
{
	serialisation::json_value vault_root;

	// Deserialise JSON from stream
	json_stream >> vault_root;

	// Read the JSON structure
	deserialise(vault_root);
}

void vault::serialise(std::ostream& json_stream, bool obfuscation, bool human_readable)
{
	// Construct a serialiser that writes to the stream
	serialisation::serialiser serialiser(json_stream, human_readable);

	// Serialise to the stream
	serialise(serialiser, obfuscation);
}

void vault::import_json(std::istream& input_stream)
{
	// Simply deserialise from the stream
	deserialise(input_stream);
}


void vault::import_json(const std::string& filename)
{
	// TODO binary mode?
	std::ifstream file(filename);

	if (file.good())
	{
		try
		{
			import_json(file);
		}
		catch (...)
		{
			file.close();
			throw;
		}
		file.close();
	}
	else
	{
		file.close();
		throw std::runtime_error("Could not open file.");
	}
}

void vault::export_json(std::ostream& output_stream, bool obfuscation)
{
	// Export as human-readable JSON
	serialise(output_stream, obfuscation, true);
}

void vault::export_json(const std::string& filename, bool obfuscation)
{
	// TODO binary mode?
	std::ofstream file(filename);

	if (file.good())
	{
		// Export to the file stream
		export_json(file, obfuscation);
	}
	else
	{
		file.close();
		throw std::runtime_error("Could not open file for writing.");
	}

	file.close();
}

void vault::load(const std::string& filename, cryptography::key_generator& key, const std::string& passphrase)
{
	// Open the file
	std::ifstream file(filename, std::ios::binary);

	if (file.good())
	{
		try
		{
			// Validate the header
			char d, l, k, zero;
			file >> d; file >> l; file >> k; file >> zero;
			if (d != 'D' || l != 'L' || k != 'K' || zero != 0)
			{
				throw format_error("The file is not a valid Deadlock vault; the header is incorrect.");
			}

			// Now read the version
			version file_version;
			version application_version = assembly_information::get_version();
			file_version.major = file.get(); file_version.minor = file.get();
			file_version.revision = file.get(); file_version.build = file.get();

			// Version checks could be added here to parse old versions
			// For now, there is only one version, so it does not matter
			// Forward compatibility is not assumed, reading a newer version is an error.
			if (application_version < file_version)
			{
				throw version_error("The file was created with a newer version of the application.");
			}

			// Read the number of PBKDF2 iterations (stored as a big-endian 32-bit integer)
			std::uint32_t iterations;
			file.read(reinterpret_cast<char*>(&iterations), 4);
			iterations = portable_to_internal(iterations);

			// Followed by the 32 bytes of salt that were used to generate the key
			for (size_t i = 0; i < key.salt_size; i++)
			{
				key.get_salt()[i] = file.get();
			}

			// Now generate the key
			key.generate_key(passphrase, iterations);

			// Create a decryption stream that reads encrypted data
			cryptography::aes_cbc_decrypt_stream decrypt_stream(file, key);
			// And a decompression stream that decompresses data
			cryptography::xz_decompress_stream decompress_stream(decrypt_stream);

			// Read the obfuscated JSON as follows: file >> AES CBC decrypt >> XZ decompress >> JSON >> deserialise
			deserialise(decompress_stream);
		}
		catch (...)
		{
			file.close();
			throw;
		}
		file.close();
	}
	else
	{
		file.close();
		throw std::runtime_error("Could not open file.");
	}
}

void vault::save(const std::string& filename, cryptography::key_generator& key)
{
	// Open the file
	std::ofstream file(filename, std::ios::binary);

	if (file.good())
	{
		// First, write the header structure
		// In this case, it is "DLK\0", followed by four bytes for the version
		// The version is written to allow future extensions / reading legacy formats
		file.put('D'); file.put('L'); file.put('K'); file.put(0);
		version file_version = assembly_information::get_version();
		// Write the version bytes independently to avoid endianness issues
		file.put(file_version.major); file.put(file_version.minor);
		file.put(file_version.revision); file.put(file_version.build);

		// Now for the current version, write the number of PBKDF2 iterations (as a big-endian 32-bit integer)
		std::uint32_t iterations = key.get_iterations();
		iterations = internal_to_portable(iterations);
		file.write(reinterpret_cast<char*>(&iterations), 4);

		// Followed by the 32 bytes of salt that were used to generate the key
		for (size_t i = 0; i < key.salt_size; i++)
		{
			file.put(key.get_salt()[i]);
		}

		// Create an encryption stream that saves data encrypted
		cryptography::aes_cbc_encrypt_stream encrypt_stream(file, key);
		// And a compression stream that compresses data
		cryptography::xz_compress_stream compress_stream(encrypt_stream, 6);

		// Write the obfuscated JSON as follows: JSON >> XZ compress >> AES CBC encrypt >> file
		serialise(compress_stream, true, false);
		compress_stream.flush(); // Finalises compression, adds padding for encryption, and flushes
		encrypt_stream.flush();
		file.flush();
	}
	else
	{
		file.close();
		throw std::runtime_error("Could not open file for writing.");
	}

	file.close();
}
