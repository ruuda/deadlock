#include "password.h"

#include <ctime>

using namespace deadlock::core::data;

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