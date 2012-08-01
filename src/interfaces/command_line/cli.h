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

#ifndef _DEADLOCK_INTERFACES_COMMAND_LINE_CLI_H_
#define _DEADLOCK_INTERFACES_COMMAND_LINE_CLI_H_

#ifdef _WIN32
#define BOOST_ALL_STATIC_LINK
#endif
#include <boost/program_options.hpp>
#include <string>

#include "../../core/core.h"

namespace deadlock
{
	namespace interfaces
	{
		namespace command_line
		{
			/// The Deadlock command-line interface
			class cli
			{
				public:

					/// Executes the program
					int run(int argc, char** argv);

				protected:

					/// The vault that will be used throughout the session
					core::vault vault;

					/// The key that will be used throughout the session
					core::cryptography::key key;

					/// The file that the vault was loaded from
					std::string vault_filename;

					/// Asks the user for a passphrase
					core::data::secure_string_ptr ask_passphrase() const;

					/// Tries to open the vault, asks the user for a passphrase in the process,
					/// and returns whether the operation was successful.
					/// If not, it prints an error message and returns false.
					bool load_vault(const boost::program_options::variables_map& vm);

					/// Sets the vault filename if it is present,
					/// otherwise prints a message and returns false.
					bool require_vault_filename(const boost::program_options::variables_map& vm);

					/// Sets identifier, username, or additional data fields for an entry.
					/// Returns wheher any value was set.
					bool set_fields(const boost::program_options::variables_map& vm, deadlock::core::data::entry_ptr entr);

					/// Handles the 'new vault' logic
					int handle_new(const boost::program_options::variables_map& vm);

					/// Handles the 'identify' logic
					int handle_identify(const boost::program_options::variables_map& vm);

					/// Handles the 'add' logic
					int handle_add(const boost::program_options::variables_map& vm);

					/// Handles the 'list' logic
					int handle_list(const boost::program_options::variables_map& vm);

					/// Handles the 'show' logic
					int handle_show(const boost::program_options::variables_map& vm);

					/// Handles the 'set' logic
					int handle_set(const boost::program_options::variables_map& vm);
			};
		}
	}
}

#endif
