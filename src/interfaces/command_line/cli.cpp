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

#include "cli.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <boost/chrono.hpp>
#include <list>

#include "../../core/errors.h"
#include "../../core/data/secure_string.h"
#include "../../core/search.h"

namespace po = boost::program_options;
using namespace deadlock::interfaces::command_line;
using namespace deadlock::core;
using namespace deadlock::core::data;

int cli::run(int argc, char** argv)
{
	// Declare the supported options
	po::options_description desc("Supported commands", 80, 40);
	desc.add_options()
		("help,h", "produce help message")
		("version", "show Deadlock version")

		("new,n", "create a new vault")
		("key-iterations", po::value<std::uint32_t>(), "the number of iterations for the key-generation algorithm") // TODO
		("key-time", po::value<double>(), "infer the number of iterations from a time in seconds")

		("identify", "show information about the archive")

		("add,a", po::value<std::string>(), "add a new entry to the vault, with the specified identifier")

		("show,g", po::value<std::string>(), "show the entry with the specified identifier")
		("historical", "show all passwords, including old ones")

		("set,s", po::value<std::string>(), "set a field for the specified entry")
		("id", po::value<std::string>(), "set the identifier for an entry")
		("username", po::value<std::string>(), "set the username for an entry")
		("password", po::value<std::string>(), "set a new password for an entry")
		("additional-data", po::value<std::string>(), "set additional data for an entry")

		("list,l", po::value<std::string>(), "list the identifiers of all stored entries, " \
																 "or all the entries that match the search criteria")

		("export", po::value<std::string>(), "export the archive to JSON (removes encryption)")
		("plain", "save data as plain text instead of hexadecimal representation")

		("import", po::value<std::string>(), "import the archive from JSON (appends to an archive)")

		("vault", po::value<std::string>()->implicit_value(""), "the vault to operate on");
	;

	// The file parameter at the end
	po::positional_options_description p;
	p.add("vault", 1);

	// Parse program options
	po::variables_map vm;
	try
	{
		po::store(
			po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);

		// Fix argument precedence
		if (!vm.count("vault") && vm.count("list"))
		{
			vm.insert(std::make_pair("vault", vm.at("list")));
			vm.at("list").as<std::string>() = std::string();
		}

		po::notify(vm);
	}
	catch (std::exception& ex)
	{
		std::cout << "Failed to parse command-line; " << ex.what() << "." << std::endl;
		return EXIT_FAILURE;
	}

	// Show the details of one entry
	if (vm.count("show"))
	{
		return handle_show(vm);
	}

	// Set fields for one entry
	if (vm.count("set"))
	{
		return handle_set(vm);
	}

	// Add entry to archive
	if (vm.count("add"))
	{
		return handle_add(vm);
	}

	// List all stored entries
	if (vm.count("list"))
	{
		return handle_list(vm);
	}	

	// Create a new archive
	else if (vm.count("new"))
	{
		return handle_new(vm);
	}

	// Print help message
	if (vm.count("help"))
	{
		std::cout << "Deadlock password manager" << std::endl;
		std::cout << "Licensed under the GNU Public Licence" << std::endl;
		std::cout << std::endl;
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}

	// Give information about an archive
	if (vm.count("identify"))
	{
		return handle_identify(vm);
	}

	// Print version
	if (vm.count("version"))
	{
		std::cout << "Deadlock command-line interface (deadlock_cli)" << std::endl;
		std::cout << "Using libdeadlock " << deadlock::core::assembly_information::get_version() << std::endl;
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}

#if defined (_WIN32)

/// Disables echo to allow password input
void set_echo(bool do_echo)
{
	DWORD  mode;
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hConsole, &mode);
	if (do_echo)
	{
		mode = mode | ENABLE_ECHO_INPUT;
	}
	else
	{
		mode = mode & ~ENABLE_ECHO_INPUT;
	}
	SetConsoleMode(hConsole, mode);
}

#else

void set_echo(bool do_echo)
{

}

#endif

secure_string_ptr cli::ask_passphrase() const
{
	secure_string_ptr passphrase = make_secure_string();

	do
	{
		// Ask for a passphrase
		// TODO: use the secure variant that does not write to the console
		std::cout << "Passphrase: ";
		set_echo(false);
		std::getline(std::cin, *passphrase);
		set_echo(true);
		std::cout << std::endl; // Print a newline, because the enter from getline is not echoed.

		if (passphrase->empty())
		{
			std::cout << "Empty passphrases are not allowed." << std::endl;
		}
	}
	while (passphrase->empty());

	return passphrase;
}

bool cli::require_vault_filename(const po::variables_map& vm)
{
	if (!vm.count("vault") || vm.at("vault").as<std::string>().empty())
	{
		std::cerr << "No vault specified." << std::endl;
		return false;
	}

	vault_filename = vm.at("vault").as<std::string>();

	// Make sure the vault exists
	std::ifstream vault_file;
	vault_file.open(vault_filename, std::ios_base::in);
	if (!vault_file.good())
	{
		std::cerr << "The file does not exist." << std::endl;
		return false;
	}
	vault_file.close();

	return true;
}

bool cli::load_vault(const boost::program_options::variables_map& vm)
{
	if (!require_vault_filename(vm))
	{
		return false;
	}

	// Ask the user for his passphrase
	data::secure_string_ptr passphrase = ask_passphrase();	

	// Try to load the vault
	try
	{
		vault.load(vault_filename, key, *passphrase);
	}
	// Check for incorrect key
	catch (incorrect_key_error&)
	{
		std::cerr << "The passphrase is incorrect." << std::endl;
		return false;
	}
	// If anything other goes wrong, report error.
	catch (std::runtime_error& ex)
	{
		std::cerr << "Could not open vault." << std::endl;
		std::cerr << ex.what() << std::endl;
		return false;
	}
	
	return true;	
}

bool cli::set_fields(const po::variables_map& vm, data::entry_ptr entr)
{
	bool anything_set = false;

	// Handle setting identifier
	if (vm.count("id"))
	{
		// Retrieve the identifier from the command line and store it in a secure string.
		data::secure_string_ptr id = data::make_secure_string(vm.at("id").as<std::string>());
		// Set the new identifier.
		entr->set_id(*id);

		anything_set |= true;
	}

	// Handle setting username
	if (vm.count("username"))
	{
		// Retrieve the username from the command line and store it in a secure string.
		data::secure_string_ptr username = data::make_secure_string(vm.at("username").as<std::string>());
		// Set the new username.
		entr->set_username(*username);

		anything_set |= true;
	}

	// Handle adding a new password
	if (vm.count("password"))
	{
		// Retrieve the password from the command line and store it in a secure string.
		// Please note that this is severely insecure anyway, because the terminal probably caches the password.
		data::secure_string_ptr password = data::make_secure_string(vm.at("password").as<std::string>());
		// Set the new password.
		entr->set_password(*password);

		anything_set |= true;
	}

	// Handle setting additional data
	if (vm.count("additional-data"))
	{
		// Retrieve the additional data from the command line and store it in a secure string.
		data::secure_string_ptr additional_data = data::make_secure_string(vm.at("additional-data").as<std::string>());
		// Set the new additional_data.
		entr->set_additional_data(*additional_data);

		anything_set |= true;
	}

	return anything_set;
}

int cli::handle_new(const po::variables_map& vm)
{
	if (!require_vault_filename(vm))
	{
		return EXIT_FAILURE;
	}

	data::secure_string_ptr passphrase = ask_passphrase();	

	// Use 100 000 iterations by default
	std::uint32_t key_iterations = 0;

	// Check whether the user specified anything about key size
	if (vm.count("key-iterations"))
	{
		key_iterations = vm.at("key-iterations").as<std::uint32_t>();
	}
	else if (vm.count("key-time") || key_iterations == 0)
	{
		// Use a default access duration of 1.0 seconds (if nothing is specified)
		double duration = 1.0;
		if (vm.count("key-time"))
		{
			duration = vm.at("key-time").as<double>();
		}
		
		// Determine key iterations based on time
		std::cout << "Measuring suitable number of iterations ...";
		key_iterations = key.get_required_iterations(passphrase->length(), duration);
		std::cout << "\b\b\b\b, done." << std::endl;
	}

	// Use at least one iteration
	key_iterations = std::max<std::uint32_t>(1, key_iterations);
	// Limit the maximum number of iterations, for LibTomCrypt cannot handle more
	key_iterations = std::min<std::uint32_t>(std::numeric_limits<std::int32_t>::max(), key_iterations);

	// Use a random salt for the key
	key.set_salt_random();

	std::cout << "Deriving key using " << key_iterations << " PBKDF2 iterations ...";
	auto start_time = boost::chrono::high_resolution_clock::now();
	key.generate_key(*passphrase, key_iterations);
	auto end_time = boost::chrono::high_resolution_clock::now();
	double duration = static_cast<double>((end_time - start_time).count()) / 1.0e9;
	std::cout << "\b\b\b\b, done in " << std::setprecision(2) << std::fixed << duration << " seconds." << std::endl;

	std::cout << "Encrypting and writing empty vault ...";
	try
	{
		vault.save(vault_filename, key);
	}
	catch (const std::runtime_error& ex)
	{
		std::cout << std::endl;
		std::cerr << "Failed to write vault." << std::endl;
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "\b\b\b\b, done." << std::endl;

	return EXIT_SUCCESS;
}


int cli::handle_identify(const po::variables_map& vm)
{
	if (!require_vault_filename(vm))
	{
		return EXIT_FAILURE;
	}

	// Try to load the file, and see what happens
	try
	{
		vault.load(vault_filename, key, *data::make_secure_string("no_passphrase"));
	}
	// If a format exception is thrown, the file is not valid
	catch (deadlock::core::format_error& format_ex)
	{
		std::cout << format_ex.what() << std::endl;
		return EXIT_SUCCESS;
	}
	catch (deadlock::core::version_error&)
	{
		std::cout << "Deadlock " << vault.get_version() << " vault." << std::endl;
		std::cout << "No more information available due to a version mismatch." << std::endl;
		return EXIT_SUCCESS;
	}
	catch (incorrect_key_error&)
	{
		std::cout << "Deadlock " << vault.get_version() << " vault." << std::endl;
		std::cout << "PBKDF2 iterations: " << key.get_iterations() << "." << std::endl;
		return EXIT_SUCCESS;
	}
	// If anything other goes wrong, report error.
	catch (std::runtime_error& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "Deadlock " << vault.get_version() << " vault." << std::endl;
	std::cout << "PBKDF2 iterations: " << key.get_iterations() << "." << std::endl;
	// If there is no error, the passphrase was "no_passphrase"
	std::cout << "You should use a stronger passphrase." << std::endl;
	return EXIT_SUCCESS;
}

int cli::handle_add(const po::variables_map& vm)
{
	// Make sure the user specified a vault to use
	if (!require_vault_filename(vm))
	{
		return EXIT_FAILURE;
	}

	// Open the vault
	if(!load_vault(vm))
	{
		return EXIT_FAILURE;
	}

	// Retrieve the identifier from the command line and store it in a secure string.
	// The secure string is simply easier to use in combination with the rest of the application;
	// it adds no value since the data is insecure anyway.
	data::secure_string_ptr id = data::make_secure_string(vm.at("add").as<std::string>());

	// Make sure the key is not present already
	if (std::find_if(vault.begin(), vault.end(), [&id](const entry& e)
		{ return string_equals(e.get_id(), *id); }
	) != vault.end())
	{
		std::cerr << "The entry '" << *id << "' exists already." << std::endl;
		return EXIT_FAILURE;
	}

	// Create the new entry
	entry_ptr new_entry = data::make_entry();
	new_entry->set_id(*id);

	// Set additional fiels on the entry, if specified
	set_fields(vm, new_entry);

	// Add the entry to the vault
	vault.add_entry(new_entry);

	// Write the vault with the new contents
	std::cout << "'" << *id << "' added, encrypting and writing vault ...";
	try
	{
		vault.save(vault_filename, key);
	}
	catch (const std::runtime_error& ex)
	{
		std::cout << std::endl;
		std::cerr << "Failed to write vault." << std::endl;
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "\b\b\b\b, done." << std::endl;

	return EXIT_SUCCESS;
}

int cli::handle_list(const po::variables_map& vm)
{
	// Make sure the user specified a vault to use
	if (!require_vault_filename(vm))
	{
		return EXIT_FAILURE;
	}

	// Open the vault
	if(!load_vault(vm))
	{
		return EXIT_FAILURE;
	}

	// If no argument was given, print all stored entries
	if (vm.at("list").as<std::string>().empty())
	{	
		// Print out the identifiers of every entry, one per line
		for (auto i = vault.begin(); i != vault.end(); i++)
		{
			std::cout << i->get_id() << std::endl;
		}
	}
	else // Otherwise, print only the entries that match the search query
	{
		// Retrieve the identifier from the command line and store it in a secure string.
		// The secure string is simply easier to use in combination with the rest of the application;
		// it adds no value since the data is insecure anyway.
		data::secure_string_ptr query = data::make_secure_string(vm.at("list").as<std::string>());

		// Create a search algorithm
		deadlock::core::search search;

		// Search results
		std::list<data::entry_ptr> results;

		// Now execute the search
		search.find_matches(*query, vault.begin(), vault.end(), std::back_inserter(results));

		// And print the identifiers of the matches, one per line
		for (auto i = results.begin(); i != results.end(); i++)
		{
			std::cout << (*i)->get_id() << std::endl;
		}
	}

	return EXIT_SUCCESS;
}

int cli::handle_show(const po::variables_map& vm)
{
	// Make sure the user specified a vault to use
	if (!require_vault_filename(vm))
	{
		return EXIT_FAILURE;
	}

	// Open the vault
	if(!load_vault(vm))
	{
		return EXIT_FAILURE;
	}

	// Retrieve the identifier from the command line and store it in a secure string.
	// The secure string is simply easier to use in combination with the rest of the application;
	// it adds no value since the data is insecure anyway.
	data::secure_string_ptr query = data::make_secure_string(vm.at("show").as<std::string>());

	// Create a search algorithm
	deadlock::core::search search;

	// Now execute the search
	data::entry_ptr result = search.find_match(*query, vault.begin(), vault.end());

	if (result != nullptr)
	{

		// Write the identifier first
		std::cout << result->get_id() << std::endl;
		// Followed by the username if it is set
		if (!result->get_username().empty())
		{
			std::cout << "Username: " << result->get_username() << std::endl;
		}

		// Then the most recent password(s)
		if (vm.count("historical"))
		{
			std::cout << "Passwords:" << std::endl;
			// Print all passwords including date
			for (auto i = result->passwords_begin(); i != result->passwords_end(); i++)
			{
				// Indent the list by one space
				std::cout << " ";

				// Get the time that the password was stored
				time_t timestamp = i->get_stored_time();
				std::tm* store_time = std::localtime(&timestamp);

				// Print in a (hopefully non-ambiguous) format
				std::cout << std::setw(4) << std::setfill('0') << std::fixed << (store_time->tm_year + 1900) << "-";
				std::cout << std::setw(2) << std::setfill('0') << std::fixed << (store_time->tm_mon + 1) << "-";
				std::cout << std::setw(2) << std::setfill('0') << std::fixed << (store_time->tm_mday) << " ";
				std::cout << std::setw(2) << std::setfill('0') << std::fixed << (store_time->tm_hour) << ":";
				std::cout << std::setw(2) << std::setfill('0') << std::fixed << (store_time->tm_min) << ":";
				std::cout << std::setw(2) << std::setfill('0') << std::fixed << (store_time->tm_sec) << " ";

				// Write the password itself
				std::cout << i->get_password() << std::endl;
			}

			// Clear the static tm variable
			time_t clear_timestamp = 0;
			std::localtime(&clear_timestamp);
		}
		else // Only the most recent password
		{
			std::cout << "Password: " << result->get_password().get_password() << std::endl;
		}
		
		// Followed by additional data (if set)
		if (!result->get_additional_data().empty())
		{
			std::cout << "Additional data: " << result->get_additional_data() << std::endl;
		}
	
		return EXIT_SUCCESS;
	}
	else // The entry was not found
	{
		std::cout << "Nothing found that resembles '" << *query << "'." << std::endl;
		return EXIT_FAILURE;
	}
}

int cli::handle_set(const po::variables_map& vm)
{
	// Make sure the user specified a vault to use
	if (!require_vault_filename(vm))
	{
		return EXIT_FAILURE;
	}

	// Open the vault
	if(!load_vault(vm))
	{
		return EXIT_FAILURE;
	}

	// Retrieve the identifier from the command line and store it in a secure string.
	// The secure string is simply easier to use in combination with the rest of the application;
	// it adds no value since the data is insecure anyway.
	data::secure_string_ptr query = data::make_secure_string(vm.at("set").as<std::string>());

	// Create a search algorithm
	deadlock::core::search search;

	// Now execute the search
	data::entry_ptr result = search.find_match(*query, vault.begin(), vault.end());

	if (result != nullptr)
	{
		// Change values for the entry
		if (set_fields(vm, result))
		{
			// Write the vault with the updated entry
			std::cout << "'" << result->get_id() << "' updated, encrypting and writing vault ...";
			try
			{
				vault.save(vault_filename, key);
			}
			catch (const std::runtime_error& ex)
			{
				std::cout << std::endl;
				std::cerr << "Failed to write vault." << std::endl;
				std::cerr << ex.what() << std::endl;
				return EXIT_FAILURE;
			}
			std::cout << "\b\b\b\b, done." << std::endl;

			return EXIT_SUCCESS;
		}
		else // No field specified to set
		{
			std::cerr << "No fields specified to set." << std::endl;
			return EXIT_FAILURE;
		}		
	}
	else // The entry was not found
	{
		std::cout << "Nothing found that resembles '" << *query << "'." << std::endl;
		return EXIT_FAILURE;
	}
}
