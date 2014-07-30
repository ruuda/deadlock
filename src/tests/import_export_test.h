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

#ifndef _DEADLOCK_TESTS_IMPORT_EXPORT_TEST_H_
#define _DEADLOCK_TESTS_IMPORT_EXPORT_TEST_H_

#include "test.h"

namespace deadlock
{
  namespace tests
  {
    /// Tests JSON import / export functionality
    class import_export_test : public test
    {
      public:

        /// Runs the test
        void run();

        /// Returns the name of the test
        std::string get_name();
    };
  }
}

#endif
