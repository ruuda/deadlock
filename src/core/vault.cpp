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

#include "core.h"

using namespace deadlock::core;

vault::vault()
{

}

void vault::deserialise(const serialisation::json_value::object_t& json_data)
{

}

void vault::serialise(serialisation::serialiser& serialiser, bool obfuscation)
{
	// Root is an object
	serialiser.write_begin_object();
	{
		// Write version
		serialiser.write_object_key("version");
		std::stringstream str_stream; str_stream << assembly_information::get_version();
		serialiser.write_string(str_stream.str());

		// If obfuscated, write the obfuscation buffer as an array of bytes
		if (obfuscation)
		{
			serialiser.write_object_key("obfuscation_buffer");

			// Convert the buffer to a string, using hexadecimal representation
			std::stringstream hex_string;
			
			// Output each character to the stream
			for (size_t i = 0; i < obfuscation_buffer.size(); i++)
			{
				hex_string << std::hex << static_cast<int>(obfuscation_buffer[i]);
			}
			
			serialiser.write_string(hex_string.str());
		}
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

void vault::import_json(const std::string& filename)
{
	// TODO binary mode?
	std::ifstream file(filename);

	if (file.good())
	{
		try
		{
			deserialise(file);
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

void vault::export_json(const std::string& filename, bool obfuscation)
{
	// TODO binary mode?
	std::ofstream file(filename);

	if (file.good())
	{
		// Export as human-readable JSON
		serialise(file, obfuscation, true);
	}
	else
	{
		file.close();
		throw std::runtime_error("Could not open file for writing.");
	}

	file.close();
}