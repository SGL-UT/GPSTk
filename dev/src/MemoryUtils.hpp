#pragma ident "$Id$"

/**
* @file MemoryUtils.hpp
* 
*/

#ifndef GPSTK_MEMORYUTILS_HPP
#define GPSTK_MEMORYUTILS_HPP

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <list>
#include <algorithm>

namespace gpstk
{
   ///
   template <class C>
   class AutoReleasePool
   {
   public:
      AutoReleasePool()
      { }

      ~AutoReleasePool()
      { release();}

      void add(C* pObject)
      { if (pObject) _list.push_back(pObject); }

      void release()
      {
         while (!_list.empty())
         {
            _list.front()->release();
            _list.pop_front();
         }
      }

   protected:

      std::list<C*> _list;

   };

   // Assist classes for the class 'AutoPtr'


   class ReferenceCounter
   {
   public:
      ReferenceCounter(): counter(1)
      { }

      void duplicate()
      { ++counter; }

      int release()
      { return (--counter <= 0) ? 0 : counter; }

      int referenceCount() const
      { return counter; }

   private:
      long counter;
   };


   template <class C>
   class ReleasePolicy
   {
   public:
      static void release(C* pObj) { delete pObj; }
   };


   template <class C>
   class ReleaseArrayPolicy
   {
   public:
      static void release(C* pObj) { delete [] pObj; }
   };


      /** AutoPtr is a "smart" pointer for classes implementing
       * reference counting based garbage collection.
       *
       * AutoPtr works in the following way:
       *
       * If an AutoPtr is assigned an ordinary pointer to
       * an object (via the constructor or the assignment operator),
       * it takes ownership of the object and the object's reference 
       * count is initialized to one.
       *
       * If the AutoPtr is assigned another SharedPtr, the
       * object's reference count is incremented by one.
       * The destructor of SharedPtr decrements the object's
       * reference count by one and deletes the object if the
       * reference count reaches zero.
       *
       * AutoPtr supports dereferencing with both the ->
       * and the * operator. An attempt to dereference a null
       * SharedPtr results in a NullPointerException being thrown.
       * SharedPtr also implements all relational operators and
       * a cast operator in case dynamic casting of the encapsulated data types
       * is required.
       *
       * @warning DO NOT assign a pointer to multi AutoPtr Objects.
       */
   template <class C, class RC = ReferenceCounter, class RP = ReleasePolicy<C> >
   class AutoPtr
   {
   public:
      AutoPtr(): _pCounter(new RC), _ptr(0)
      {}

      AutoPtr(C* ptr): _pCounter(new RC), _ptr(ptr)
      {}

      template <class Other, class OtherRP> 
      AutoPtr(const AutoPtr<Other, RC, OtherRP>& ptr)
         : _pCounter(ptr._pCounter), _ptr(const_cast<Other*>(ptr.get()))
      { _pCounter->duplicate(); }

      AutoPtr(const AutoPtr& ptr)
         : _pCounter(ptr._pCounter), _ptr(ptr._ptr)
      { _pCounter->duplicate(); }

      ~AutoPtr()
      { release(); }

      AutoPtr& assign(C* ptr)
      {
         if (get() != ptr)
         {
            RC* pTmp = new RC;
            release();
            _pCounter = pTmp;
            _ptr = ptr;
         }
         return *this;
      }

      AutoPtr& assign(const AutoPtr& ptr)
      {
         if (&ptr != this)
         {
            AutoPtr tmp(ptr);
            swap(tmp);
         }
         return *this;
      }

      template <class Other, class OtherRP>
      AutoPtr& assign(const AutoPtr<Other, RC, OtherRP>& ptr)
      {
         if (ptr.get() != _ptr)
         {
            AutoPtr tmp(ptr);
            swap(tmp);
         }
         return *this;
      }

      AutoPtr& operator = (C* ptr)
      { return assign(ptr); }

      AutoPtr& operator = (const AutoPtr& ptr)
      { return assign(ptr); }

      template <class Other, class OtherRP>
      AutoPtr& operator = (const AutoPtr<Other, RC, OtherRP>& ptr)
      { return assign<Other>(ptr); }

      void swap(AutoPtr& ptr)
      {
         std::swap(_ptr, ptr._ptr);
         std::swap(_pCounter, ptr._pCounter);
      }

         /// Casts the SharedPtr via a dynamic cast to the given type.
         /// Returns an SharedPtr containing NULL if the cast fails.
         /// Example: (assume class Sub: public Super)
         ///    SharedPtr<Super> super(new Sub());
         ///    SharedPtr<Sub> sub = super.cast<Sub>();
         ///    poco_assert (sub.get());
      template <class Other> 
      AutoPtr<Other, RC, RP> cast() const
      {
         Other* pOther = dynamic_cast<Other*>(_ptr);
         if (pOther)
            return AutoPtr<Other, RC, RP>(_pCounter, pOther);
         return AutoPtr<Other, RC, RP>();
      }

         /// Casts the SharedPtr via a static cast to the given type.
         /// Example: (assume class Sub: public Super)
         ///    SharedPtr<Super> super(new Sub());
         ///    SharedPtr<Sub> sub = super.unsafeCast<Sub>();
         ///    poco_assert (sub.get());
      template <class Other> 
      AutoPtr<Other, RC, RP> unsafeCast() const
      {
         Other* pOther = static_cast<Other*>(_ptr);
         return AutoPtr<Other, RC, RP>(_pCounter, pOther);
      }

      C* operator -> ()
      { return deref(); }

      const C* operator -> () const
      { return deref(); }

      C& operator * ()
      { return *deref(); }

      const C& operator * () const
      { return *deref(); }

      C* get()
      { return _ptr; }

      const C* get() const
      { return _ptr; }

      operator C* ()
      { return _ptr; }

      operator const C* () const
      { return _ptr; }

      bool operator ! () const
      { return _ptr == 0; }

      bool isNull() const
      { return _ptr == 0; }

      bool operator == (const AutoPtr& ptr) const
      { return get() == ptr.get(); }

      bool operator == (const C* ptr) const
      { return get() == ptr; }

      bool operator == (C* ptr) const
      { return get() == ptr; }

      bool operator != (const AutoPtr& ptr) const
      { return get() != ptr.get(); }

      bool operator != (const C* ptr) const
      { return get() != ptr; }

      bool operator != (C* ptr) const
      { return get() != ptr; }

      bool operator < (const AutoPtr& ptr) const
      { return get() < ptr.get(); }

      bool operator < (const C* ptr) const
      { return get() < ptr; }

      bool operator < (C* ptr) const
      { return get() < ptr; }

      bool operator <= (const AutoPtr& ptr) const
      { return get() <= ptr.get(); }

      bool operator <= (const C* ptr) const
      { return get() <= ptr; }

      bool operator <= (C* ptr) const
      { return get() <= ptr; }

      bool operator > (const AutoPtr& ptr) const
      { return get() > ptr.get(); }

      bool operator > (const C* ptr) const
      { return get() > ptr; }

      bool operator > (C* ptr) const
      { return get() > ptr; }

      bool operator >= (const AutoPtr& ptr) const
      { return get() >= ptr.get(); }

      bool operator >= (const C* ptr) const
      { return get() >= ptr; }

      bool operator >= (C* ptr) const
      { return get() >= ptr; }

      int referenceCount() const
      { return _pCounter->referenceCount(); }

   private:
      C* deref() const
      {
         if (!_ptr) throw exception("null pointer");

         return _ptr;
      }

      void release()
      {
         //poco_assert_dbg (_pCounter);
         int i = _pCounter->release();
         if (i == 0)
         {
            RP::release(_ptr);
            _ptr = 0;

            delete _pCounter;
            _pCounter = 0;
         }
      }

      AutoPtr(RC* pCounter, C* ptr): _pCounter(pCounter), _ptr(ptr)
         /// for cast operation
      {
         //poco_assert_dbg (_pCounter);
         _pCounter->duplicate();
      }

   private:
      RC* _pCounter;
      C*  _ptr;

      template <class OtherC, class OtherRC, class OtherRP> friend class AutoPtr;
   };


   template <class C, class RC, class RP>
   inline void swap(AutoPtr<C, RC, RP>& p1, AutoPtr<C, RC, RP>& p2)
   {
      p1.swap(p2);
   }

}   // End of namespace gpstk


#endif  //GPSTK_MEMORYUTILS_HPP

