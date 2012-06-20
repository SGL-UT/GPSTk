#pragma ident "$Id$"

/**
 * @file Any.hpp
 * Modified from Poco, Original copyright by Applied Informatics.
 */

#ifndef GPSTK_ANY_HPP
#define GPSTK_ANY_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <algorithm>
#include <typeinfo>
#include "Exception.hpp"

namespace gpstk
{
      /// An Any class represents a general type and is capable of storing any type, supporting type-safe extraction
      /// of the internally stored data.
      ///
      /// Code taken from the Boost 1.33.1 library. Original copyright by Kevlin Henney. 
      /// Modified for Poco by Applied Informatics.
      /// Modified for GPSTK by Wei Yan.
   class Any
   {
   public:
      Any():_content(0){}

      template <typename ValueType>
      Any(const ValueType& value):_content(new Holder<ValueType>(value)) {}

      Any(const Any& other):_content(other._content ? other._content->clone() : 0){}

      ~Any(){delete _content;}

      Any& swap(Any& rhs)
      {
         std::swap(_content, rhs._content);
         return *this;
      }

      template <typename ValueType>
      Any& operator = (const ValueType& rhs)
      {
         Any(rhs).swap(*this);
         return *this;
      }

      Any& operator = (const Any& rhs)
      {
         Any(rhs).swap(*this);
         return *this;
      }

      bool empty() const
      {
         return !_content;
      }

      const std::type_info& type() const
      {
         return _content ? _content->type() : typeid(void);
      }

   private:
      class Placeholder
      {
      public:
         virtual ~Placeholder(){}

         virtual const std::type_info& type() const = 0;
         virtual Placeholder* clone() const = 0;
      };

      template <typename ValueType>
      class Holder: public Placeholder
      {
      public: 
         Holder(const ValueType& value):_held(value){}

         virtual const std::type_info& type() const
         {
            return typeid(ValueType);
         }

         virtual Placeholder* clone() const
         {
            return new Holder(_held);
         }

         ValueType _held;
      };

   private:
      template <typename ValueType>
      friend ValueType* AnyCast(Any*);

      template <typename ValueType>
      friend ValueType* UnsafeAnyCast(Any*);

      Placeholder* _content;
   };


      /// AnyCast operator used to extract the ValueType from an Any*. Will return a pointer
      /// to the stored value. 
      ///
      /// Example Usage: 
      ///     MyType* pTmp = AnyCast<MyType*>(pAny).
      /// Will return NULL if the cast fails, i.e. types don't match.
   template <typename ValueType>
   ValueType* AnyCast(Any* operand)
   {
      return operand && operand->type() == typeid(ValueType)
         ? &static_cast<Any::Holder<ValueType>*>(operand->_content)->_held
         : 0;
   }


      /// AnyCast operator used to extract a const ValueType pointer from an const Any*. Will return a const pointer
      /// to the stored value. 
      ///
      /// Example Usage:
      ///     const MyType* pTmp = AnyCast<MyType*>(pAny).
      /// Will return NULL if the cast fails, i.e. types don't match.
   template <typename ValueType>
   const ValueType* AnyCast(const Any* operand)     
   {
      return AnyCast<ValueType>(const_cast<Any*>(operand));
   }


      /// AnyCast operator used to extract a copy of the ValueType from an const Any&.
      ///
      /// Example Usage: 
      ///     MyType tmp = AnyCast<MyType>(anAny).
      /// Will throw a BadCastException if the cast fails.
      /// Dont use an AnyCast in combination with references, i.e. MyType& tmp = ... or const MyType& = ...
      /// Some compilers will accept this code although a copy is returned. Use the RefAnyCast in
      /// these cases.
   template <typename ValueType>
   ValueType AnyCast(const Any& operand)
   {
      ValueType* result = AnyCast<ValueType>(const_cast<Any*>(&operand));
      if (!result) throw Exception("Failed to convert between const Any types");
      return *result;
   }

      /// AnyCast operator used to extract a copy of the ValueType from an Any&.
      ///
      /// Example Usage: 
      ///     MyType tmp = AnyCast<MyType>(anAny).
      /// Will throw a BadCastException if the cast fails.
      /// Dont use an AnyCast in combination with references, i.e. MyType& tmp = ... or const MyType& tmp = ...
      /// Some compilers will accept this code although a copy is returned. Use the RefAnyCast in
      /// these cases.
   template <typename ValueType>
   ValueType AnyCast(Any& operand)
   {
      ValueType* result = AnyCast<ValueType>(&operand);
      if (!result) throw Exception("Failed to convert between Any types");
      return *result;
   }

      /// AnyCast operator used to return a const reference to the internal data. 
      ///
      /// Example Usage: 
      ///     const MyType& tmp = RefAnyCast<MyType>(anAny);
   template <typename ValueType>
   const ValueType& RefAnyCast(const Any & operand)
   {
      ValueType* result = AnyCast<ValueType>(const_cast<Any*>(&operand));
      if (!result) throw Exception("RefAnyCast: Failed to convert between const Any types");
      return *result;
   }


      /// AnyCast operator used to return a reference to the internal data.
      ///
      /// Example Usage: 
      ///     MyType& tmp = RefAnyCast<MyType>(anAny);
   template <typename ValueType>
   ValueType& RefAnyCast(Any& operand)
   {
      ValueType* result = AnyCast<ValueType>(&operand);
      if (!result) throw Exception("RefAnyCast: Failed to convert between Any types");
      return *result;
   }

      /// The "unsafe" versions of AnyCast are not part of the
      /// public interface and may be removed at any time. They are
      /// required where we know what type is stored in the any and can't
      /// use typeid() comparison, e.g., when our types may travel across
      /// different shared libraries.
   template <typename ValueType>
   ValueType* UnsafeAnyCast(Any* operand)    
   {
      return &static_cast<Any::Holder<ValueType>*>(operand->_content)->_held;
   }

      /// The "unsafe" versions of AnyCast are not part of the
      /// public interface and may be removed at any time. They are
      /// required where we know what type is stored in the any and can't
      /// use typeid() comparison, e.g., when our types may travel across
      /// different shared libraries.
   template <typename ValueType>
   const ValueType* UnsafeAnyCast(const Any* operand)
   {
      return AnyCast<ValueType>(const_cast<Any*>(operand));
   }
   
}   // End of namespace gpstk


#endif  //GPSTK_ANY_HPP

