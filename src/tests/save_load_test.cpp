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

#include "save_load_test.h"
#include "../core/core.h"

#include <stdexcept>
#include <sstream>

using namespace deadlock::core;
using namespace deadlock::tests;

std::string save_load_test::get_name()
{
	return "save_load";
}

void save_load_test::run()
{
	// Create a key
	cryptography::key_generator key;
	key.set_salt_random();
	std::string passphrase = "correct horse battery staple";
	size_t iterations = key.get_required_iterations(passphrase.length(), 0.1);
	key.generate_key(passphrase, iterations);

	// Create one vault that saves, one vault that loads
	vault first, second;

	// Store two entries in the vault
	data::entry etr1;
	etr1.set_username("Guybrush Threepwood");
	etr1.set_key("Fictional Key 1");
	etr1.set_password(data::obfuscated_string(first.obfuscate_string("correct horse battery staple")));
	etr1.set_additional_data("nothing");
	first.add_entry(etr1);

	data::entry etr2;
	etr2.set_username("Gordon Freeman");
	etr2.set_key("Fictional Key 2");
	etr2.set_password(data::obfuscated_string(first.obfuscate_string("the cake is a lie")));
	first.add_entry(etr2);

	// Save
	first.obfuscate_key(key);
	first.save("test_save_load.dlk", key);

	// Load
	second.load("test_save_load.dlk", key, passphrase);

	// TODO: validate contents
}
