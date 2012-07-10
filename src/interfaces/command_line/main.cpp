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

#include <iostream>

#ifdef _WIN32
#define BOOST_ALL_STATIC_LINK
#endif
#include <boost/program_options.hpp>

#include "../../core/core.h"

namespace po = boost::program_options;
using namespace deadlock::core;

int main(int argc, char** argv)
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

	/// The vault that will be used throughout this session
	vault archive;

	std::cout << "Deadlock " << deadlock::core::assembly_information::get_version() << std::endl;

	return 0;
}
