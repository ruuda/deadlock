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

		("new,n", "create a new archive")
		("key-iterations", po::value<int>(), "the number of iterations for the key-generation algorithm") // TODO
		("key-time", po::value<double>(), "infer the number of iterations from a time in seconds")

		("identify,i", "show information about the archive")

		("export", po::value<std::string>(), "export the archive to JSON (removes encryption)")
		("no-obfuscation", "remove obfuscation from exported passwords (saves your passwords as text)")

		("import", po::value<std::string>(), "import the archive from JSON (appends to an archive)")
	;

	// Parse program options
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

	if (vm.count("help"))
	{
		std::cout << "Deadlock password manager" << std::endl;
		std::cout << "Licensed under the GNU Public Licence" << std::endl;
		std::cout << std::endl;
		std::cout << desc << std::endl;
		return 0;
	}

	if (vm.count("version"))
	{
		std::cout << "Deadlock command-line interface (deadlock_cli)" << std::endl;
		std::cout << "Using libdeadlock " << deadlock::core::assembly_information::get_version() << std::endl;
		return 0;
	}

	/// Create a new archive
	if (vm.count("new"))
	{
		return handle_new(vm);
	}

	return 0;
}

secure_string_ptr cli::ask_passphrase() const
{
	secure_string_ptr passphrase = make_secure_string();

	// Ask for a passphrase
	// TODO: use the secure variant that does not write to the console
	std::cout << "Passphrase: ";
	std::getline(std::cin, *passphrase);

	return passphrase;
}

int cli::handle_new(po::variables_map& vm)
{
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
		// Use a default access duration of 0.8 seconds (if nothing is specified)
		double duration = 0.8;
		if (vm.count("key-time"))
		{
			duration = vm.at("key-time").as<double>();
		}
		
		// Determine key iterations based on time
		key_iterations = key.get_required_iterations(passphrase->length(), duration);
	}

	// Use at least one iteration
	key_iterations = std::max<std::uint32_t>(1, key_iterations);

	// Use a random salt for the key
	key.set_salt_random();

	std::cout << "Generating key using " << key_iterations << " PBKDF2 iterations ...";
	auto start_time = boost::chrono::high_resolution_clock::now();
	key.generate_key(*passphrase, key_iterations);
	auto end_time = boost::chrono::high_resolution_clock::now();
	double duration = static_cast<double>((end_time - start_time).count()) / 1.0e9;
	std::cout << "\b\b\b\b, done in " << std::setprecision(3) << duration << " seconds." << std::endl;

	std::cout << "Encrypting and writing empty vault ...";
	archive.save("test.dlk", key);
	std::cout << "\b\b\b\b, done." << std::endl;

	return 0;
}