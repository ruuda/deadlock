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
			/// This class can write well-formed JSON to a stream
			/// @todo: use header and cpp file
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

				/// Writes a newline and indentation characters (if whitespace is enabled)
				inline void write_indentation()
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
				/// If newline is true, it will emit a newline and indentation after an object key, otherwise a simple space.
				/// Of course whitespace will only be written to human readable streams.
				inline void write_begin_value(bool newline = false)
				{
					if (states.top() == state_after_key)
					{
						if (write_whitespace && !newline) ostr << " ";
						else if (write_whitespace) write_indentation();
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
				inline serialiser(std::ostream& os) : indentation(0), ostr(os), write_whitespace(false)
				{
					// The state stack must contain at least one state
					states.push(state_none);
				}

				/// Creates a serialiser that can be used to write to the stream
				/// Optionally writes whitespace to make the output more readable
				inline serialiser(std::ostream& os, bool human_readable) : indentation(0), ostr(os), write_whitespace(human_readable)
				{
					// The state stack must contain at least one state
					states.push(state_none);
				}

				/// Writes the secure string, with a few special characters escaped to form valid JSON
				inline void write_string(const data::secure_string& str)
				{
					write_begin_value();

					ostr << "\"";
					const char* c;

					// TODO: could compiler optimisations create vulnerabilities here?

					size_t sz = str.size();
					for (size_t i = 0; i < sz; i++)
					{
						c = &str.at(i);
						if (*c == '"')
							ostr << '\\' << '"';
						else if (*c == '\\')
							ostr << '\\' << '\\';
						else if (*c == '\b')
							ostr << '\\' << 'b';
						else if (*c == '\f')
							ostr << '\\' << 'f';
						else if (*c == '\n')
							ostr << '\\' << 'n';
						else if (*c == '\r')
							ostr << '\\' << 'r';
						else if (*c == '\t')
							ostr << '\\' << 't';
						else
							ostr << *c;
					}

					ostr << "\"";
				}

				// Writes "true"
				inline void write_true()
				{
					write_begin_value();
					ostr << "true";
				}

				/// Writes "false"
				inline void write_false()
				{
					write_begin_value();
					ostr << "false";
				}

				/// Writes a boolean
				inline void write_boolean(bool value)
				{
					if (value) write_true(); else write_false();
				}

				/// Writes "null"
				inline void write_null()
				{
					write_begin_value();
					ostr << "null";
				}

				/// Writes a number
				template <typename T> inline void write_number(T number)
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
				inline void write_begin_array()
				{
					write_begin_value(true);

					ostr << "[";

					indentation++;
					states.push(state_begin_list);
				}

				/// Writes the end of an array
				inline void write_end_array()
				{
					states.pop();
					indentation--;

					// In this case write_indentation will fail
					if (indentation == 0 && write_whitespace) ostr << std::endl;

					write_indentation();

					ostr << "]";
				}

				/// Writes the start of an object
				inline void write_begin_object()
				{
					write_begin_value(true);

					ostr << "{";

					indentation++;
					states.push(state_begin_list);
				}

				/// Writes the end of an array
				inline void write_end_object()
				{
					states.pop();
					indentation--;

					write_indentation();

					// In this case write_indentation will fail
					if (indentation == 0 && write_whitespace) ostr << std::endl;

					ostr << "}";
				}

				/// Writes the key for an object member
				inline void write_object_key(const data::secure_string& key)
				{
					write_string(key);

					ostr << ":";

					states.push(state_after_key);
				}
			};

			/// Writes a byte
			template <> inline void serialiser::write_number<std::uint8_t>(std::uint8_t number)
			{
				write_begin_value();

				ostr << (int)number;
			}

			/// Writes a byte
			template <> inline void serialiser::write_number<std::int8_t>(std::int8_t number)
			{
				write_begin_value();

				ostr << (int)number;
			}
		}
	}
}

#endif
