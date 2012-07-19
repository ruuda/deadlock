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
#include "../../core/errors.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <boost/chrono.hpp>

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

		("add,a", po::value<std::string>(), "add a new entry to the vault, with the specified key")

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

std::string cli::require_vault_filename(po::variables_map& vm)
{
	if (!vm.count("vault") || vm.at("vault").as<std::string>().empty())
	{
		std::cout << "No vault specified." << std::endl;
		return "";
	}

	return vm.at("vault").as<std::string>();
}

int cli::handle_new(po::variables_map& vm)
{
	std::string filename = require_vault_filename(vm);
	if (filename.empty())
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
	std::cout << "\b\b\b\b, done in " << std::setprecision(3) << duration << " seconds." << std::endl;

	std::cout << "Encrypting and writing empty vault ...";
	vault.save(filename, key);
	std::cout << "\b\b\b\b, done." << std::endl;

	return EXIT_SUCCESS;
}


int cli::handle_identify(po::variables_map& vm)
{
	std::string filename = require_vault_filename(vm);
	if (filename.empty())
	{
		return EXIT_FAILURE;
	}

	// Try to load the file, and see what happens
	try
	{
		vault.load(filename, key, *data::make_secure_string("no_passphrase"));
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

int cli::handle_add(po::variables_map& vm)
{
	std::string filename = require_vault_filename(vm);
	if (filename.empty())
	{
		return EXIT_FAILURE;
	}

	// Ask the user for his passphrase
	data::secure_string_ptr passphrase = ask_passphrase();	

	// Try to load the vault
	try
	{
		vault.load(filename, key, *passphrase);
	}
	// Check for incorrect key
	catch (incorrect_key_error&)
	{
		std::cerr << "The key is incorrect." << std::endl;
		return EXIT_FAILURE;
	}
	// If anything other goes wrong, report error.
	catch (std::runtime_error& ex)
	{
		std::cerr << "Could not open vault." << std::endl;
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	//std::cout << "Encrypting and writing empty vault ...";
	//vault.save(filename, key);
	//std::cout << "\b\b\b\b, done." << std::endl;

	return EXIT_SUCCESS;
}