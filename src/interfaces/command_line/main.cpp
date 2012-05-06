#include <iostream>
#include <boost/program_options.hpp>

#include "../../core/core.h"

namespace po = boost::program_options;
using namespace deadlock::core;

int main(int argc, char** argv)
{
	// Declare the supported options
	po::options_description desc("Supported commands");
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
		std::cout << "Using libdeadlock " << deadlock::core::deadlock_core_version << std::endl;
		return 0;
	}

	/// The document that will be used throughout this session
	document archive;

	std::cout << "Deadlock " << deadlock::core::deadlock_core_version << std::endl;

	return 0;
}