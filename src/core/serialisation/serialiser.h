#ifndef _VG_SERIALISATION_SERIALISER_H_
#define _VG_SERIALISATION_SERIALISER_H_

#include <ostream>
#include "value.h"

namespace veniogames
{
	namespace serialisation
	{
		namespace detail
		{
			///This class serialises a value to a stream.
			/**
			A serialiser is constructed with one value.
			It can then serialise this value to a stream.
			The syntax followed is JSON.
			You need not use this class: it is used implicitly by operator<<.
			*/
			template <typename value_t> class serialiser
			{
			protected:
			
				///The value on which the serialiserr acts.
				const value_t& main_value;

				///The current level of indentation
				unsigned int indentation;

				///The output stream
				std::ostream& ostr;

				void output_value(const value_t& value)
				{
					if (value.type == value_type::string) //write string
					{
						output_string(static_cast<std::string>(value));
					}
					else if (value.type == value_type::number) //write number
					{
						ostr << *static_cast<std::string*>(value.value);
					}
					else if (value.type == value_type::object)
					{
						const typename value_t::object_t& object = *static_cast<typename value_t::object_t*>(value.value);
					
						if (object.begin() == object.end())
						{
							ostr << "{}";
						}
						else
						{
							ostr << "{" << std::endl;
							indentation++;

							auto last = object.end()--; last--;
							for (auto i = object.begin(); i != object.end(); i++)
							{
								ostr << std::string(indentation, '\t'); //indent
								output_string((*i).first); //output key
								ostr << ": ";
								output_value((*i).second); //output value
							
								if (i != last)
									ostr << ",";
								ostr << std::endl;
							}

							indentation--;
							ostr << std::string(indentation, '\t');
							ostr << "}";
						}
					}
					else if (value.type == value_type::array)
					{
						const typename value_t::array_t& array = *static_cast<typename value_t::array_t*>(value.value);
						bool normal_flow = true;
					
						if (array.begin() == array.end())
						{
							ostr << "[]";
							normal_flow = false;
						}
						else if (array.size() <= 4)
						{
							normal_flow = false;
							for (size_t i = 0; i < array.size(); i++)
							{
								if (array[i].get_type() != value_type::c_false &&
									array[i].get_type() != value_type::c_true  &&
									array[i].get_type() != value_type::c_null  &&
									array[i].get_type() != value_type::number   )
										normal_flow = true;
							}

							if (!normal_flow)
							{
								ostr << "[";

								auto last = array.end()--; last--;
								for (auto i = array.begin(); i != array.end(); i++)
								{
									output_value((*i)); //output value
							
									if (i != last)
										ostr << ", ";
								}
								ostr << "]";
							}
						}
						
						if (normal_flow)
						{
							ostr << "[" << std::endl;
							indentation++;

							auto last = array.end()--; last--;
							for (auto i = array.begin(); i != array.end(); i++)
							{
								ostr << std::string(indentation, '\t'); //indent
								output_value((*i)); //output value
							
								if (i != last)
									ostr << ",";
								ostr << std::endl;
							}

							indentation--;
							ostr << std::string(indentation, '\t');
							ostr << "]";
						}
					}
					else if (value.type == value_type::c_true)
					{
						ostr << "true";
					}
					else if (value.type == value_type::c_false)
					{
						ostr << "false";
					}
					else if (value.type == value_type::c_null)
					{
						ostr << "null";
					}
				}

				void output_string(const std::string& str)
				{
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

			public:

				///Constructor
				/**
				Takes a reference to the value to use
				*/
				serialiser(const value_t& v, std::ostream& os) : main_value(v), indentation(0), ostr(os) {}

				///Writes the value to the stream
				void write()
				{
					output_value(main_value);
				}
			};
		}

		///Serialises a value to a stream
		template <
			template <typename, typename> class t_object_t,
			template <typename> class t_array_t
			>
		std::ostream& operator<<(std::ostream& ostr, const generic_value<t_object_t, t_array_t>& value)
		{
			detail::serialiser<generic_value<t_object_t, t_array_t> >(value, ostr).write();
			return ostr;
		}
	}
}

#endif