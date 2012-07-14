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

					/// Asks the user for a passphrase
					core::data::secure_string_ptr ask_passphrase() const;

					/// Returns the vault filename if it is present,
					/// otherwise prints a message and returns and returns an empty string.
					std::string require_vault_filename(boost::program_options::variables_map& vm);

					/// Handles the 'new vault' logic
					int handle_new(boost::program_options::variables_map& vm);

					/// Handles the 'identify' logic
					int handle_identify(boost::program_options::variables_map& vm);
			};
		}
	}
}

#endif