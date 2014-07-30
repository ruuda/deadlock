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

#include "cryptography_stream_test.h"
#include "../core/core.h"
#include "../core/cryptography/aes_cbc_encrypt_stream.h"
#include "../core/cryptography/aes_cbc_decrypt_stream.h"

#include <stdexcept>
#include <sstream>

using namespace deadlock::core;
using namespace deadlock::tests;

std::string cryptography_stream_test::get_name()
{
  return "cryptography_stream";
}

void cryptography_stream_test::run()
{
  // Create a key
  cryptography::key key;
  key.set_salt_random();
  data::secure_string_ptr passphrase = data::make_secure_string("correct horse battery staple");
  std::uint32_t iterations = key.get_required_iterations(passphrase->length(), 0.1);
  key.generate_key(*passphrase, iterations);

  // Pass 1: 16 bytes IV + 8 characters + 8 bytes padding
  {
    // The output stream
    std::stringstream encrypted_data_stream;
    // The encrypt cryptostream
    cryptography::aes_cbc_encrypt_stream enc_stream(encrypted_data_stream, key);
    // Write some data
    enc_stream << "12345678";
    enc_stream.close();

    // Validate length
    std::string ciphertext = encrypted_data_stream.str();
    if (ciphertext.length() != 32)
      throw std::runtime_error("Unexpected encrypted size; should be 32.");

    // Seek to the beginning of the encrypted data
    encrypted_data_stream.seekg(0, std::ios_base::beg);
    // The decrypt cryptostream
    cryptography::aes_cbc_decrypt_stream dec_stream(encrypted_data_stream, key);
    // Read some data
    std::string plaintext; dec_stream >> plaintext;

    // Validate length
    if (plaintext.length() != 8)
      throw std::runtime_error("Unexpected decrypted size; should be 8.");

    // Validate text
    if (plaintext != "12345678")
      throw std::runtime_error("Data was not decrypted correctly.");
  }
  
  // Pass 2: 16 bytes IV + 16 characters + 16 bytes padding
  {
    // The output stream
    std::stringstream encrypted_data_stream;
    // The cryptostream
    cryptography::aes_cbc_encrypt_stream enc_stream(encrypted_data_stream, key);
    // Write some data
    enc_stream << "abcdefghijklmnop";
    enc_stream.close();

    // Validate length
    std::string ciphertext = encrypted_data_stream.str();
    if (ciphertext.length() != 48)
      throw std::runtime_error("Unexpected encrypted size; should be 48.");

    // Seek to the beginning of the encrypted data
    encrypted_data_stream.seekg(0, std::ios_base::beg);
    // The decrypt cryptostream
    cryptography::aes_cbc_decrypt_stream dec_stream(encrypted_data_stream, key);
    // Read some data
    std::string plaintext; dec_stream >> plaintext;

    // Validate length
    if (plaintext.length() != 16)
      throw std::runtime_error("Unexpected decrypted size; should be 16.");

    // Validate text
    if (plaintext != "abcdefghijklmnop")
      throw std::runtime_error("Data was not decrypted correctly.");
  }

  // Pass 3: 16 bytes IV + 250 characters + 6 bytes padding
  {
    // The output stream
    std::stringstream encrypted_data_stream;
    // The cryptostream
    cryptography::aes_cbc_encrypt_stream enc_stream(encrypted_data_stream, key);
    // Write some data
    for (std::uint8_t i = 0; i < 250; i++)
    {
      enc_stream.put(i);
    }
    enc_stream.close();

    // Validate length
    std::string ciphertext = encrypted_data_stream.str();
    if (ciphertext.length() != 272)
      throw std::runtime_error("Unexpected encrypted size; should be 272.");

    // Seek to the beginning of the compressed data
    encrypted_data_stream.seekg(0, std::ios_base::beg);
    // The decrypt cryptostream
    cryptography::aes_cbc_decrypt_stream dec_stream(encrypted_data_stream, key);
    
    // Read bytes until the stream ends
    size_t n = 0;
    char j;
    while (dec_stream.get(j))
    {
      std::uint8_t i = j;
      if (i != n) throw std::runtime_error("Data was not decrypted correctly.");
      n++;
    }

    if (n != 250)
      throw std::runtime_error("Unexpected decrypted size; should be 250.");
  }

  // Pass 4: 16 bytes IV + N characters + padding
  {
    for (unsigned int amount = 0; amount < 999; amount++)
    {
      // The output stream
      std::stringstream encrypted_data_stream;
      // The cryptostream
      cryptography::aes_cbc_encrypt_stream enc_stream(encrypted_data_stream, key);
      // Write some data
      for (unsigned int i = 0; i < amount; i++)
      {
        enc_stream.put(static_cast<std::uint8_t>(i % 0x100));
      }
      enc_stream.close();

      // Calculate the amount of padding which should be added
      int mod = amount % 16;
      int padding = 16 - mod;

      // Validate length
      std::string ciphertext = encrypted_data_stream.str();
      if (ciphertext.length() != 16 + amount + padding)
        throw std::runtime_error("Unexpected encrypted size.");

      // Seek to the beginning of the compressed data
      encrypted_data_stream.seekg(0, std::ios_base::beg);
      // The decrypt cryptostream
      cryptography::aes_cbc_decrypt_stream dec_stream(encrypted_data_stream, key);
    
      // Read bytes until the stream ends
      std::uint8_t n = 0;
      char j;
      while (dec_stream.get(j))
      {
        std::uint8_t i = j;
        if (i != n) throw std::runtime_error("Data was not decrypted correctly.");
        n++;
      }

      if (n != (amount % 0x100))
        throw std::runtime_error("Unexpected decrypted size.");
    }
  }
}
