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


#ifndef _DEADLOCK_CORE_WIN32_HEADERS_H_
#define _DEADLOCK_CORE_WIN32_HEADERS_H_

#if defined(_WIN32)
  // Troublesome windows headers mess up everything, so this is a way to include them without much trouble
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>

  #undef min
  #undef max
#endif

#endif