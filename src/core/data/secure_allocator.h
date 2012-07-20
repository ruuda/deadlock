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

#ifndef _DEADLOCK_CORE_DATA_SECURE_ALLOCATOR_H_
#define _DEADLOCK_CORE_DATA_SECURE_ALLOCATOR_H_

//#include <allocators>
#include <memory>
#include <cstring> // for std::memset

#define INCLUDE_WINDOWS_HEADERS
#include "../win32.h"

namespace deadlock
{
	namespace core
	{
		namespace data
		{
			namespace detail
			{
				#ifdef _WIN32
				/// Zeroes a block of memory
				inline void secure_memzero(void* buffer, size_t n)
				{
					SecureZeroMemory(buffer, n);
				}
				#else
				/// Zeroes a block of memory
				inline void secure_memzero(void* buffer, size_t n)
				{
					// TODO: use a secure vairant on other platforms as well?
					std::memset(buffer, 0, n);
				}
				#endif

				/// An allocator that zeroes memory upon deallocation
				/// Based on http://stackoverflow.com/questions/5698002/how-does-one-securely-clear-stdstring
				/// and http://stackoverflow.com/questions/3785582/how-to-write-a-password-safe-class
				template <typename T> class secure_allocator : public std::allocator<T>
				{
					public:

						typedef typename std::allocator<T>::pointer pointer;
						typedef typename std::allocator<T>::size_type size_type;

						template<typename U> struct rebind { typedef secure_allocator<U> other; };
						secure_allocator() throw() {}
						secure_allocator(const secure_allocator&) throw() {}
						template <typename U> secure_allocator(const secure_allocator<U>&) throw() {}

						void deallocate(pointer p, size_type num)
						{
							size_t q = sizeof(T) * num;
							secure_memzero(p, sizeof(T) * num);
							std::allocator<T>::deallocate(p, num);
						}
				};
			}
		}
	}
}

#endif
