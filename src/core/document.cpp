#include "document.h"

#include <fstream>

#include "core.h"

using namespace deadlock::core;

document::document()
{

}

void document::deserialise(const serialisation::json_value::object_t& json_data)
{

}

void document::serialise(serialisation::serialiser& serialiser, bool no_obfuscation)
{

}

void document::deserialise(std::istream& json_stream)
{
	serialisation::json_value document_root;

	// Deserialise JSON from stream
	json_stream >> document_root;

	// Read the JSON structure
	deserialise(document_root);
}

void document::serialise(std::ostream& json_stream, bool no_obfuscation, bool human_readable)
{
	// Construct a serialiser that writes to the stream
	serialisation::serialiser serialiser(json_stream, human_readable);

	// Serialise to the stream
	serialise(serialiser, no_obfuscation);
}

void document::import_json(const std::string& filename)
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

void document::export_json(const std::string& filename, bool no_obfuscation)
{
	// TODO binary mode?
	std::ofstream file(filename);

	if (file.good())
	{
		// Export as human-readable JSON
		serialise(file, no_obfuscation, true);
	}
	else
	{
		file.close();
		throw std::runtime_error("Could not open file for writing.");
	}

	file.close();
}