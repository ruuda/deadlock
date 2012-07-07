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

#ifndef _DEADLOCK_CORE_CRYPTOGRAPHY_INITIALISATION_H_
#define _DEADLOCK_CORE_CRYPTOGRAPHY_INITIALISATION_H_

namespace deadlock
{
	namespace core
	{
		namespace cryptography
		{
			namespace detail
			{
				/// Registers the hash and cipher functions used by LibTomCrypt upon construction
				class _initialisation
				{
				private:

					/// Registers the required LibTomCrypt hashes and ciphers
					/// Private because there should be one single instance, also contained within this class
					_initialisation();

					/// A static instance to make sure the constructor gets called once
					static _initialisation _instance;

				public:

					/// Unregisters the LibTomCrypt hashes and ciphers
					~_initialisation();

					/// The LibTomCrypt index for the SHA-256 hash
					static int sha256_index;

					/// The LibTomCrypt index for the AES cipher
					static int aes_index;
				};
			}
		}
	}
}

#endif