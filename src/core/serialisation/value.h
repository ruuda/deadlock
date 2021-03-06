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

#ifndef _DEADLOCK_CORE_SERIALISATION_VALUE_H_
#define _DEADLOCK_CORE_SERIALISATION_VALUE_H_

#include <map>
#include "../data/secure_string.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace deadlock
{
  namespace core
  {
    namespace serialisation
    {
      /// The types of value as defined by JSON
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
        class deserialiser;
      }

      /// Represents a JSON value
      /// \todo better documentation
      class json_value
      {
        friend class detail::deserialiser;

      public:

        /// The map container
        /// TODO: could small-string optimisations impose a security risk here?
        typedef std::map<data::secure_string, json_value, std::less<data::secure_string>,
          data::detail::secure_allocator<std::pair<const data::secure_string, json_value>>> object_t;

        /// The list/array container
        typedef std::vector<json_value, data::detail::secure_allocator<json_value>> array_t;

      protected:

        /// Type of this class
        typedef json_value self_type;

        /// What this value actually is
        value_type type;

        /// The value
        void* value;

        /// Deletes the stored value the right way
        void delete_value()
        {
          if (type == value_type::string) delete static_cast<data::secure_string_ptr*>(value);
          else if (type == value_type::number) delete static_cast<data::secure_string_ptr*>(value);
          else if (type == value_type::object) delete static_cast<object_t*>(value);
          else if (type == value_type::array) delete static_cast<array_t*>(value);
        }

      public:

        /// Default constructor
        /// Initialises a null value
        json_value()
        {
          type = value_type::c_null;
          value = nullptr;
        }

        /// Copy constructor
        json_value(const self_type& other)
        {
          if (&other == this) return;

          type = other.type;
        
          if (type == value_type::string) value = new data::secure_string_ptr(*static_cast<data::secure_string_ptr*>(other.value));
          else if (type == value_type::number) value = new data::secure_string_ptr(*static_cast<data::secure_string_ptr*>(other.value));
          else if (type == value_type::object) value = new object_t(*static_cast<object_t*>(other.value));
          else if (type == value_type::array) value = new array_t(*static_cast<array_t*>(other.value));
        }

        /// Assignment operator
        self_type& operator=(const self_type& other)
        {
          if (&other == this) return *this;

          if (value) delete_value();

          type = other.type;

          if (type == value_type::string) value = new data::secure_string_ptr(*static_cast<data::secure_string_ptr*>(other.value));
          else if (type == value_type::number) value = new data::secure_string_ptr(*static_cast<data::secure_string_ptr*>(other.value));
          else if (type == value_type::object) value = new object_t(*static_cast<object_t*>(other.value));
          else if (type == value_type::array) value = new array_t(*static_cast<array_t*>(other.value));

          return *this;
        }

        /// Assignment constructor (string)
        json_value(data::secure_string_ptr v)
        {
          type = value_type::string;
          value = new data::secure_string_ptr(v);
        }

        /// Assignment operator (string)
        const data::secure_string& operator=(const data::secure_string& v)
        {
          if (value) delete_value();
          type = value_type::string;
          value = new data::secure_string_ptr(data::make_secure_string(v));
          return v;
        }

        /// Conversion operator (string)
        operator const data::secure_string& () const
        {
          if (type == value_type::string)
            return **static_cast<data::secure_string_ptr*>(value);
          throw std::runtime_error("The stored value is not a string.");
        }

        /// Assignment constructor (const char*)
        json_value(const char* v)
        {
          type = value_type::string;
          value = new data::secure_string_ptr(data::make_secure_string(v));
        }

        /// Assignment operator (const char*)
        std::string operator=(const char* v)
        {
          if (value) delete_value();
          type = value_type::string;
          value = new data::secure_string_ptr(data::make_secure_string(v));
          return v;
        }

        /// Assignment constructor (short)
        json_value(short v)
        {
          type = value_type::number;
          data::secure_stringstream_ptr strstr = data::make_secure_stringstream();
          (*strstr) << v;

          // TODO: the str() function is a potential vulnerability because of the short string optimisation
          value = new data::secure_string_ptr(data::make_secure_string(strstr->str()));
        }

        /// Assignment operator (short)
        short operator=(short v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (short)
        operator short () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<short>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (unsigned short)
        json_value(unsigned short v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (unsigned short)
        unsigned short operator=(unsigned short v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        ///Conversion operator (unsigned short)
        operator unsigned short () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<unsigned short>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (int)
        json_value(int v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (int)
        int operator=(int v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (int)
        operator int () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<int>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (unsigned int)
        json_value(unsigned int v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (unsigned int)
        unsigned int operator=(unsigned int v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (unsigned int)
        operator unsigned int () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<unsigned int>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (long int)
        json_value(long int v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (long int)
        long int operator=(long int v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (long int)
        operator long int () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<long int>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (unsigned long int)
        json_value(unsigned long int v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (unsigned long int)
        unsigned long int operator=(unsigned long int v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (unsigned long int)
        operator unsigned long int () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<unsigned long int>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (long long)
        json_value(long long v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (long long)
        long long operator=(long long v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (long long)
        operator long long () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<long long>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (unsigned long long)
        json_value(unsigned long long v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (unsigned long long)
        unsigned long long operator=(unsigned long long v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (unsigned long long)
        operator unsigned long long () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<unsigned long long>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (float)
        json_value(float v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (float)
        float operator=(float v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (float)
        operator float () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<float>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (double)
        json_value(double v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (double)
        double operator=(double v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (double)
        operator double () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<double>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number.");
        }

        /// Assignment constructor (long double)
        json_value(long double v)
        {
          // TODO
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
        }

        /// Assignment operator (long double)
        long double operator=(long double v)
        {
          // TODO
          if (value) delete_value();
          type = value_type::number;
          value = new data::secure_string_ptr(data::make_secure_string(boost::lexical_cast<data::secure_string>(v)));
          return v;
        }

        /// Conversion operator (long double)
        operator long double () const
        {
          // TODO
          if (type == value_type::number)
            return boost::lexical_cast<long double>(**static_cast<data::secure_string_ptr*>(value));
          throw std::runtime_error("The stored value is not a number."); //todo: derive exception
        }

        /// Assignment constructor (object)
        json_value(const object_t& v)
        {
          type = value_type::object;
          value = new object_t(std::move(v));
        }

        /// Assignment operator (object)
        object_t operator=(const object_t& v)
        {
          if (value) delete_value();
          type = value_type::object;
          value = new object_t(std::move(v));
          return v;
        }

        /// Conversion operator (object)
        operator object_t () const
        {
          if (type == value_type::object)
            return *static_cast<object_t*>(value);
          throw std::runtime_error("The stored value is not an object.");
        }

        /// Assignment constructor (array)
        json_value(const array_t& v)
        {
          type = value_type::array;
          value = new array_t(std::move(v));
        }

        /// Assignment operator (array)
        array_t operator=(const array_t& v)
        {
          if (value) delete_value();
          type = value_type::array;
          value = new array_t(std::move(v));
          return v;
        }

        /// Conversion operator (object)
        operator array_t () const
        {
          if (type == value_type::array)
            return *static_cast<array_t*>(value);
          throw std::runtime_error("The stored value is not an array.");
        }

        /// Assignment constructor (bool)
        json_value(bool v)
        {
          type = v ? value_type::c_true : value_type::c_false;
          value = nullptr;
        }

        /// Assignment operator (bool)
        bool operator=(bool v)
        {
          if (value) delete_value();
          type = v ? value_type::c_true : value_type::c_false;
          value = nullptr;
          return v;
        }

        /// Conversion operator (bool)
        /// Please note, if the value is null, it will also evaluate to false.
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

        /// Assignment constructor (null)
        json_value(std::nullptr_t)
        {
          type = value_type::c_null;
          value = nullptr;
        }

        /// Assignment operator (null)
        std::nullptr_t operator=(std::nullptr_t)
        {
          if (value) delete_value();
          type = value_type::c_null;
          value = nullptr;
          return nullptr;
        }

        /// Conversion operator (null)
        operator std::nullptr_t () const
        {
          if (type == value_type::c_null)
            return nullptr;
          throw std::runtime_error("The stored value is not null.");
        }

        /// Destructor
        /// Deletes the stored value
        ~json_value()
        {
          if (value) delete_value();
        }

        /// Object index operator
        const self_type& operator[](const data::secure_string& index) const
        {
          if (type == value_type::object)
            return (*static_cast<object_t*>(value))[index];
          throw std::runtime_error("The stored value is not an object.");
        }

        /// Object index operator
        self_type& operator[](const data::secure_string& index)
        {
          if (type != value_type::object)
            throw std::runtime_error("The stored value is not an object.");

          return (*static_cast<object_t*>(value))[index];
        }

        /// Object index operator
        const self_type& operator[](const char* index) const
        {
          if (type == value_type::object)
            return (*static_cast<object_t*>(value))[data::secure_string(index)]; // TODO: small-string vulnerabilities
          throw std::runtime_error("The stored value is not an object.");
        }

        /// Object index operator
        self_type& operator[](const char* index)
        {
          if (type != value_type::object)
            throw std::runtime_error("The stored value is not an object.");

          return (*static_cast<object_t*>(value))[data::secure_string(index)]; // TODO: small-string vulnerabilities
        }

        ///Push back something on the array
        /// This method will automatically switch the value to object, if it is not already (losing any current information).
        void push_back(const self_type& v)
        {
          if (type != value_type::array)
            throw std::runtime_error("The stored value is not an array.");
        
          static_cast<array_t*>(value)->push_back(v);
        }

        /// Returns the type of the currently stored value
        value_type get_type() const
        {
          return type;
        }
      };
    }
  }
}

#endif
