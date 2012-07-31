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

		("list,l", po::value<std::string>()->implicit_value(""), "list the identifiers of all stored entries, " \
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
		po::notify(vm);
	}
	catch (std::exception& ex)
	{
		std::cout << "Failed to parse command-line; " << ex.what() << "." << std::endl;
		return EXIT_FAILURE;
	}

	// Add entry to archive
	if (vm.count("add"))
	{
		return handle_add(vm);
	}

	// Show the details of one entry
	if (vm.count("show"))
	{
		return handle_show(vm);
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
	else if (vm.count("help"))
	{
		std::cout << "Deadlock password manager" << std::endl;
		std::cout << "Licensed under the GNU Public Licence" << std::endl;
		std::cout << std::endl;
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}

	// Give information about an archive
	else if (vm.count("identify"))
	{
		return handle_identify(vm);
	}

	// Print version
	else if (vm.count("version"))
	{
		std::cout << "Deadlock command-line interface (deadlock_cli)" << std::endl;
		std::cout << "Using libdeadlock " << deadlock::core::assembly_information::get_version() << std::endl;
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}

secure_string_ptr cli::ask_passphrase() const
{
	secure_string_ptr passphrase = make_secure_string();

	do
	{
		// Ask for a passphrase
		// TODO: use the secure variant that does not write to the console
		std::cout << "Passphrase: ";
		std::getline(std::cin, *passphrase);

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
		// Then the most recent password
		std::cout << "Password: " << result->get_password().get_password() << std::endl;
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
