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

#ifndef _DEADLOCK_CORE_SERIALISATION_SERIALISER_H_
#define _DEADLOCK_CORE_SERIALISATION_SERIALISER_H_

#include <ostream>
#include <cstdint>
#include <stack>

#include "value.h"

namespace deadlock
{
	namespace core
	{
		namespace serialisation
		{
			/// This class serialises a value to a stream.
			/// A serialiser is constructed with one value.
			/// It can then serialise this value to a stream.
			/// The syntax followed is JSON.
			/// You need not use this class: it is used implicitly by operator<<.
			class serialiser
			{
			protected:

				enum serialisation_state
				{
					state_none,
					state_begin_list,
					state_in_list,
					state_after_key
				};

				/// The current level of indentation
				unsigned int indentation;

				/// Whether to output human-readable JSON with lots of whitespace (true)
				/// or minimal JSON without unnecessary whitespace (false)
				bool write_whitespace;

				/// The output stream
				std::ostream& ostr;

				std::stack<serialisation_state> states;

				/// Returns four bits as one character (hexadecimal representation)
				char get_hexadecimal_nibble(std::uint8_t nibble)
				{
					switch (nibble)
					{
					case 0x0: return '0';
					case 0x1: return '1';
					case 0x2: return '2';
					case 0x3: return '3';
					case 0x4: return '4';
					case 0x5: return '5';
					case 0x6: return '6';
					case 0x7: return '7';
					case 0x8: return '8';
					case 0x9: return '9';
					case 0xa: return 'a';
					case 0xb: return 'b';
					case 0xc: return 'c';
					case 0xd: return 'd';
					case 0xe: return 'e';
					case 0xf: return 'f';
					}

					throw std::runtime_error("Invalid nibble, it must be in the range 0x0 -- 0xf");
				}

				/// Writes the byte as two characters (hexadecimal representation)
				void write_hexadecimal(std::uint8_t byte)
				{
					ostr << get_hexadecimal_nibble((byte >> 4) & 0xf) << get_hexadecimal_nibble(byte & 0xf);
				}

				/// Writes a newline and indentation characters (if whitespace is enabled)
				void write_indentation()
				{
					if (write_whitespace)
					{
						if (indentation > 0)
						{
							ostr << std::endl;
						}

						ostr << std::string(indentation, '\t');
					}
				}

				/// Written before every type of value
				void write_begin_value()
				{
					if (states.top() == state_after_key)
					{
						if (write_whitespace) ostr << " ";
						states.pop();
					}
					else if (states.top() == state_begin_list)
					{
						// Newline and indent
						write_indentation();

						// After the first element, the state is inside the list
						states.pop(); states.push(state_in_list);
					}
					else if (states.top() == state_in_list)
					{
						// Append comma to previous item
						ostr << ",";

						// Newline and indent
						write_indentation();
					}
				}

			public:

				/// Creates a serialiser that can be used to write to the stream
				serialiser(std::ostream& os) : indentation(0), ostr(os), write_whitespace(false)
				{
					// The state stack must contain at least one state
					states.push(state_none);
				}

				/// Creates a serialiser that can be used to write to the stream
				/// Optionally writes whitespace to make the output more readable
				serialiser(std::ostream& os, bool human_readable) : indentation(0), ostr(os), write_whitespace(human_readable)
				{
					// The state stack must contain at least one state
					states.push(state_none);
				}

				/// Writes the string, with a few special characters escaped to form valid JSON
				void write_string(const std::string& str)
				{
					write_begin_value();

					ostr << "\"";

					size_t sz = str.size();
					for (size_t i = 0; i < sz; i++)
					{
						char c = str[i];
						if (c == '"')
							ostr << '\\' << '"';
						else if (c == '\\')
							ostr << '\\' << '\\';
						else if (c == '\b')
							ostr << '\\' << 'b';
						else if (c == '\f')
							ostr << '\\' << 'f';
						else if (c == '\n')
							ostr << '\\' << 'n';
						else if (c == '\r')
							ostr << '\\' << 'r';
						else if (c == '\t')
							ostr << '\\' << 't';
						else
							ostr << c;
					}

					ostr << "\"";
				}

				/// Writes the string, where every character is replaced by a hexadecimal escape sequence
				void write_hexadecimal_string(const std::string& str)
				{
					write_begin_value();

					ostr << "\"";

					size_t sz = str.size();
					for (size_t i = 0; i < sz; i++)
					{
						// TODO: code point size?
						char c = str[i];
						ostr << "\\u00";
						write_hexadecimal(c);
					}

					ostr << "\"";
				}

				// Writes "true"
				void write_true()
				{
					write_begin_value();
					ostr << "true";
				}

				/// Writes "false"
				void write_false()
				{
					write_begin_value();
					ostr << "false";
				}

				/// Writes a boolean
				void write_boolean(bool value)
				{
					if (value) write_true(); else write_false();
				}

				/// Writes "null"
				void write_null()
				{
					write_begin_value();
					ostr << "null";
				}

				/// Writes a number
				template <typename T> void write_number(T number)
				{
					#ifdef _DEBUG
					// Useless arithmetic that cause the template to fail for non-number types (in most cases)
					T t2 = number + 1;
					t2 *= 3;
					#endif

					write_begin_value();
					// TODO: will this always be formatted correctly?
					ostr << number;
				}

				/// Writes the start of an array
				void write_begin_array()
				{
					write_begin_value();

					// Newline and indentation
					write_indentation();

					ostr << "[";

					indentation++;
					states.push(state_begin_list);
				}

				/// Writes the end of an array
				void write_end_array()
				{
					states.pop();
					indentation--;

					// In this case write_indentation will fail
					if (indentation == 0 && write_whitespace) ostr << std::endl;

					write_indentation();

					ostr << "]";
				}

				/// Writes the start of an object
				void write_begin_object()
				{
					write_begin_value();

					// Newline and indentation
					write_indentation();

					ostr << "{";

					indentation++;
					states.push(state_begin_list);
				}

				/// Writes the end of an array
				void write_end_object()
				{
					states.pop();
					indentation--;

					write_indentation();

					// In this case write_indentation will fail
					if (indentation == 0 && write_whitespace) ostr << std::endl;

					ostr << "}";
				}

				/// Writes the key for an object member
				void write_object_key(std::string key)
				{
					write_string(key);

					ostr << ":";

					states.push(state_after_key);
				}
			};
		}
	}
}

#endif