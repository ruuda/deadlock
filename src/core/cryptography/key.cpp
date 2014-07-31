// Deadlock – fast search-based password manager
// Copyright (C) 2012–2013 Ruud van Asseldonk

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

#include "key.h"

#include <boost/chrono.hpp>
extern "C"
{
  #include <tomcrypt.h>
}

#include "../errors.h"
#include "cryptography_initialisation.h"

using namespace deadlock::core::cryptography;
using namespace deadlock::core;
using deadlock::core::cryptography::detail::_initialisation;

key::key()
{
  number_of_iterations = 0;
}

key::~key()
{
  // Clear the key memory upon destruction
  data::detail::secure_memzero(salt_data, key_size);
  data::detail::secure_memzero(salt_data, salt_size);
}

void key::set_salt_random()
{
  // Create a random engine and distribution for generating random bytes
  // TODO: use a cryptographically strong random number generator
  std::random_device random_engine;
  std::uniform_int_distribution<std::uint8_t> random_byte(0x00, 0xff);

  // Simply copy the buffer
  for (size_t i = 0; i < salt_size; i++)
  {
    salt_data[i] = random_byte(random_engine);
  }
}

void key::generate_key(const data::secure_string& passphrase, std::uint32_t iterations)
{
  // Use PKCS5 PBKDF2 password-based key derivation function with an SHA256 hash to generate keys

  // Allocate a buffer for the password (because libtom expects bytes, not a string)
  std::uint8_t* passphrase_buffer = new std::uint8_t[passphrase.size()];

  // Copy the password
  for (size_t i = 0; i < passphrase.size(); i++)
  {
    passphrase_buffer[i] = passphrase[i];
  }

  // Derive the key from the password, using PKCS#5 PBKDF2 with SHA256 hash
  unsigned long the_key_size = key_size;
  int err;
  if ((err = pkcs_5_alg2(passphrase_buffer, passphrase.size(), salt_data, salt_size, iterations, _initialisation::sha256_index, key_data, &the_key_size))
    != CRYPT_OK)
  {
    throw key_error("Failed to derive key using password-based key derivation function: " + std::string(error_to_string(err)));
  }

  // Reset the password buffer memory before deleting it
  data::detail::secure_memzero(passphrase_buffer, passphrase.size());

  delete [] passphrase_buffer;

  number_of_iterations = iterations;
}

std::uint32_t key::get_required_iterations(size_t passphrase_length, double seconds)
{
  // Create a dummy passphrase
  std::uint8_t* passphrase = new std::uint8_t[passphrase_length];

  // Fill the buffer with numbers, actual content is not important here
  for (size_t i = 0; i < passphrase_length; i++)
  {
    passphrase[i] = static_cast<std::uint8_t>(i);
  }

  // Derive the key from the password, using PKCS#5 PBKDF2 with SHA256 hash
  int err;
  unsigned long the_key_size = key_size;

  // Measure the time it takes to do 4096 iterations
  auto start_time = boost::chrono::high_resolution_clock::now();
  if ((err = pkcs_5_alg2(passphrase, passphrase_length, salt_data, salt_size, 4096, _initialisation::sha256_index, key_data, &the_key_size))
    != CRYPT_OK)
  {
    throw key_error("Password-based key derivation function failed while determining required number of iterations: " + std::string(error_to_string(err)));
  }
  auto end_time = boost::chrono::high_resolution_clock::now();

  // TODO: what about short durations, where the resolution of std::clock() is too low?
  // And what about division by zero due to these inaccuracies?

  // Now calculate the time it took do do the 4096 iterations
  double seconds_4096 = static_cast<double>((end_time - start_time).count()) / 1.0e9;

  // This should give an indication of the number of iterations per second
  double iterations_per_second = 4096 / seconds_4096;

  // Make a first estimate of the required number of iterations
  std::uint32_t required_iterations = static_cast<std::uint32_t>(iterations_per_second * seconds);


  // Second pass
  // Measure the time to do this amount of iterations
  start_time = boost::chrono::high_resolution_clock::now();
  if ((err = pkcs_5_alg2(passphrase, passphrase_length, salt_data, salt_size, required_iterations, _initialisation::sha256_index, key_data, &the_key_size)) != CRYPT_OK)
  {
    throw key_error("Password-based key derivation function failed while determining required number of iterations.");
  }
  end_time = boost::chrono::high_resolution_clock::now();

  // Now calculate the time it took do do that number of iterations
  double seconds_eps = static_cast<double>((end_time - start_time).count()) / 1e9;

  // This should give a better indication of the number of iterations per second
  iterations_per_second = required_iterations / seconds_eps;

  // Make a better estimate of the required number of iterations
  required_iterations = static_cast<std::uint32_t>(iterations_per_second * seconds);

  // Clean the buffer
  delete [] passphrase;

  return required_iterations;
}
