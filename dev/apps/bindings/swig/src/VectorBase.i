#pragma ident "$Id$"
#ifndef GPSTK_VECTOR_BASE_HPP
#define GPSTK_VECTOR_BASE_HPP
#include <valarray>
#include "Exception.hpp"
#include "MathBase.hpp"

namespace gpstk
{
NEW_EXCEPTION_CLASS(VectorException, gpstk::Exception);

   template <class T, class BaseClass>
   class ConstVectorBase
   {
   public:
      explicit ConstVectorBase() {}

      size_t size() const
         { return static_cast<const BaseClass*>(this)->size(); }

      // T operator[] (size_t i) const 
      //    { return constVectorRef(i); }

      // T operator() (size_t i) const 
      //    { return constVectorRef(i); }

   protected:

      inline T constVectorRef(size_t i) const
         throw(VectorException)
         {
            const BaseClass& b = static_cast<const BaseClass&>(*this);
#ifdef RANGECHECK
            if (i >= b.size())
            {
               VectorException e("Invalid ConstVectorBase index");
               GPSTK_THROW(e);
            }
#endif
            return b[i];
         }
   };
   class RefVectorBaseHelper
   {
   public:
      // static double zeroTolerance;
   };

   template <class T, class BaseClass>
   class RefVectorBase : public ConstVectorBase<T, BaseClass>,
                      public RefVectorBaseHelper
   {
   public:

      explicit RefVectorBase() {}

      // T& operator[] (size_t i) 
      //    { return vecRef(i); }

      // T& operator() (size_t i) 
      //    { return vecRef(i); }

      // BaseClass& zeroize()
      //    {
      //       BaseClass& me = static_cast<BaseClass&>(*this); 
      //       size_t i;
      //       for (i = 0; i < me.size(); i++)
      //          if (ABS(me[i]) < zeroTolerance)
      //             me[i] = T(0);
      //       return me;
      //    }

#define VecBaseArrayAssignMacroDontCheckRange(func) \
   BaseClass& me = static_cast<BaseClass&>(*this); \
   size_t i; for (i=0; i < me.size(); i++) { \
      me[i] func x[i]; \
   } \
   return me;

#ifdef RANGECHECK
#define VecBaseArrayAssignMacro(func) \
   BaseClass& me = static_cast<BaseClass&>(*this); \
   if (x.size() != me.size()) \
      { \
         VectorException e("Unequal lengths for vectors"); \
         GPSTK_THROW(e); \
      } \
   size_t i; for (i=0; i < me.size(); i++) me[i] func x[i]; \
   return me;
#else
#define VecBaseArrayAssignMacro(func) \
VecBaseArrayAssignMacroDontCheckRange(func)
#endif

#define VecBaseAtomicAssignMacro(func) \
   BaseClass& me = static_cast<BaseClass&>(*this); \
   size_t i; for (i=0; i < me.size(); i++) me[i] func x; \
   return me;

#define VecBaseNewAssignOperator(funcName, op) \
            /** Performs op on (*this).size() elements of (*this) from x */ \
   template <class E> BaseClass& funcName(const ConstVectorBase<T, E>& x) \
      { VecBaseArrayAssignMacro(op) } \
            /** Performs op on (*this).size() elements of (*this) from x */ \
   BaseClass& funcName(const std::valarray<T>& x) \
      { VecBaseArrayAssignMacro(op) } \
            /** Performs op on (*this).size() elements of (*this) from x */ \
   BaseClass& funcName(const T* x) \
      { VecBaseArrayAssignMacroDontCheckRange(op) } \
            /** Performs op on (*this).size() elements of (*this) from x */ \
   BaseClass& funcName(T x) \
      { VecBaseAtomicAssignMacro(op) }

     
      VecBaseNewAssignOperator(assignFrom, =);
      VecBaseNewAssignOperator(operator+=, +=);
      VecBaseNewAssignOperator(operator-=, -=);
      VecBaseNewAssignOperator(operator*=, *=);
      VecBaseNewAssignOperator(operator/=, /=);

      const BaseClass operator-() const
      {
         const T x=T(-1);
         BaseClass me = static_cast<BaseClass>(*this);
         size_t i;
         for (i=0; i < me.size(); i++) me(i) *= x;
         return me;
      }

   protected:
      inline T& vecRef(size_t i) 
         throw(VectorException)
         {
            BaseClass& b = static_cast<BaseClass&>(*this);
#ifdef RANGECHECK
            if (i >= b.size())
            {
               VectorException e("Invalid VectorBase index");
               GPSTK_THROW(e);
            }
#endif
            return b[i]; 
         }
   };

   template <class BaseClass>
   class VectorSliceBase
   {
   public:
         /// constructor
      explicit VectorSliceBase() {}

         /// the number of elements in the slice.
      size_t size() const
         { return static_cast<const BaseClass*>(this)->size(); }
         /// the start index in the BaseClass vector for this slice.
      size_t start() const
         { return static_cast<const BaseClass*>(this)->start(); }
         /// How many elements separate the i'th element from the i+1'th element.
      size_t stride() const
         { return static_cast<const BaseClass*>(this)->stride(); }

   protected:
         /// Given the size of the source vector, checks that the slice is valid.
      inline void vecSliceCheck(size_t sourceSize) const
         throw(VectorException)
         {
#ifdef RANGECHECK
               // sanity checks...
            if ( (start() >= sourceSize) ||
                 ((start() + (size() - 1) * stride()) >= sourceSize) )
            {
               VectorException e("Invalid range for slice");
               GPSTK_THROW(e);
            }
#endif
         }
   };

   template <class T, class BaseClass>
   class ConstVectorSliceBase : public VectorSliceBase<BaseClass>,
                             public ConstVectorBase<T, BaseClass>
   {
public:
   explicit ConstVectorSliceBase() {}
};

template <class T, class BaseClass>
class RefVectorSliceBase : public VectorSliceBase<BaseClass>,
                        public RefVectorBase<T, BaseClass>
{
public:
   explicit RefVectorSliceBase() {}
};


} 
#include "VectorBaseOperators.hpp"
#endif //GPSTK_VECTOR_BASE_HPP
