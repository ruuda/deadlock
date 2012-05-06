#include "obfuscated_string.h"

using namespace deadlock::core::data;

// <see password.cpp for implementation of the empty obfuscated string>

obfuscated_string::obfuscated_string(const std::string& string_of_obfuscated_bytes) :
	obfuscated_data(string_of_obfuscated_bytes)
{

}

const obfuscated_string& obfuscated_string::empty()
{
	return empty_obfuscated_string;
}