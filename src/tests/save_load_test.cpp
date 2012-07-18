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
#include <fstream>

using namespace deadlock::core;
using namespace deadlock::tests;

std::string save_load_test::get_name()
{
	return "save_load";
}

void save_load_test::run()
{
	// Create a key
	cryptography::key key;
	key.set_salt_random();
	data::secure_string_ptr passphrase = data::make_secure_string("correct horse battery staple");
	size_t iterations = key.get_required_iterations(passphrase->length(), 0.1);
	key.generate_key(*passphrase, iterations);

	// Create one vault that saves, one vault that loads
	vault first, second;

	// Store two entries in the vault
	data::entry etr1;
	etr1.set_username("Guybrush Threepwood");
	etr1.set_id("Fictional Key 1");
	etr1.set_password("correct horse battery staple");
	etr1.set_additional_data("nothing");
	first.add_entry(etr1);

	data::entry etr2;
	etr2.set_username("Gordon Freeman");
	etr2.set_id("Fictional Key 2");
	etr2.set_password("the cake is a lie");
	first.add_entry(etr2);

	// Save
	first.save("test_save_load.dlk", key);

	// Load
	second.load("test_save_load.dlk", key, *passphrase);
	
	vault::const_entry_iterator it = second.begin();

	// Validate the first entry
	if (it->get_username() != etr1.get_username()) throw std::runtime_error("Username not retrieved correctly.");
	if (it->get_password().get_password() != etr1.get_password().get_password()) throw std::runtime_error("Password not retrieved correctly.");
	if (it->get_password().get_stored_time() != etr1.get_password().get_stored_time()) throw std::runtime_error("Password timestamp not retrieved correctly.");
	if (it->get_id() != etr1.get_id()) throw std::runtime_error("Identifier not retrieved correctly.");
	if (it->get_additional_data() != etr1.get_additional_data()) throw std::runtime_error("Additional data not retrieved correctly.");

	it++;

	// Validate the second entry
	if (it->get_username() != etr2.get_username()) throw std::runtime_error("Username not retrieved correctly.");
	if (it->get_password().get_password() != etr2.get_password().get_password()) throw std::runtime_error("Password not retrieved correctly.");
	if (it->get_password().get_stored_time() != etr2.get_password().get_stored_time()) throw std::runtime_error("Password timestamp not retrieved correctly.");
	if (it->get_id() != etr2.get_id()) throw std::runtime_error("Identifier not retrieved correctly.");
	if (it->get_additional_data() != etr2.get_additional_data()) throw std::runtime_error("Additional data not retrieved correctly.");

	it++;
	if (it != second.end()) throw std::runtime_error("Incorrect number of entries encountered.");
}
