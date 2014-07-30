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

#include "search.h"

#include <cctype>

using namespace deadlock::core;

search::search()
{
  
}

data::secure_string_ptr search::make_acronym(const data::secure_string& words) const
{
  // Start with an empty string
  data::secure_string_ptr acronym = data::make_secure_string();

  bool inside_word = false;
  // Loop through the characters
  for (auto i = words.begin(); i != words.end(); i++)
  {
    // TODO: what about UTF-8 etc?
    bool alnum = std::isalnum(*i) != 0;

    // At the start of a word, append the character to the acronym.
    if (alnum && !inside_word)
    {
      data::combine_secure_string(acronym, *i);
    }

    inside_word = alnum;
  }

  return acronym;
}

int search::cross_match_words(const data::secure_string_vector& needles, const data::secure_string_vector& haystacks) const
{
  int sum = 0;

  for (auto i = needles.begin(); i != needles.end(); i++)
  {
    for (auto j = haystacks.begin(); j != haystacks.end(); j++)
    {
      sum += match_words(*i, *j);
    }
  }

  return sum;
}

int search::match_words(const data::secure_string& needle, const data::secure_string& haystack) const
{
  int match = 0;

  // Loop over the minimum subset of the string beginnings, and check whether they are equal.
  size_t min_length = std::min(needle.length(), haystack.length());
  size_t max_length = std::max(needle.length(), haystack.length());
  
  int front_bonus = 0;

  // Loop through the characters
  for (int i = 0; i < min_length; i++)
  {
    // Increase the match probability for every character that is equal
    if (needle[i] == haystack[i])
    {
      front_bonus++;
    }
    else
    {
      front_bonus = 0;
      break;
    }
  }

  match += front_bonus;

  // If the words are exactly equal, add an additional bonus
  if (needle.length() == haystack.length() && front_bonus == min_length)
  {
    match += 15;
  }
  else
  {
    // Now check backwards whether one string ends with the other
    if (needle.length() <= haystack.length())
    {
      int end_bonus = 0;
      for (int i = 0; i < min_length; i++)
      {
        if (haystack[max_length - 1 - i] == needle[min_length - 1 - i])
        {
          end_bonus++;
        }
        else
        {
          end_bonus = 0;
          break;
        }
      }

      match += end_bonus;
    }
  }

  return match;
}