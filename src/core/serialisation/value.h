#ifndef _VG_SERIALISATION_VALUE_H_
#define _VG_SERIALISATION_VALUE_H_

#include <map>
#include <string>
#include <boost/lexical_cast.hpp>

namespace veniogames
{
	namespace serialisation
	{
		///The types of value as defined by JSON
		enum value_type
		{
			string,
			number,
			object,
			array,
			c_true,
			c_false,
			c_null
		};

		namespace detail
		{
			///use std::map as object type
			template <typename key_t, typename value_t> struct std_map_object { typedef std::map<key_t, value_t> type; };

			///use std::vector as array type
			template <typename value_t> struct std_vector_array { typedef std::vector<value_t> type; };

			template<typename> class serialiser;
			template<typename> class deserialiser;
		}

		///Represents a JSON value
		/**
		todo: implement move/forwarding scemantics.
		todo: serialisation guide
		*/
		template <
			template <typename, typename> class t_object_t,
			template <typename> class t_array_t
			>
		class generic_value
		{
			template<typename> friend class detail::serialiser;
			template<typename> friend class detail::deserialiser;

		public:

			///The map type
			typedef typename t_object_t<std::string, generic_value>::type object_t;

			///The map type
			typedef typename t_array_t<generic_value>::type array_t;

		protected:

			///Type of this class
			typedef generic_value<t_object_t, t_array_t> self_type;

			///What this value actually is
			value_type type;

			///The value
			void* value;

			///Deletes the stored value the right way
			void delete_value()
			{
				if (type == value_type::string) delete static_cast<std::string*>(value);
				else if (type == value_type::number) delete static_cast<std::string*>(value);
				else if (type == value_type::object) delete static_cast<object_t*>(value);
				else if (type == value_type::array) delete static_cast<array_t*>(value);
			}

		public:

			///Default constructor
			/**
			Makes this value equal to null.
			*/
			generic_value()
			{
				type = value_type::c_null;
				value = nullptr;
			}

			///Copy constructor
			generic_value(const self_type& other)
			{
				if (&other == this) return;

				type = other.type;
				
				if (type == value_type::string) value = new std::string(*static_cast<std::string*>(other.value));
				else if (type == value_type::number) value = new std::string(*static_cast<std::string*>(other.value));
				else if (type == value_type::object) value = new object_t(*static_cast<object_t*>(other.value));
				else if (type == value_type::array) value = new array_t(*static_cast<array_t*>(other.value));
			}

			///Assignment operator
			self_type& operator=(const self_type& other)
			{
				if (&other == this) return *this;

				if (value) delete_value();

				type = other.type;

				if (type == value_type::string) value = new std::string(*static_cast<std::string*>(other.value));
				else if (type == value_type::number) value = new std::string(*static_cast<std::string*>(other.value));
				else if (type == value_type::object) value = new object_t(*static_cast<object_t*>(other.value));
				else if (type == value_type::array) value = new array_t(*static_cast<array_t*>(other.value));

				return *this;
			}

			///Assignment constructor (string)
			generic_value(const std::string& v)
			{
				type = value_type::string;
				value = new std::string(std::move(v));
			}

			///Assignment operator (string)
			std::string operator=(const std::string& v)
			{
				if (value) delete_value();
				type = value_type::string;
				value = new std::string(std::move(v));
				return v;
			}

			///Conversion operator (string)
			operator std::string () const
			{
				if (type == value_type::string)
					return *static_cast<std::string*>(value);
				throw std::runtime_error("The stored value is not a string.");
			}

			///Assignment constructor (const char*)
			generic_value(const char* v)
			{
				type = value_type::string;
				value = new std::string(v);
			}

			///Assignment operator (const char*)
			std::string operator=(const char* v)
			{
				if (value) delete_value();
				type = value_type::string;
				value = new std::string(v);
				return v;
			}

			///Assignment constructor (short)
			generic_value(short v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (short)
			short operator=(short v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (short)
			operator short () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<short>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (unsigned short)
			generic_value(unsigned short v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (unsigned short)
			unsigned short operator=(unsigned short v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (unsigned short)
			operator unsigned short () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<unsigned short>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (int)
			generic_value(int v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (int)
			int operator=(int v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (int)
			operator int () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<int>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (unsigned int)
			generic_value(unsigned int v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (unsigned int)
			unsigned int operator=(unsigned int v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (unsigned int)
			operator unsigned int () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<unsigned int>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (long int)
			generic_value(long int v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (long int)
			long int operator=(long int v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (long int)
			operator long int () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<long int>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (unsigned long int)
			generic_value(unsigned long int v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (unsigned long int)
			unsigned long int operator=(unsigned long int v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (unsigned long int)
			operator unsigned long int () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<unsigned long int>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (long long)
			generic_value(long long v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (long long)
			long long operator=(long long v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (long long)
			operator long long () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<long long>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (unsigned long long)
			generic_value(unsigned long long v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (unsigned long long)
			unsigned long long operator=(unsigned long long v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (unsigned long long)
			operator unsigned long long () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<unsigned long long>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (float)
			generic_value(float v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (float)
			float operator=(float v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (float)
			operator float () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<float>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (double)
			generic_value(double v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (double)
			double operator=(double v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (double)
			operator double () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<double>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number.");
			}

			///Assignment constructor (long double)
			generic_value(long double v)
			{
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
			}

			///Assignment operator (long double)
			long double operator=(long double v)
			{
				if (value) delete_value();
				type = value_type::number;
				value = new std::string(boost::lexical_cast<std::string>(v));
				return v;
			}

			///Conversion operator (long double)
			operator long double () const
			{
				if (type == value_type::number)
					return boost::lexical_cast<long double>(*static_cast<std::string*>(value));
				throw std::runtime_error("The stored value is not a number."); //todo: derive exception
			}

			///Assignment constructor (object)
			generic_value(const object_t& v)
			{
				type = value_type::object;
				value = new object_t(std::move(v));
			}

			///Assignment operator (object)
			object_t operator=(const object_t& v)
			{
				if (value) delete_value();
				type = value_type::object;
				value = new object_t(std::move(v));
				return v;
			}

			///Conversion operator (object)
			operator object_t () const
			{
				if (type == value_type::object)
					return *static_cast<object_t*>(value);
				throw std::runtime_error("The stored value is not an object.");
			}

			///Assignment constructor (array)
			generic_value(const array_t& v)
			{
				type = value_type::array;
				value = new array_t(std::move(v));
			}

			///Assignment operator (array)
			array_t operator=(const array_t& v)
			{
				if (value) delete_value();
				type = value_type::array;
				value = new array_t(std::move(v));
				return v;
			}

			///Conversion operator (object)
			operator array_t () const
			{
				if (type == value_type::array)
					return *static_cast<array_t*>(value);
				throw std::runtime_error("The stored value is not an array.");
			}

			///Assignment constructor (bool)
			generic_value(bool v)
			{
				type = v ? value_type::c_true : value_type::c_false;
				value = nullptr;
			}

			///Assignment operator (bool)
			bool operator=(bool v)
			{
				if (value) delete_value();
				type = v ? value_type::c_true : value_type::c_false;
				value = nullptr;
				return v;
			}

			///Conversion operator (bool)
			/**
			Please note, if the value is null, it will also evaluate to false.
			*/
			operator bool () const
			{
				if (type == value_type::c_true)
					return true;
				if (type == value_type::c_false)
					return false;
				if (type == value_type::c_null)
					return false;
				throw std::runtime_error("The stored value is neither true nor false.");
			}

			///Assignment constructor (null)
			generic_value(std::nullptr_t v)
			{
				type = value_type::c_null;
				value = nullptr;
			}

			///Assignment operator (null)
			std::nullptr_t operator=(std::nullptr_t v)
			{
				if (value) delete_value();
				type = value_type::c_null;
				value = nullptr;
				return nullptr;
			}

			///Conversion operator (null)
			operator std::nullptr_t () const
			{
				if (type = value_type::c_null)
					return nullptr;
				throw std::runtime_error("The stored value is not null.");
			}

			///Destructor
			/**
			Deletes the stored value
			*/
			~generic_value()
			{
				if (value) delete_value();
			}

			///Object index operator
			const self_type& operator[](const std::string& index) const
			{
				if (type == value_type::object)
					return (*static_cast<const object_t*>(value))[index];
				throw std::runtime_error("The stored value is not an object.");
			}

			///Object index operator
			self_type& operator[](const std::string& index)
			{
				if (type != value_type::object)
					throw std::runtime_error("The stored value is not an object.");

				return (*static_cast<object_t*>(value))[index];
			}

			///Object index operator
			const self_type& operator[](const char* index) const
			{
				if (type == value_type::object)
					return (*static_cast<const object_t*>(value))[std::string(index)];
				throw std::runtime_error("The stored value is not an object.");
			}

			///Object index operator
			self_type& operator[](const char* index)
			{
				if (type != value_type::object)
					throw std::runtime_error("The stored value is not an object.");

				return (*static_cast<object_t*>(value))[std::string(index)];
			}

			///Push back something at the array
			/**
			This operator will automatically switch the value to object, if it is not already (losing any current information).
			*/
			void push_back(const self_type& v)
			{
				if (type != value_type::array)
					throw std::runtime_error("The stored value is not an array.");
				
				static_cast<array_t*>(value)->push_back(v);
			}

			///Returns the type of the currently stored value
			value_type get_type() const
			{
				return type;
			}
		};

		///Normal value
		typedef generic_value<detail::std_map_object, detail::std_vector_array> value;
	}
}

#endif