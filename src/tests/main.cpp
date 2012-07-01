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
#include <stdexcept>

#include "test.h"
#include "import_export_test.h"
#include "compression_stream_test.h"

using namespace deadlock::tests;

int main(int, char**)
{
	// Create a list of things to test
	test* unit_tests[] =
	{
		new import_export_test(),
		new compression_stream_test()
	};

	const size_t number_of_tests = sizeof(unit_tests) / sizeof(test*);
	size_t failed_tests = 0;

	std::cout << "Deadlock unit tests" << std::endl << std::endl;

	for (size_t i = 0; i < number_of_tests; i++)
	{
		std::cout << "testing " << unit_tests[i]->get_name() << std::endl;
		try
		{
			unit_tests[i]->run();

			std::cout << "test passed" << std::endl << std::endl;
		}
		catch (std::exception& ex)
		{
			failed_tests++;
			std::cout << "test failed: " << ex.what() << std::endl << std::endl;
		}
		catch (...)
		{
			failed_tests++;
			std::cout << "test failed for unknown reasons" << std::endl << std::endl;
		}

		// Immediately clean up the test
		delete unit_tests[i];
	}

	if (failed_tests)
	{
		std::cout << failed_tests << "/" << number_of_tests << "failed" << std::endl;
	}
	else
	{
		std::cout << "all tests passed succesfully" << std::endl;
	}

	return 0;
}
