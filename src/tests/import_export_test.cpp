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

#include "import_export_test.h"
#include "../core/core.h"

#include <stdexcept>
#include <sstream>

using namespace deadlock::core;
using namespace deadlock::tests;

std::string import_export_test::get_name()
{
	return "import_export";
}

void import_export_test::run()
{
	// Create two initial vaults
	vault first, second;

	// Store one entry in each vault
	data::entry etr1;
	etr1.set_username("Guybrush Threepwood");
	etr1.set_key("Fictional Key 1");
	etr1.set_password("correct horse battery staple");
	etr1.set_additional_data("nothing");
	first.add_entry(etr1);

	// Store one entry in each vault
	data::entry etr2;
	etr2.set_username("Gordon Freeman");
	etr2.set_key("Fictional Key 2");
	etr2.set_password("the cake is a lie");
	second.add_entry(etr2);

	// Export both vaults, the first obfuscated, the second plain
	first.export_json("test_import_export_1.json", true);
	second.export_json("test_import_export_2.json", false);

	// Create a third vault
	vault third;

	// Import both exported vaults into it
	third.import_json("test_import_export_1.json");
	third.import_json("test_import_export_2.json");

	// TODO: validate the data in the third vault
}
