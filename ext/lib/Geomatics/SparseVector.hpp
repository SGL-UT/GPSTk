/// @file SparseVector.hpp  Class for a sparse vector.

#ifndef SPARSE_VECTOR_INCLUDE
#define SPARSE_VECTOR_INCLUDE

#include "MathBase.hpp"       // defines ABS SQRT
//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file SparseVector.hpp Class for template sparse vectors; use with SparseMatrix.

// TODO
// outer product?

#include <cstdlib>

#include <string>
#include <map>
#include <vector>
#include <algorithm>    // for find,lower_bound
#include <sstream>

#include "Vector.hpp"
#include "Matrix.hpp"

namespace gpstk
{
   /// forward declarations
   template <class T> class SparseVector;
   template <class T> class SparseMatrix;

   //---------------------------------------------------------------------------
   /// Proxy class for elements of the SparseVector (SV). This allows disparate
   /// treatment of rvalues and lvalues.
   template <class T> class SVecProxy
   {
   public:
      /// constructor
      SVecProxy(SparseVector<T>& SV, unsigned int index);

      /// operator = for non-const (lvalue)
      SVecProxy& operator=(const SVecProxy<T>& rhs)
         { assign(rhs); return *this; }
      /// operator = for const (rvalue)
      SVecProxy& operator=(T rhs)
         { assign(rhs); return *this; }

      /// cast or implicit conversion
      operator T() const;

      /// operator+= for non-const (lvalue)
      SVecProxy& operator+=(const SVecProxy<T>& rhs)
         { assign(value()+rhs); return *this; }

      /// operator+= for const (rvalue)
      SVecProxy& operator+=(T rhs)
         { assign(value()+rhs); return *this; }

      /// operator-= for non-const (lvalue)
      SVecProxy& operator-=(const SVecProxy<T>& rhs)
         { assign(value()-rhs); return *this; }

      /// operator-= for const (rvalue)
      SVecProxy& operator-=(T rhs)
         { assign(value()-rhs); return *this; }

      /// operator*= for non-const (lvalue)
      SVecProxy& operator*=(const SVecProxy<T>& rhs)
         { assign(value()*rhs); return *this; }

      /// operator*= for const (rvalue)
      SVecProxy& operator*=(T rhs)
         { assign(value()*rhs); return *this; }

   private:
      /// reference to the vector to which this data belongs
      SparseVector<T>& mySV;

      /// index in mySV for this data
      unsigned int index;

      /// get the value of the SparseVector at index
      T value(void) const;

      /// assign the SparseVector element, used by operator=,+=,etc
      void assign(T rhs);

   }; // end class SVecProxy


   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   // must declare friends before the class
   // output stream
   template <class T>
      std::ostream& operator<<(std::ostream& os, const SparseVector<T>& SV);
   // operators
   template <class T> T norm(const SparseVector<T>& SV);
   template <class T> T cosVec(const SparseVector<T>& S1, const SparseVector<T>& S2);
   template <class T> T cosVec(const SparseVector<T>& SV, const Vector<T>& V);
   template <class T> T cosVec(const Vector<T>& V, const SparseVector<T>& SV);
   template <class T> T dot(const SparseVector<T>& SL, const SparseVector<T>& SR);
   template <class T> T dot_lim(const SparseVector<T>& SL, const SparseVector<T>& SR,
                                const unsigned int kb, const unsigned int ke);
   template <class T> T dot(const SparseVector<T>& SL, const Vector<T>& SR);
   template <class T> T dot(const Vector<T>& SL, const SparseVector<T>& SR);
   template <class T> T min(const SparseVector<T>& SV);
   template <class T> T max(const SparseVector<T>& SV);
   template <class T> T minabs(const SparseVector<T>& SV);
   template <class T> T maxabs(const SparseVector<T>& SV);

   // TD? outer product - put in Matrix
   // addition and subtraction
   template <class T> SparseVector<T> operator-(const SparseVector<T>& L,
                                                const SparseVector<T>& R);
   template <class T> SparseVector<T> operator-(const SparseVector<T>& L,
                                                const Vector<T>& R);
   template <class T> SparseVector<T> operator-(const Vector<T>& L,
                                                const SparseVector<T>& R);
   template <class T> SparseVector<T> operator+(const SparseVector<T>& L,
                                                const SparseVector<T>& R);
   template <class T> SparseVector<T> operator+(const SparseVector<T>& L,
                                                const Vector<T>& R);
   template <class T> SparseVector<T> operator+(const Vector<T>& L,
                                                const SparseVector<T>& R);
   // SparseMatrix
   template <class T> SparseMatrix<T> transpose(const SparseMatrix<T>& M);
   template <class T> SparseMatrix<T> transform(const SparseMatrix<T>& M,
                                       const SparseMatrix<T>& C) throw(Exception);
   template <class T> SparseVector<T> operator*(const SparseMatrix<T>& L,
                                                const SparseVector<T>& V);
   template <class T> SparseVector<T> operator*(const SparseMatrix<T>& L,
                                                const Vector<T>& V);
   template <class T> SparseMatrix<T> operator*(const SparseMatrix<T>& L,
                                                const SparseMatrix<T>& R);
   template <class T> SparseMatrix<T> operator*(const SparseMatrix<T>& L,
                                                const Matrix<T>& R);
   template <class T> SparseMatrix<T> operator*(const Matrix<T>& L,
                                                const SparseMatrix<T>& R);
   template <class T> SparseMatrix<T> operator||(const SparseMatrix<T>& L,
                                                 const Vector<T>& V);
   template <class T> SparseMatrix<T> operator||(const SparseMatrix<T>& L,
                                                 const SparseMatrix<T>& R);
   template <class T> SparseMatrix<T> inverse(const SparseMatrix<T>& A)
                                                               throw(Exception);
   template <class T> SparseMatrix<T> lowerCholesky(const SparseMatrix<T>& A)
                                                    throw(Exception);
   template <class T> SparseMatrix<T> inverseLT(const SparseMatrix<T>& LT,
                             T *ptrSmall=NULL, T *ptrBig=NULL) throw(Exception);
   // special matrices
   template <class T> SparseMatrix<T> identSparse(const unsigned int dim) throw();

   // diag of P * C * PT
   template <class T> Vector<T> transformDiag(const SparseMatrix<T>& P,
                                              const Matrix<T>& C) throw(Exception);
   // Householder
   template <class T> SparseMatrix<T> SparseHouseholder(const SparseMatrix<T>& A)
                                                    throw(Exception);
   template <class T> void SrifMU(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& A,
                                  const unsigned int M=0) throw(Exception);
   template <class T> void SrifMU(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& P,
                             Vector<T>& D, const unsigned int M=0) throw(Exception);

   //---------------------------------------------------------------------------
   /// Class SparseVector. This class is designed to present an interface nearly
   /// identical to class Vector, but more efficiently handle sparse vectors, in
   /// which most of the elements are zero. The class stores only non-zero elements
   /// in a map with key = index; it also stores a nominal length. The class uses
   /// a proxy class, SVecProxy, to access elements; this allows rvalues and
   /// lvalues to be treated separately.
   template <class T> class SparseVector
   {
   public:
      /// Proxy needs access to vecMap
      friend class SVecProxy<T>;
      friend class SparseMatrix<T>;

      /// lots of friends
      // output stream operator
      friend std::ostream& operator<< <T>(std::ostream& os, const SparseVector<T>& S);
      // operators: norm, cos, etc (dot is member)
      friend T norm<T>(const SparseVector<T>& SV);
      friend T cosVec<T>(const SparseVector<T>& S1, const SparseVector<T>& S2);
      friend T cosVec<T>(const SparseVector<T>& SV, const Vector<T>& V);
      friend T cosVec<T>(const Vector<T>& V, const SparseVector<T>& SV);
      friend T dot<T>(const SparseVector<T>& SL, const SparseVector<T>& SR);
      friend T dot_lim<T>(const SparseVector<T>& SL, const SparseVector<T>& SR,
                          const unsigned int kb, const unsigned int ke);
      friend T dot<T>(const SparseVector<T>& SL, const Vector<T>& SR);
      friend T dot<T>(const Vector<T>& SL, const SparseVector<T>& SR);
      friend T min<T>(const SparseVector<T>& SV);
      friend T max<T>(const SparseVector<T>& SV);
      friend T minabs<T>(const SparseVector<T>& SV);
      friend T maxabs<T>(const SparseVector<T>& SV);
      // arithmetic
      friend SparseVector<T> operator-<T>(const SparseVector<T>& L,
                                          const SparseVector<T>& R);
      friend SparseVector<T> operator-<T>(const SparseVector<T>& L,
                                          const Vector<T>& R);
      friend SparseVector<T> operator-<T>(const Vector<T>& L,
                                          const SparseVector<T>& R);
      friend SparseVector<T> operator+<T>(const SparseVector<T>& L,
                                          const SparseVector<T>& R);
      friend SparseVector<T> operator+<T>(const SparseVector<T>& L,
                                          const Vector<T>& R);
      friend SparseVector<T> operator+<T>(const Vector<T>& L,
                                          const SparseVector<T>& R);
      // SparseMatrix
      friend SparseMatrix<T> transpose<T>(const SparseMatrix<T>& M);
      friend SparseMatrix<T> transform<T>(const SparseMatrix<T>& M,
                                          const SparseMatrix<T>& C) throw(Exception);
      friend SparseVector<T> operator*<T>(const SparseMatrix<T>& L,
                                          const SparseVector<T>& V);
      friend SparseVector<T> operator*<T>(const SparseMatrix<T>& L,
                                          const Vector<T>& V);
      friend SparseMatrix<T> operator*<T>(const SparseMatrix<T>& L,
                                          const SparseMatrix<T>& V);
      friend SparseMatrix<T> operator*<T>(const SparseMatrix<T>& L,
                                          const Matrix<T>& R);
      friend SparseMatrix<T> operator*<T>(const Matrix<T>& L,
                                          const SparseMatrix<T>& R);
      friend SparseMatrix<T> operator||<T>(const SparseMatrix<T>& L,
                                           const Vector<T>& V);
      friend SparseMatrix<T> operator||<T>(const SparseMatrix<T>& L,
                                           const SparseMatrix<T>& R);
      friend SparseMatrix<T> inverse<T>(const SparseMatrix<T>& A) throw(Exception);
      friend SparseMatrix<T> lowerCholesky<T>(const SparseMatrix<T>& A)
                                                    throw(Exception);
      friend SparseMatrix<T> inverseLT<T>(const SparseMatrix<T>& LT,
                             T *ptrSmall, T *ptrBig) throw(Exception);
      // special matrices
      friend SparseMatrix<T> identSparse<T>(const unsigned int dim) throw();

      // diag of P * C * PT
      friend Vector<T> transformDiag<T>(const SparseMatrix<T>& P,
                                        const Matrix<T>& C) throw(Exception);
      // Householder
      friend SparseMatrix<T> SparseHouseholder<T>(const SparseMatrix<T>& A)
                                                    throw(Exception);
      friend void SrifMU<T>(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& A,
                                  const unsigned int M) throw(Exception);
      friend void SrifMU<T>(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& P,
                            Vector<T>& D, const unsigned int M) throw(Exception);

      /// tolerance in considering element to be zero is std::abs(elem) < tolerance
      /// see zeroize(), where this is the default input value
      static const double zeroTolerance;

      /// empty constructor
      SparseVector(void) : len(0) { }

      /// constructor with length
      SparseVector(const unsigned int N) : len(N) { }

      /// constructor from regular Vector<T>
      SparseVector(const Vector<T>& V);

      /// subvector constructor
      /// @param SV SparseVector to copy
      /// @param ind starting index for the copy
      /// @param len length of new SparseVector
      SparseVector(const SparseVector<T>& SV,
                  const unsigned int& ind, const unsigned int& len);

      // TD watch for unintended consequences - cast to Vector to use some Vector::fun
      /// cast to Vector or implicit conversion to Matrix<T>
      operator Vector<T>() const;

      /// size - of the real Vector, not the data array
      inline unsigned int size(void) const { return len; }

      /// datasize - number of non-zero data
      inline unsigned int datasize(void) const { return vecMap.size(); }

      /// is this SV empty? NB may have to call zeroize() to get a yes.
      inline bool isEmpty(void) const
         { return (vecMap.begin() == vecMap.end()); }

      /// density - ratio of number of non-zero element to size()
      inline double density(void) const
         { return (double(vecMap.size())/double(len)); }

      /// truncate - removes elements, if necessary, at and beyond column index j
      /// same as resize(n) but does not change the nominal length len.
      inline void truncate(const unsigned int n)
      {
         if(n == 0)
            vecMap.clear();
         else if(n < len) {
            typename std::map<unsigned int, T>::iterator it;
            // lower_bound returns it for first key >= newlen
            it = vecMap.lower_bound(n);
            vecMap.erase(it,vecMap.end());
         }
      }

      /// resize - remove elements (truncate) and change nominal length len
      inline void resize(const unsigned int newlen)
      {
         truncate(newlen);
         len = newlen;
      }

      /// clear - set all data to 0 (i.e. remove all data); leave length alone
      inline void clear(void) { vecMap.clear(); }

      /// zeroize - remove elements that are less than or equal to tolerance
      /// in abs value. Called with a non-zero tolerance only by the user.
      /// NB this class and SparseMatrix call this when constructing a new object,
      /// e.g. after matrix multiply, but ONLY with the tolerance T(0).
      void zeroize(const T tol=static_cast<T>(zeroTolerance));

      /// true if the element is non-zero
      inline bool isFilled(const unsigned int i) const
         { return (vecMap.find(i) != vecMap.end()); }

      // operators ----------------------------------------------------
      /// operator[] for const, but SVecProxy does all the work
      const SVecProxy<T> operator[](unsigned int in) const
      {
      #ifdef RANGECHECK
         if(in >= len) GPSTK_THROW(Exception("index out of range"));
      #endif
         return SVecProxy<T>(const_cast<SparseVector&>(*this), in);
      }

      /// operator[] for non-const, but SVecProxy does all the work
      SVecProxy<T> operator[](unsigned int in)
      {
      #ifdef RANGECHECK
         if(in >= len) GPSTK_THROW(Exception("index out of range"));
      #endif
         return SVecProxy<T>(*this, in);
      }

      // output -------------------------------------------------------
      /// Dump only non-zero values, with indexes (i,value)
      std::string dump(const int p=3,bool dosci=false) const
      {
         std::ostringstream oss;
         size_t i;
         oss << "len=" << len << ", N=" << vecMap.size();
         oss << (dosci ? std::scientific : std::fixed) << std::setprecision(p);
         typename std::map<unsigned int, T>::const_iterator it = vecMap.begin();
         for( ; it != vecMap.end(); ++it)
            oss << " " << it->first << "," << it->second;   // << ")";
         return oss.str();
      }

      // operations ---------------------------------------------------
      /// Sum of elements
      inline T sum(const SparseVector<T>& SV) const
      {
         T tot(0);
         typename std::map<unsigned int, T>::iterator it = vecMap.begin();
         for( ; it != vecMap.end(); ++it)
            tot += it->second;
         return tot;
      }

      // arithmetic and other operators
      SparseVector<T>& operator-=(const SparseVector<T>& SV);
      SparseVector<T>& operator-=(const Vector<T>& SV);
      SparseVector<T>& operator+=(const SparseVector<T>& SV);
      SparseVector<T>& operator+=(const Vector<T>& SV);
      SparseVector<T>& operator*=(const T& value);
      SparseVector<T>& operator/=(const T& value);
      // special case for use with matrix inverse
      void addScaledSparseVector(const T& a, const SparseVector<T>& SV);

      // unary minus
      SparseVector<T> operator-() const
      {
         //std::cout << " SV unary minus with len " << len << std::endl;
         SparseVector<T> toRet(*this);
         typename std::map<unsigned int, T>::iterator it;
         for(it = toRet.vecMap.begin(); it != toRet.vecMap.end(); ++it) {
            toRet.vecMap[it->first] = -toRet.vecMap[it->first];
         }
         return toRet;
      }

   private:
      /// length of the "real" vector (not the number of data stored = vecMap.size())
      unsigned int len;

      /// map of index,value pairs; vecMap[index in real vector] = data element
      std::map<unsigned int, T> vecMap;

      /// return a vector containing all the indexes, in order, of non-zero elements.
      inline std::vector<unsigned int> getIndexes(void) const
      {
         std::vector<unsigned int> vecind;
         typename std::map<unsigned int, T>::const_iterator it;
         for(it = vecMap.begin(); it != vecMap.end(); ++it)
            vecind.push_back(it->first);
         return vecind;
      }

   }; // end class SparseVector


   //---------------------------------------------------------------------------
   // implementation of SVecProxy
   //---------------------------------------------------------------------------
   // Default constructor
   template <class T> SVecProxy<T>::SVecProxy(SparseVector<T>& sv, unsigned int i)
         : mySV(sv), index(i) { }

   //---------------------------------------------------------------------------
   // get the value of the SparseVector at index
   template <class T> T SVecProxy<T>::value(void) const
   {
      typename std::map<unsigned int, T>::iterator it = mySV.vecMap.find(index);
      if(it != mySV.vecMap.end())
         return it->second;
      return T(0);
   }

   //---------------------------------------------------------------------------
   // assignment, used by operator=, operator+=, etc
   template <class T> void SVecProxy<T>::assign(T rhs)
   {
      // zero or default - remove from map
      if(T(rhs) == T(0)) {
         typename std::map<unsigned int, T>::iterator it = mySV.vecMap.find(index);
         if(it != mySV.vecMap.end())
            mySV.vecMap.erase(it);
      }

      // add/replace it in the map
      else {
         (static_cast< std::map< unsigned int, T>& >(mySV.vecMap))[index] = rhs;
      }
   }

   //---------------------------------------------------------------------------
   // cast 
   template <class T> SVecProxy<T>::operator T() const
   {
      typename std::map<unsigned int, T>::iterator it = mySV.vecMap.find(index);
      if(it != mySV.vecMap.end())
         return (*it).second;
      else
         return T(0);
   }


   //---------------------------------------------------------------------------
   // implementation of SparseVector
   //---------------------------------------------------------------------------
   // constructor from regular Vector<T>
   template <class T> SparseVector<T>::SparseVector(const Vector<T>& V)
   {
      len = V.size();
      for(unsigned int i=0; i<len; i++) {
         if(V[i] == T(0)) continue;
         // non-zero, must add it
         vecMap[i] = V[i];
      }
   }

   // subvector constructor
   // @param SV SparseVector to copy
   // @param ind starting index for the copy
   // @param n length of new SparseVector
   template <class T> SparseVector<T>::SparseVector(const SparseVector<T>& SV,
                                 const unsigned int& ind, const unsigned int& n)
   {
      if(ind+n > SV.len)
         GPSTK_THROW(Exception("Invalid input subvector c'tor - out of range"));
      if(n == 0) return;

      len = n;
      typename std::map<unsigned int, T>::const_iterator it;
      for(it = SV.vecMap.begin(); it != SV.vecMap.end(); ++it) {
         if(it->first < ind) continue;       // skip ones before ind
         if(it->first > ind+n) break;
         vecMap[it->first-ind] = it->second;
      }
   }

   // cast to Vector<T>
   template <class T> SparseVector<T>::operator Vector<T>() const
   {
      Vector<T> toRet(len,T(0));
      typename std::map< unsigned int, T >::const_iterator it;
      for(it = vecMap.begin(); it != vecMap.end(); ++it) {
         toRet(it->first) = it->second;
      }

      return toRet;
   }

   // zeroize - remove elements that are less than or equal to tolerance in abs value
   // Called with a non-zero tolerance only by the user.
   // NB this class and SparseMatrix call this when constructing a new object,
   // e.g. after matrix multiply, but ONLY with the tolerance T(0).
   template <class T>
   void SparseVector<T>::zeroize(const T tol)
   {
      std::vector<unsigned int> toDelete;
      typename std::map<unsigned int, T>::iterator it;

      for(it = vecMap.begin(); it != vecMap.end(); ++it) {
         if(ABS(it->second) <= tol) 
            toDelete.push_back(it->first);
      }
      
      for(unsigned int i=0; i<toDelete.size(); i++)
         vecMap.erase(toDelete[i]);
   }

   // SparseVector stream output operator
   template <class T>
   std::ostream& operator<<(std::ostream& os, const SparseVector<T>& SV) 
   {
      std::ofstream savefmt;
      savefmt.copyfmt(os);

      unsigned int i;               // the "real" vector index
      typename std::map<unsigned int, T>::const_iterator it = SV.vecMap.begin();
      for(i=0; i<SV.len; i++) {
         if(i > 0) os << std::setw(1) << ' ';
         os.copyfmt(savefmt);
         if(it != SV.vecMap.end() && i == it->first) {
            os << it->second;
            ++it;
         }
         else
            os << "0";
      }
         
      return os;
   }

   //---------------------------------------------------------------------------
   // Norm = sqrt(sum(squares))
   template <class T> T norm(const SparseVector<T>& SV)
   {
      typename std::map<unsigned int, T>::const_iterator it = SV.vecMap.begin();
      if(it == SV.vecMap.end())
         return T(0);

      T tn(ABS(it->second));   // cmath/cstdlib makes this valid for std types
      for( ; it != SV.vecMap.end(); ++it) {
         if(tn > ABS(it->second))
            tn *= SQRT(T(1) + (it->second/tn)*(it->second/tn));
         else if(tn < ABS(it->second))
            tn = ABS(it->second) * SQRT(T(1)+(tn/it->second)*(tn/it->second));
         else
            tn *= SQRT(T(2));
      }
      return tn;
   }

   //---------------------------------------------------------------------------
   // cosine of angle between two vectors
   template <class T> T cosVec(const SparseVector<T>& S1, const SparseVector<T>& S2)
   {
      T cv;
      // try in case RANGECHECK is set
      try { cv = S1.dot(S2); }
      catch(Exception& e) { GPSTK_RETHROW(e); }

      T norm1(norm(S1)), norm2(norm(S2));
      if(norm1 == T(0) || norm2 == T(0))
         GPSTK_THROW(Exception("zero norm"));

      return ((cv/norm1)/norm2);
   }

   template <class T> T cosVec(const SparseVector<T>& SV, const Vector<T>& V)
   {
      T cv;
      // try in case RANGECHECK is set
      try { cv = SV.dot(V); }
      catch(Exception& e) { GPSTK_RETHROW(e); }

      T norm1(norm(SV)), norm2(norm(V));
      if(norm1 == T(0) || norm2 == T(0))
         GPSTK_THROW(Exception("zero norm"));

      return ((cv/norm1)/norm2);
   }

   template <class T> T cosVec(const Vector<T>& V, const SparseVector<T>& SV)
      { return cosVec(SV,V); }

   //---------------------------------------------------------------------------
   // dot products
   /// dot (SparseVector, SparseVector)
   template <class T> T dot(const SparseVector<T>& SL, const SparseVector<T>& SR)
   {
      if(SL.size() != SR.size()) GPSTK_THROW(Exception("length mismatch"));
      T value(0);
      typename std::map<unsigned int, T>::const_iterator it = SL.vecMap.begin();
      typename std::map<unsigned int, T>::const_iterator jt = SR.vecMap.begin();
      while(it != SL.vecMap.end() && jt != SR.vecMap.end()) {
         if(it->first > jt->first)
            ++jt;
         else if(jt->first > it->first)
            ++it;
         else {
            value += it->second * jt->second;
            ++it;
            ++jt;
         }
      }
      return value;
   }

   /// dot (SparseVector, SparseVector) but only use indexes k=kb, k<ke
   template <class T> T dot_lim(const SparseVector<T>& SL, const SparseVector<T>& SR,
                                const unsigned int kb, const unsigned int ke)
   {
      if(SL.size() != SR.size()) GPSTK_THROW(Exception("length mismatch"));
      T value(0);
      typename std::map<unsigned int, T>::const_iterator it = SL.vecMap.begin();
      typename std::map<unsigned int, T>::const_iterator jt = SR.vecMap.begin();
      while(it != SL.vecMap.end() && jt != SR.vecMap.end()) {
         if(it->first >= ke || jt->first >= ke) break;
         if(it->first > jt->first || jt->first < kb)
            ++jt;
         else if(jt->first > it->first || it->first < kb)
            ++it;
         else {      // equal indexes: it->first == jt->first
            if(it->first >= kb) value += it->second * jt->second;
            ++it;
            ++jt;
         }
      }
      return value;
   }

   /// dot (SparseVector, Vector)
   template <class T> T dot(const SparseVector<T>& SL, const Vector<T>& R)
   {
      if(SL.size() != R.size()) GPSTK_THROW(Exception("length mismatch"));
      T value(0);
      typename std::map<unsigned int, T>::const_iterator it;
      for(it = SL.vecMap.begin(); it != SL.vecMap.end(); ++it) {
         value += it->second * R[it->first];
      }
      return value;
   }

   /// dot (Vector, SparseVector)
   template <class T> T dot(const Vector<T>& L, const SparseVector<T>& SR)
   {
      return dot(SR,L);
   }

   template <class T> T min(const SparseVector<T>& SV)
   {
      typename std::map<unsigned int, T>::const_iterator it = SV.vecMap.begin();
      if(it == SV.vecMap.end()) return T(0);
      T value(it->second);
      for(++it; it != SV.vecMap.end(); ++it)
         if(it->second < value) value = it->second;
      return value;
   }

   template <class T> T max(const SparseVector<T>& SV)
   {
      typename std::map<unsigned int, T>::const_iterator it = SV.vecMap.begin();
      if(it == SV.vecMap.end()) return T(0);
      T value(it->second);
      for(++it; it != SV.vecMap.end(); ++it)
         if(it->second > value) value = it->second;
      return value;
   }

   template <class T> T minabs(const SparseVector<T>& SV)
   {
      typename std::map<unsigned int, T>::const_iterator it = SV.vecMap.begin();
      if(it == SV.vecMap.end()) return T(0);
      T value(ABS(it->second));
      for(++it; it != SV.vecMap.end(); ++it)
         if(ABS(it->second) < value) value = ABS(it->second);
      return value;
   }

   template <class T> T maxabs(const SparseVector<T>& SV)
   {
      typename std::map<unsigned int, T>::const_iterator it = SV.vecMap.begin();
      if(it == SV.vecMap.end()) return T(0);
      T value(ABS(it->second));
      for(++it; it != SV.vecMap.end(); ++it)
         if(ABS(it->second) > value) value = ABS(it->second);
      return value;
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   // addition and subtraction
   // member function operator-=(SparseVector)
   template <class T>
   SparseVector<T>& SparseVector<T>::operator-=(const SparseVector<T>& R)
   {
      if(len != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op-=(SV)"));

      typename std::map<unsigned int, T>::const_iterator Rit;
      for(Rit = R.vecMap.begin(); Rit != R.vecMap.end(); ++Rit) {
         if(vecMap.find(Rit->first) == vecMap.end())
            vecMap[Rit->first] = -Rit->second;
         else
            vecMap[Rit->first] -= Rit->second;
      }
      zeroize(T(0));
 
      return *this;
   }

   // member function operator-=(Vector)
   template <class T>
   SparseVector<T>& SparseVector<T>::operator-=(const Vector<T>& R)
   {
      if(len != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op-=(V)"));

      for(unsigned int i=0; i<R.size(); i++) {
         if(R[i] == T(0)) continue;

         if(vecMap.find(i) == vecMap.end())
            vecMap[i] = -R[i];
         else
            vecMap[i] -= R[i];
      }
      zeroize(T(0));

      return *this;
   }

   // member function operator+=(SparseVector)
   template <class T>
   SparseVector<T>& SparseVector<T>::operator+=(const SparseVector<T>& R)
   {
      if(len != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op+=(SV)"));

      typename std::map<unsigned int, T>::const_iterator Rit;
      for(Rit = R.vecMap.begin(); Rit != R.vecMap.end(); ++Rit) {
         if(vecMap.find(Rit->first) == vecMap.end())
            vecMap[Rit->first] = Rit->second;
         else
            vecMap[Rit->first] += Rit->second;
      }
      zeroize(T(0));

      return *this;
   }

   // member function operator+=(Vector)
   template <class T>
   SparseVector<T>& SparseVector<T>::operator+=(const Vector<T>& R)
   {
      if(len != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op+=(V)"));
      //std::cout << " op+=(V)" << std::endl;

      for(unsigned int i=0; i<R.size(); i++) {
         if(R[i] == T(0)) continue;

         if(vecMap.find(i) == vecMap.end())
            vecMap[i] = R[i];
         else
            vecMap[i] += R[i];
      }
      zeroize(T(0));

      return *this;
   }

   // member function ~ op+=(a*SV)
   template <class T>
   void SparseVector<T>::addScaledSparseVector(const T& a, const SparseVector<T>& R)
   {
      if(a == T(0)) return;
      if(len != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions addScaledSparseVector()"));

      for(unsigned int i=0; i<R.size(); i++) {
         if(R[i] == T(0)) continue;

         if(vecMap.find(i) == vecMap.end())
            vecMap[i] = a*R[i];
         else
            vecMap[i] += a*R[i];
      }
      zeroize(T(0));
   }

   // member function operator*=(scalar)
   template <class T>
   SparseVector<T>& SparseVector<T>::operator*=(const T& value)
   {
      if(value == T(0)) {
         resize(0);
      }
      else {
         typename std::map<unsigned int, T>::iterator it;
         for(it=vecMap.begin(); it != vecMap.end(); ++it) {
            it->second *= value;
         }
      }

      return *this;
   }

   // member function operator/=(scalar)
   template <class T>
   SparseVector<T>& SparseVector<T>::operator/=(const T& value)
   {
      if(value == T(0)) GPSTK_THROW(Exception("Divide by zero"));

      typename std::map<unsigned int, T>::iterator it;
      for(it=vecMap.begin(); it != vecMap.end(); ++it) {
         it->second /= value;
      }

      return *this;
   }

   // SparseVector = SparseVector - SparseVector
   template <class T> SparseVector<T> operator-(const SparseVector<T>& L,
                                                const SparseVector<T>& R)
   {
      if(L.size() != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op-(SV,SV)"));

      //std::cout << "Call copy ctor from op-(SV,SV)" << std::endl;
      SparseVector<T> retSV(L);
      retSV -= R;

      return retSV;
   }

   // SparseVector = SparseVector - Vector
   template <class T> SparseVector<T> operator-(const SparseVector<T>& L,
                                                const Vector<T>& R)
   {
      if(L.size() != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op-(SV,V)"));
      //std::cout << "Call copy ctor from op-(SV,V)" << std::endl;

      SparseVector<T> retSV(L);
      retSV -= R;

      return retSV;
   }

   // SparseVector = Vector - SparseVector
   template <class T> SparseVector<T> operator-(const Vector<T>& L,
                                                const SparseVector<T>& R)
   {
      if(L.size() != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op-(V,SV)"));
      //std::cout << "Call copy ctor from op-(V,SV)" << std::endl;

      SparseVector<T> retSV(R);
      retSV = -retSV;
      retSV += L;

      return retSV;
   }

   // SparseVector = SparseVector + SparseVector
   template <class T> SparseVector<T> operator+(const SparseVector<T>& L,
                                                const SparseVector<T>& R)
   {
      if(L.size() != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op+(SV,SV)"));
      //std::cout << "Call copy ctor from op+(SV,SV)" << std::endl;

      SparseVector<T> retSV(R);
      retSV += L;

      return retSV;
   }

   // SparseVector = SparseVector + Vector
   template <class T> SparseVector<T> operator+(const SparseVector<T>& L,
                                                const Vector<T>& R)
   {
      if(L.size() != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op+(SV,V)"));
      //std::cout << "Call copy ctor from op+(SV,V)" << std::endl;

      SparseVector<T> retSV(L);
      retSV += R;

      return retSV;
   }

   // SparseVector = Vector + SparseVector
   template <class T> SparseVector<T> operator+(const Vector<T>& L,
                                                const SparseVector<T>& R)
   {
      if(L.size() != R.size())
         GPSTK_THROW(Exception("Incompatible dimensions op+(V,SV)"));
      //std::cout << "Call copy ctor from op+(V,SV)" << std::endl;

      SparseVector<T> retSV(R);
      retSV += L;

      return retSV;
   }

}  // namespace

#endif   // define SPARSE_VECTOR_INCLUDE
