#include "password.h"

#include <ctime>

using namespace deadlock::core::data;

// Initialise empty obfuscated string here to avoid static initialisation fiasco
obfuscated_string obfuscated_string::empty_obfuscated_string = obfuscated_string("");
password password::empty_password = password(obfuscated_string::empty(), 0x0);

password::password(obfuscated_string password_data, std::int64_t stored_time) :
	obfuscated_password(password_data), store_time(stored_time)
{

}

password::password(obfuscated_string password_data) :
	obfuscated_password(password_data)
{
	// Store with the current time
	store_time = std::time(nullptr);
}

const password& password::empty()
{
	return empty_password;
}