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

#include "cryptography_initialisation.h"

extern "C"
{
	#include <tomcrypt.h>
}

#include "../errors.h"

using deadlock::core::cryptography::detail::_initialisation;

// The static instance that will be constructed when the program starts
_initialisation _initialisation::_instance = _initialisation();

int _initialisation::sha256_index = -1;
int _initialisation::aes_index = -1;

_initialisation::_initialisation()
{
	// Register the SHA-256 hash
	sha256_index = register_hash(&sha256_desc);

	// Register the AES cipher
	aes_index = register_cipher(&aes_desc);
}

_initialisation::~_initialisation()
{
	// Cleanup
	unregister_hash(&sha256_desc);
	unregister_cipher(&aes_desc);
}
