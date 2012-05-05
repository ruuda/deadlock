#ifndef _DEADLOCK_CORE_SERIALISATION_DESERIALISER_H_
#define _DEADLOCK_CORE_SERIALISATION_DESERIALISER_H_

#include <istream>
#include <stdexcept>
#include "value.h"

namespace deadllock
{
	namespace core
	{
		namespace serialisation
		{
			/// Indicates a problem with deserialisation
			class deserialisation_error: public std::runtime_error
			{
				public:
					deserialisation_error(std::string const& msg) : std::runtime_error(msg) {}
			};

			/// Indicates a problem with the data source: it is ill-formed.
			/// The syntax of the data source is wrong: it is ill-formed.
			/// Therefore, the deserialiser cannot correctly parse the source, and throws this exception.
			class ill_formed_source_error: public deserialisation_error
			{
				public:
					ill_formed_source_error(std::string const& msg) : deserialisation_error(msg) {}
			};

			/// Indicates a problem with the data source: the stream failed.
			/// The stream ended before deserialisation was complete. This could be because the file
			/// ended, or due to an other stream failure.
			/// Therefore, the deserialiser has not enough information to reconstruct the data,
			/// and throws this exception.
			class bad_stream_error: public deserialisation_error
			{
				public:
					bad_stream_error(std::string const& msg) : deserialisation_error(msg) {}
			};

			namespace detail
			{
				///This class deserialises a value from a stream.
				/// A deserialiser is constructed with one value.
				/// It can then deserialise the stream to this value.
				/// The syntax followed is JSON.
				/// You need not use this class: it is used implicitly by operator>>.
				/// The deserialiser might read one character extra from the stream.
				/// This is because it cannot know (in some cases), wether the
				/// next character will be relevant. Therefore, you should make
				/// sure that you append one whitespace character to your source,
				/// or just let the stream fail (due to an end-of-file, for example).
				template <typename value_t> class deserialiser
				{
				protected:
			
					/// The value on which the serialiserr acts.
					value_t& main_value;

					/// The current character
					char c;

					/// The input stream
					std::istream& istr;

					/// Reads the next character
					bool read_next()
					{
						istr.get(c);
						if (istr.good())
						{
							return true;
						}
						else
						{
							c = '\0';
							return false;
						}
					}

					/// Reads the next character, and throws a bad stream exception if it fails.
					void require_next()
					{
						if (!read_next())
							throw bad_stream_error("The stream ended before deserialisation was complete.");
					}

					/// Reads one hexadecimal character
					char read_hex()
					{
						require_next();
						switch (c)
						{
						case '0': return 0x0;
						case '1': return 0x1;
						case '2': return 0x2;
						case '3': return 0x3;
						case '4': return 0x4;
						case '5': return 0x5;
						case '6': return 0x6;
						case '7': return 0x7;
						case '8': return 0x8;
						case '9': return 0x9;
						case 'a':
						case 'A': return 0xA;
						case 'b':
						case 'B': return 0xB;
						case 'c':
						case 'C': return 0xC;
						case 'd':
						case 'D': return 0xD;
						case 'e':
						case 'E': return 0xE;
						case 'f':
						case 'F': return 0xF;
						}

						throw ill_formed_source_error("The data source is ill-formed: hex-value is invalid.");
					}

					/// Reads four hexadecimal characters and returns the UTF-8 representation
					std::string read_codepoint()
					{
						return std::string() + static_cast<char>((read_hex() << 4) | read_hex()) + static_cast<char>((read_hex() << 4) | read_hex());
					}

					/// Reads a string escape sequence
					/// Assumes '\\' has been read.
					std::string read_escape_sequence()
					{
						require_next(); //skip '\\'
						if (c == '"') return "\"";
						else if (c == '\\') return "\\";
						else if (c == '/') return "/";
						else if (c == 'b') return "\b";
						else if (c == 'f') return "\f";
						else if (c == 'n') return "\n";
						else if (c == 'r') return "\r";
						else if (c == 't') return "\t";
						else if (c == 'u') return read_codepoint();

						throw ill_formed_source_error("The data source is ill-formed: escape character is invalid.");
					}

					/// Reads a raw string
					/// Returns the string in is's raw form, not as a value.
					std::string read_string_raw()
					{
						require_next(); //skip '"'
						std::string s;

						while (c != '"') //read the string
						{
							if (c == '\\') s += read_escape_sequence();
							else s += c;
							require_next();
						}
						read_next(); //skip '"'

						return s;
					}

					/// Reads a string and returns it as a value
					value_t read_string()
					{
						return value_t(read_string_raw());
					}

					/// Reads the exponent part of a number
					std::string read_number_exponent()
					{
						if (c == 'e' || c == 'E') require_next();
						else if (c == '-')
						{
							require_next();
							return '-' + read_number_exponent();
						}
						else if ('0' <= c && '9' >= c)
						{
							char d = c;
							if (read_next())
							{
								return d + read_number_exponent();
							}
							else
							{
								return std::string(1, d);
							}
						}
						else
						{
							return "";
						}
					}

					/// Reads the part of a number after and including the decimal separator
					std::string read_number_float()
					{
						if (c == '.')
						{
							if (read_next())
							{
								return '.' + read_number_float();
							}
							else
							{
								return ".";
							}
						}
						else if (c == 'e' || c == 'E')
						{
							require_next();
							return 'e' + read_number_exponent();
						}
						else if ('0' <= c && '9' >= c)
						{
							char d = c;
							if (read_next())
							{
								return d + read_number_float();
							}
							else
							{
								return std::string(1, d);
							}
						}
						else
						{
							return "";
						}
					}

					/// Reads the integer part of a number
					std::string read_number_base()
					{
						if ('0' <= c && '9' >= c)
						{
							char d = c;
							if (read_next())
							{
								return d + read_number_base();
							}
							else
							{
								return std::string(1, d);
							}
						}
						else if (c == '.')
						{
							if (read_next())
							{
								return '.' + read_number_float();
							}
							else
							{
								return ".";
							}
						}
						else if (c == 'e' || c == 'E')
						{
							return 'e' + read_number_exponent();
						}
						else
						{
							return "";
						}
					}

					/// Reads a raw number
					/// Returns the raw string representation of the number.
					std::string read_number_raw()
					{
						if (c == '-')
						{
							require_next();
							return '-' + read_number_raw();
						}
						else if (c == '0')
						{
							if (read_next())
							{
								return '0' + read_number_float();
							}
							else
							{
								return "0";
							}
						}
						else if ('1' <= c && '9' >= c)
						{
							char d = c;
							if (read_next())
							{
								return d + read_number_base();
							}
							else
							{
								return std::string(1, d);
							}
						}

						throw ill_formed_source_error("The data source is ill-formed: number value is invalid.");
					}

					/// Reads a number from the stream
					value_t read_number()
					{
						value_t v;
						v.type = value_type::number;
						v.value = new std::string(read_number_raw());
						return v;
					}

					/// Reads true fron the stream
					/// Ill-formed JSON will fail here.
					/// To improve performance, validation has been omitted.
					value_t read_true()
					{
						read_next(); // skip 't'
						read_next(); // skip 'r'
						read_next(); // skip 'u'
						read_next(); // skip 'e'
						value_t v;
						v.type = value_type::c_true;
						return v;
					}

					/// Reads false fron the stream
					/// Ill-formed JSON will fail here.
					/// To improve performance, validation has been omitted.
					value_t read_false()
					{
						read_next(); // skip 'f'
						read_next(); // skip 'a'
						read_next(); // skip 'l'
						read_next(); // skip 's'
						read_next(); // skip 'e'
						value_t v;
						v.type = value_type::c_false;
						return v;
					}

					/// Reads null fron the stream
					/// Ill-formed JSON will fail here.
					/// To improve performance, validation has been omitted.
					value_t read_null()
					{
						read_next(); // skip 'n'
						read_next(); // skip 'u'
						read_next(); // skip 'l'
						read_next(); // skip 'l'
						value_t v;
						v.type = value_type::c_null;
						return v;
					}

					/// Reads an object from the stream
					value_t read_object()
					{
						require_next(); // skip '{'
					
						typename value_t::object_t map;

						while (c != '}')
						{
							if (c == '"')
							{
								std::string key = read_string_raw();
								while (c != ':') require_next();
								map.insert(std::make_pair(key, read_value()));
							}
							else
							{
								require_next();
							}
						}
						read_next(); // skip '}'

						return value_t(map);
					}

					/// Reads an array from the stream
					value_t read_array()
					{
						require_next(); // skip '['
					
						typename value_t::array_t arr;

						while (c != ']')
						{
							if (is_value())
							{
								arr.push_back(read_value());
							}
							else
							{
								require_next();
							}
						}
						read_next(); // skip ']'

						return value_t(arr);
					}

					/// Tests whether the current character is a valid value character
					bool is_value()
					{
						if (c == '{') return true;
						if (c == '[') return true;
						if (c == '"') return true;
						if (c == '-' || ('0' <= c && '9' >= c)) return true;
						if (c == 't') return true;
						if (c == 'f') return true;
						if (c == 'n') return true;
						return false;
					}

					/// Reads a value from the stream
					value_t read_value()
					{
						value_t value;

						do
						{
							if (c == '{')
							{
								value = read_object();
								return value;
							}
							else if (c == '[')
							{
								value = read_array();
								return value;
							}
							else if (c == '"')
							{
								value = read_string();
								return value;
							}
							else if (c == '-' || ('0' <= c && '9' >= c))
							{
								value = read_number();
								return value;
							}
							else if (c == 't')
							{
								value = read_true();
								return value;
							}
							else if (c == 'f')
							{
								value = read_false();
								return value;
							}
							else if (c == 'n')
							{
								value = read_null();
								return value;
							}

						} while (require_next(), true); // accept anything as whitespace

						return value;
					}

				public:

					/// Constructor
					/// Takes a reference to the value to use
					deserialiser(value_t& v, std::istream& is) : main_value(v), c('\0'), istr(is) {}

					/// Reads the valure from the stream
					void read()
					{
						read_next();
						main_value = read_value();
					}
				};
			}

			/// Deserialises a value from a stream
			template <
				template <typename, typename> class t_object_t,
				template <typename> class t_array_t
				>
			std::istream& operator>>(std::istream& istr, generic_value<t_object_t, t_array_t>& value)
			{
				detail::deserialiser<generic_value<t_object_t, t_array_t> >(value, istr).read();
				return istr;
			}
		}
	}
}

#endif