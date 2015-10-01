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

/// @file SparseMatrix.hpp  Class for template sparse matrices; use with SparseVector.

#ifndef SPARSE_MATRIX_INCLUDE
#define SPARSE_MATRIX_INCLUDE

#include <map>
#include <string>
#include <algorithm>          // for find,lower_bound

#include "SparseVector.hpp"
#include "Matrix.hpp"

// TODO
// >trace
// >add concatenations {&& ||}{SM SV M V} to SparseMatrix
// >add minabs maxabs to Matrix
// notes
// >potential trap in that rowsMap[.].len may not agree with ncols...
// >do we want a cast-to-Matrix function? Or do we want to replace with explicit func?
// >watch out for implicit casts to Matrix - this could really confuse things.
//  compiler could silently cast to Matrix to access some Matrix::func not found here
//  ?create Matrix from SparseMatrix using an explicit function rather than cast?
// >can you define SV<double>::zeroTolerance = 1.e-14; SV<int>::zeroTolerance = 1;
// >document
//
// v >inverse,LUD,SVD,HH - only inverse Cholesky for now...cast to Matrix?
//     costly to implement, how much benefit?  HH is used in SRIF
//     not able to speed up LUD and SVD - cast to Matrix. Chol, HH< SRIFMU done.
// v >go through all the code and avoid using proxys; access maps directly; SV friend
// v >is rowsMap.insert(pair(j,SparseVector) faster than rowsMap[j] = SV ?
//     stackoverflow says no
// v >worry about SparseVector::zeroTolerance - make it set-able? No, pass it
// v > To zeroize or not? that is the question. If you zeroize inside routines here,
//   there is the risk that you'll ruin the content of your data. For example, a
//   m. cov. matrix where you set one value very small, to effectively constrain.
//   If you zeroize in, say, lowerCholesky, you may very well set it to zero which
//   would make the matrix singular.
// *** Zeroize ONLY when the user explicitly calls zeroize(), and let him pass the
//  tolerance, except call zeroize(T(0)) after operations that might create zeros.
//  Even better, ** never add zeros to the map **

namespace gpstk
{
   // forward declarations
   template <class T> class SparseMatrix;

   //---------------------------------------------------------------------------
   /// Proxy class for elements of the SparseMatrix (SM).
   template <class T> class SMatProxy
   {
   public:
      /// constructor
      SMatProxy(SparseMatrix<T>& SM, unsigned int i, unsigned int j);

      /// operator = for non-const (lvalue)
      SMatProxy& operator=(const SMatProxy<T>& rhs)
         { assign(rhs); return *this; }
      /// operator = for const (rvalue)
      SMatProxy& operator=(T rhs)
         { assign(rhs); return *this; }

      /// cast or implicit conversion
      operator T() const;

      /// operator+= for non-const (lvalue)
      SMatProxy& operator+=(const SMatProxy<T>& rhs)
         { assign(value()+rhs); return *this; }

      /// operator+= for const (rvalue)
      SMatProxy& operator+=(T rhs)
         { assign(value()+rhs); return *this; }

      /// operator-= for non-const (lvalue)
      SMatProxy& operator-=(const SMatProxy<T>& rhs)
         { assign(value()-rhs); return *this; }

      /// operator-= for const (rvalue)
      SMatProxy& operator-=(T rhs)
         { assign(value()-rhs); return *this; }

      /// operator*= for non-const (lvalue)
      SMatProxy& operator*=(const SMatProxy<T>& rhs)
         { assign(value()*rhs); return *this; }

      /// operator*= for const (rvalue)
      SMatProxy& operator*=(T rhs)
         { assign(value()*rhs); return *this; }

   private:
      /// reference to the matrix to which this data belongs
      SparseMatrix<T>& mySM;

      /// indexes in mySM for this data
      unsigned int irow, jcol;

      /// get the value of the SparseMatrix at irow,jcol
      T value(void) const;

      /// assign the SparseMatrix element, used by operator=,+=,etc
      void assign(T rhs);

   }; // end class SMatProxy

   //---------------------------------------------------------------------------
   // implementation of SMatProxy
   //---------------------------------------------------------------------------
   // Default constructor
   template <class T>
   SMatProxy<T>::SMatProxy(SparseMatrix<T>& sm, unsigned int i, unsigned int j)
         : mySM(sm), irow(i), jcol(j) { }

   //---------------------------------------------------------------------------
   // get the value of the SparseMatrix at irow, jcol
   template <class T> T SMatProxy<T>::value(void) const
   {
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      it = mySM.rowsMap.find(irow);
      if(it != mySM.rowsMap.end())
         return it->second[jcol];

      return T();
   }

   //---------------------------------------------------------------------------
   // assignment, used by operator=, operator+=, etc
   template <class T> void SMatProxy<T>::assign(T rhs)
   {
      typename std::map< unsigned int, SparseVector<T> >::iterator it;
      it = mySM.rowsMap.find(irow);

      // add a new row? only if row is not there, and rhs is not zero
      if(T(rhs) != T(0) && it == mySM.rowsMap.end()) {
         SparseVector<T> SVrow(mySM.ncols);
         mySM.rowsMap[irow] = SVrow;
         it = mySM.rowsMap.find(irow);
      }
      // data is zero and row is not there...nothing to do
      else if(it == mySM.rowsMap.end()) return;

      // assign it - let SparseVector::SMatProxy handle r/lvalue
      // first, do we need to resize the SV?
      if(it->second.size() < jcol+1) it->second.resize(jcol+1);
      it->second[jcol] = rhs;

      // if row is now empty, remove it
      if(it->second.datasize() == 0)
         mySM.rowsMap.erase(it);
   }

   //---------------------------------------------------------------------------
   // cast 
   template <class T> SMatProxy<T>::operator T() const
   {
      typename std::map< unsigned int, SparseVector<T> >::iterator it;
      it = mySM.rowsMap.find(irow);
      if(it == mySM.rowsMap.end())
         return T();
      else
         return it->second[jcol];
   }


   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   // friends of class SparseMatrix - must declare friends before the template class
   // min max
   template <class T> T min(const SparseMatrix<T>& SM);
   template <class T> T max(const SparseMatrix<T>& SM);
   template <class T> T minabs(const SparseMatrix<T>& SM);
   template <class T> T maxabs(const SparseMatrix<T>& SM);
   // output
   template <class T>
      std::ostream& operator<<(std::ostream& os, const SparseMatrix<T>& SM);
   // transpose
   template <class T>
      SparseMatrix<T> transpose(const SparseMatrix<T>& M);
   // multiplies
   template <class T>
      SparseMatrix<T> operator*(const SparseMatrix<T>& L, const SparseMatrix<T>& R);
   template <class T>
      SparseVector<T> operator*(const SparseMatrix<T>& L, const SparseVector<T>& V);
   template <class T>
      SparseVector<T> operator*(const Matrix<T>& L, const SparseVector<T>& V);
   template <class T>
      SparseVector<T> operator*(const SparseMatrix<T>& L, const Vector<T>& V);
   template <class T>
      SparseVector<T> operator*(const SparseVector<T>& V, const SparseMatrix<T>& R);
   template <class T>
      SparseVector<T> operator*(const SparseVector<T>& V, const Matrix<T>& R);
   template <class T>
      SparseVector<T> operator*(const Vector<T>& V, const SparseMatrix<T>& R);
   template <class T>
      SparseMatrix<T> operator*(const SparseMatrix<T>& L, const SparseMatrix<T>& R);
   template <class T>
      SparseMatrix<T> operator*(const SparseMatrix<T>& L, const Matrix<T>& R);
   template <class T>
      SparseMatrix<T> operator*(const Matrix<T>& L, const SparseMatrix<T>& R);
   // concatenation
   template <class T>
      SparseMatrix<T> operator||(const SparseMatrix<T>& L, const Vector<T>& V);
   template <class T>
      SparseMatrix<T> operator||(const SparseMatrix<T>& L, const SparseMatrix<T>& R);
   // addition and subtraction
   template <class T> SparseMatrix<T> operator-(const SparseMatrix<T>& L,
                                                const SparseMatrix<T>& R);
   template <class T> SparseMatrix<T> operator-(const SparseMatrix<T>& L,
                                                const Matrix<T>& R);
   template <class T> SparseMatrix<T> operator-(const Matrix<T>& L,
                                                const SparseMatrix<T>& R);
   template <class T> SparseMatrix<T> operator+(const SparseMatrix<T>& L,
                                                const SparseMatrix<T>& R);
   template <class T> SparseMatrix<T> operator+(const SparseMatrix<T>& L,
                                                const Matrix<T>& R);
   template <class T> SparseMatrix<T> operator+(const Matrix<T>& L,
                                                const SparseMatrix<T>& R);
   // inverse (via Gauss-Jordan)
   template <class T>
   SparseMatrix<T> inverse(const SparseMatrix<T>& A) throw(Exception);
   // Cholesky
   template <class T>
   SparseMatrix<T> lowerCholesky(const SparseMatrix<T>& A) throw(Exception);
   template <class T>
   SparseMatrix<T> upperCholesky(const SparseMatrix<T>& A) throw(Exception);
   // inverseLT
   template <class T>
   SparseMatrix<T> inverseLT(const SparseMatrix<T>& LT, T *ptrSmall, T *ptrBig)
                                                                throw(Exception);
   template <class T>
   SparseMatrix<T> inverseViaCholesky(const SparseMatrix<T>& A) throw(Exception);

   // special matrices
   template <class T>
   SparseMatrix<T> identSparse(const unsigned int dim) throw();

   // products MT*M, M*MT, M*C*MT etc
   // MT * M
   template <class T>
   SparseMatrix<T> transposeTimesMatrix(const SparseMatrix<T>& M) throw(Exception);
   // M * MT
   template <class T>
   SparseMatrix<T> matrixTimesTranspose(const SparseMatrix<T>& M) throw(Exception);
   // diag of P * C * PT
   template <class T> Vector<T> transformDiag(const SparseMatrix<T>& P,
                                              const Matrix<T>& C) throw(Exception);

   // Householder
   template <class T>
   SparseMatrix<T> SparseHouseholder(const SparseMatrix<T>& A) throw(Exception);

   template <class T> void SrifMU(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& A,
                                  const unsigned int M) throw(Exception);
   template <class T> void SrifMU(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& P,
                             Vector<T>& D, const unsigned int M) throw(Exception);

   //---------------------------------------------------------------------------
   /// Class SparseMatrix. This class is designed to present an interface nearly
   /// identical to class Matrix, but more efficiently handle sparse matrices, in
   /// which most of the elements are zero. The class stores only non-zero elements;
   /// using a map of SparseVectors, with key = row index. it also stores a nominal
   /// dimension - number of rows and columns. The class uses a proxy class,
   /// SMatProxy, to access elements; this allows rvalues and lvalues to be treated
   /// separately.
   /// Notes on speed. The most expensive parts are the Proxy::operator(), then
   /// find() and lower_bound(). Never use the Proxy stuff within the class, always
   /// use iterators and assign values to the maps directly. Never assign zeros to
   /// the maps. See timing and test results in the test program smtest.cpp
   /// Matrix multiply is orders of magnitude faster. Transpose() is much faster
   /// than the Matrix version, which is something of a surprise.
   /// Most time consuming is looping over columns; this is expected since the design
   /// stores by row. The trick is to re-write the algorithm in terms of the transpose
   /// of the column-loop matrix, and then apply a transpose(), which is cheap, either
   /// before starting (when col-loop matrix is input) or after returning (output).
   /// Then the loops become loops over rows.
   /// NB. never store zeros in the map, particularly when you are creating the
   /// matrix and using it at the same time, as in inverseLT().
   template <class T> class SparseMatrix
   {
   public:
      // lots of friends
      /// Proxy needs access to rowsMap
      friend class SMatProxy<T>;
      // min max
      friend T min<T>(const SparseMatrix<T>& SM);
      friend T max<T>(const SparseMatrix<T>& SM);
      friend T minabs<T>(const SparseMatrix<T>& SM);
      friend T maxabs<T>(const SparseMatrix<T>& SM);
      // stream operator
      friend std::ostream& operator<< <T>(std::ostream& os, const SparseMatrix<T>& S);
      // transpose
      friend SparseMatrix<T> transpose<T>(const SparseMatrix<T>& M);
      // arithmetic
      friend SparseMatrix<T> operator-<T>(const SparseMatrix<T>& L,
                                          const SparseMatrix<T>& R);
      friend SparseMatrix<T> operator-<T>(const SparseMatrix<T>& L,
                                          const Matrix<T>& R);
      friend SparseMatrix<T> operator-<T>(const Matrix<T>& L,
                                          const SparseMatrix<T>& R);
      friend SparseMatrix<T> operator+<T>(const SparseMatrix<T>& L,
                                          const SparseMatrix<T>& R);
      friend SparseMatrix<T> operator+<T>(const SparseMatrix<T>& L,
                                          const Matrix<T>& R);
      friend SparseMatrix<T> operator+<T>(const Matrix<T>& L,
                                          const SparseMatrix<T>& R);
      // mulitply
      friend SparseMatrix<T> operator*<T>(const SparseMatrix<T>& L,
                                          const SparseMatrix<T>& R);
      friend SparseVector<T> operator*<T>(const SparseMatrix<T>& L,
                                          const SparseVector<T>& V);
      friend SparseVector<T> operator*<T>(const Matrix<T>& L,
                                          const SparseVector<T>& V);
      friend SparseVector<T> operator*<T>(const SparseMatrix<T>& L,
                                          const Vector<T>& V);
      friend SparseVector<T> operator*<T>(const SparseVector<T>& V,
                                          const SparseMatrix<T>& R);
      friend SparseVector<T> operator*<T>(const SparseVector<T>& V,
                                          const Matrix<T>& R);
      friend SparseVector<T> operator*<T>(const Vector<T>& V,
                                          const SparseMatrix<T>& R);
      friend SparseMatrix<T> operator*<T>(const SparseMatrix<T>& L,
                                          const SparseMatrix<T>& R);
      friend SparseMatrix<T> operator*<T>(const SparseMatrix<T>& L,
                                          const Matrix<T>& R);
      friend SparseMatrix<T> operator*<T>(const Matrix<T>& L,
                                          const SparseMatrix<T>& R);
      // concatenation
      friend SparseMatrix<T> operator||<T>(const SparseMatrix<T>& L,
                                           const Vector<T>& V);
      friend SparseMatrix<T> operator||<T>(const SparseMatrix<T>& L,
                                           const SparseMatrix<T>& R);
      // inverse (via Gauss-Jordan)
      friend SparseMatrix<T> inverse<T>(const SparseMatrix<T>& A) throw(Exception);
      // Cholesky
      friend SparseMatrix<T> lowerCholesky<T>(const SparseMatrix<T>& A)
                                                         throw(Exception);
      friend SparseMatrix<T> upperCholesky<T>(const SparseMatrix<T>& A)
                                                         throw(Exception);
      friend SparseMatrix<T> inverseLT<T>(const SparseMatrix<T>& LT,
                                          T *ptrSmall, T *ptrBig) throw(Exception);
      friend SparseMatrix<T> inverseViaCholesky<T>(const SparseMatrix<T>& A)
                                                          throw(Exception);
      // special matrices
      friend SparseMatrix<T> identSparse<T>(const unsigned int dim) throw();

      // MT * M
      friend SparseMatrix<T> transposeTimesMatrix<T>(const SparseMatrix<T>& M)
                                                          throw(Exception);
      // M * MT
      friend SparseMatrix<T> matrixTimesTranspose<T>(const SparseMatrix<T>& M)
                                                          throw(Exception);
      // diag of P * C * PT
      friend Vector<T> transformDiag<T>(const SparseMatrix<T>& P,
                                        const Matrix<T>& C) throw(Exception);
      friend SparseMatrix<T> SparseHouseholder<T>(const SparseMatrix<T>& A)
                                                          throw(Exception);
      friend void SrifMU<T>(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& A,
                                    const unsigned int M) throw(Exception);
      friend void SrifMU<T>(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& P,
                            Vector<T>& D, const unsigned int M) throw(Exception);

      /// empty constructor
      SparseMatrix(void) : nrows(0), ncols(0) { }

      /// constructor with dimensions
      SparseMatrix(unsigned int r, unsigned int c) : nrows(r), ncols(c) { }

      /// sub-matrix constructor
      /// @param SV SparseVector to copy
      /// @param ind starting index for the copy
      /// @param n length of new SparseVector
      SparseMatrix(const SparseMatrix<T>& SM,
                  const unsigned int& rind, const unsigned int& cind,
                  const unsigned int& rnum, const unsigned int& cnum);

      /// constructor from regular Matrix<T>
      SparseMatrix(const Matrix<T>& M);

      // TD watch for unintended consequences - cast to Matrix to use some Matrix::fun
      /// cast to Matrix or implicit conversion to Matrix<T>
      operator Matrix<T>() const;

      /// get number of rows - of the real Matrix, not the data array
      inline unsigned int rows(void) const { return nrows; }

      /// get number of columns - of the real Matrix, not the data array
      inline unsigned int cols(void) const { return ncols; }

      /// size of matrix = rows()*cols()
      inline unsigned int size(void) const { return nrows*ncols; }

      /// datasize - number of non-zero data
      inline unsigned int datasize(void) const
      {
         unsigned int n(0);
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
         it = rowsMap.begin();
         for( ; it != rowsMap.end(); ++it)
            n += it->second.datasize();
         return n;
      }

      /// is this SM empty? NB may have to call zeroize() to get a yes.
      inline bool isEmpty(void) const
         { return (rowsMap.begin() == rowsMap.end()); }

      /// density - ratio of number of non-zero element to size()
      inline double density(void) const
         { return (double(datasize())/double(size())); }

      /// resize - only changes len and removes elements if necessary
      void resize(const unsigned int newrows, const unsigned int newcols)
      {
         typename std::map< unsigned int, SparseVector<T> >::iterator it;
         if(newrows < nrows) {
            // lower_bound returns it for first key >= newrows
            it = rowsMap.lower_bound(newrows);
            rowsMap.erase(it,rowsMap.end());
         }
         if(newcols < ncols)
            for(it=rowsMap.begin(); it != rowsMap.end(); ++it)
               it->second.resize(newcols);
         
         nrows = newrows;
         ncols = newcols;
      }

      /// clear - set all data to 0 (i.e. remove all data); leave dimensions alone
      inline void clear(void)
         { rowsMap.clear(); }

      /// zeroize - remove elements that are less than tolerance in abs value
      /// NB. This routine is called only by the user - routines defined here do not
      /// zeroize, as there is no way to appropriately choose a tolerance.
      /// The default tolerance for this routine is SparseVector<T>::zeroTolerance.
      /// The caller may want to consider a tolerance related to SM.maxabs().
      void zeroize(const T tol=static_cast<T>(SparseVector<T>::zeroTolerance));

      /// true if the element (i,j) is non-zero
      inline bool isFilled(const unsigned int i, const unsigned int j)
      {
         typename std::map< unsigned int, SparseVector<T> >::iterator it;
         it = rowsMap.find(i);
         return (it != rowsMap.end() && it->second.isFilled(j));
      }

      // operators ----------------------------------------------------
      /// operator() for const, but SMatProxy does all the work
      const SMatProxy<T> operator()(unsigned int i, unsigned int j) const
      {
      #ifdef RANGECHECK
         if(i >= nrows) GPSTK_THROW(Exception("row index out of range"));
         if(j >= ncols) GPSTK_THROW(Exception("col index out of range"));
      #endif
         return SMatProxy<T>(const_cast<SparseMatrix&>(*this), i, j);
      }

      /// operator() for non-const, but SMatProxy does all the work
      SMatProxy<T> operator()(unsigned int i, unsigned int j)
      {
      #ifdef RANGECHECK
         if(i >= nrows) GPSTK_THROW(Exception("row index out of range"));
         if(j >= ncols) GPSTK_THROW(Exception("col index out of range"));
      #endif
         return SMatProxy<T>(*this, i, j);
      }

      // output -------------------------------------------------------
      /// Dump only non-zero values, with indexes (i,value)
      std::string dump(const int p=3, bool dosci=false) const
      {
         std::ostringstream oss;
         size_t i;
         oss << "dim(" << nrows << "," << ncols << "), size " << size()
            << ", datasize " << datasize() << " :";
         oss << (dosci ? std::scientific : std::fixed) << std::setprecision(p);

         // loop over rows
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
         it = rowsMap.begin();
         if(it == rowsMap.end()) { oss << " empty"; return oss.str(); }

         for( ; it != rowsMap.end(); ++it) {
            oss << "\n row " << it->first << ": " << it->second.dump(p,dosci);
         }

         return oss.str();
      }

      /// Convert to "dump-able" form : 3 parallel vectors; rows, cols, values
      void flatten(std::vector<unsigned int>& rows,
                   std::vector<unsigned int>& cols,
                   std::vector<T>& values)
         const
      {
         rows.clear(); cols.clear(); values.clear();
         
         // loop over rows, then columns
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
         typename std::map<unsigned int, T>::const_iterator jt;
         for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {
            unsigned int row(it->first);
            for(jt = it->second.vecMap.begin(); jt != it->second.vecMap.end(); ++jt) {
               rows.push_back(row);
               cols.push_back(jt->first);
               values.push_back(jt->second);
            }
         }
      }

      /// Minimum element - return 0 if empty
      // arithmetic and other operators
      SparseMatrix<T>& operator-=(const SparseMatrix<T>& SM);
      SparseMatrix<T>& operator-=(const Matrix<T>& SM);
      SparseMatrix<T>& operator+=(const SparseMatrix<T>& SM);
      SparseMatrix<T>& operator+=(const Matrix<T>& SM);
      SparseMatrix<T>& operator*=(const T& value);
      SparseMatrix<T>& operator/=(const T& value) throw(Exception);

      // unary minus
      SparseMatrix<T> operator-() const
      {
         SparseMatrix<T> toRet(*this);
         typename std::map< unsigned int, SparseVector<T> >::iterator it;
         typename std::map<unsigned int, T>::iterator jt;
         for(it = toRet.rowsMap.begin(); it != toRet.rowsMap.end(); ++it) {
            for(jt = it->second.vecMap.begin(); jt != it->second.vecMap.end(); ++jt)
               jt->second = -jt->second;
         }
         return toRet;
      }

      /// return row i of this SparseMatrix as a SparseVector
      SparseVector<T> rowCopy(const unsigned int i) const;

      /// return col j of this SparseMatrix as a SparseVector
      SparseVector<T> colCopy(const unsigned int j) const;

      /// return diagonal of this SparseMatrix as a SparseVector
      SparseVector<T> diagCopy(void) const;

      /// swap rows of this SparseMatrix
      void swapRows(const unsigned int& ii, const unsigned int& jj);

      /// swap columns of this SparseMatrix
      void swapCols(const unsigned int ii, const unsigned int jj);

   private:
      /// dimensions of the "real" matrix (not the number of data stored)
      unsigned int nrows, ncols;

      /// map of row index, row SparseVector
      std::map< unsigned int, SparseVector<T> > rowsMap;

      // TD ? obsolete this by always using transpose when you must loop over columns
      /// private function to build a "column map" for this matrix,
      /// containing vectors (of row-indexes) for each column.
      /// colMap[column index] = vector of all row indexes, in ascending order
      std::map< unsigned int, std::vector<unsigned int> > columnMap(void) const
      {
         unsigned int j,k;
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
         typename std::map< unsigned int, std::vector<unsigned int> > colMap;
         typename std::map< unsigned int, std::vector<unsigned int> >::iterator jt;

         // loop over rows, then columns
         for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {

            // get all the column indexes for this row
            std::vector<unsigned int> colIndexes = it->second.getIndexes();

            // loop over columns, adding each to the colMap
            for(k=0; k<colIndexes.size(); k++) {
               j = colIndexes[k];
               jt = colMap.find(j);
               if(jt == colMap.end()) {            // add a vector for this column
                  std::vector<unsigned int> jvec;
                  colMap[j] = jvec;
                  jt = colMap.find(j);
               }
               jt->second.push_back(it->first);    // add row index to this col-vector
            }
         }

         //std::ostringstream oss;
         //for(jt=colMap.begin(); jt!=colMap.end(); ++jt) {
         //   oss << " col " << jt->first << " [";
         //   for(k=0; k<jt->second.size(); k++)
         //      oss << " " << jt->second[k];
         //   oss << "]\n";
         //}
         //std::cout << "Column map:\n" << oss.str();

         return colMap;
      }

   }; // end class SparseMatrix


   //---------------------------------------------------------------------------
   // implementation of SparseMatrix
   //---------------------------------------------------------------------------
   // submatrix constructor
   template <class T> SparseMatrix<T>::SparseMatrix(const SparseMatrix<T>& SM,
                           const unsigned int& rind, const unsigned int& cind,
                           const unsigned int& rnum, const unsigned int& cnum)
   {
      if(rnum == 0 || cnum == 0 || rind+rnum > SM.nrows || cind+cnum > SM.ncols)
         GPSTK_THROW(Exception("Invalid input submatrix c'tor - out of range"));

      nrows = rnum;
      ncols = cnum;
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      for(it = SM.rowsMap.begin(); it != SM.rowsMap.end(); ++it) {
         if(it->first < rind) continue;               // skip rows before rind
         if(it->first > rind+rnum) break;             // done with rows
         SparseVector<T> SV(it->second,cind,cnum);    // get sub-vector
         if(!SV.isEmpty()) rowsMap[it->first] = SV;   // add it
      }
   }

   // constructor from regular Matrix<T>
   template <class T> SparseMatrix<T>::SparseMatrix(const Matrix<T>& M)
   {
      unsigned i,j;
      nrows = M.rows();
      ncols = M.cols();
      for(i=0; i<nrows; i++) {
         bool haverow(false);                // rather than rowsMap.find() every time
         for(j=0; j<ncols; j++) {
            if(M(i,j) == T(0)) continue;     // nothing to do - element(i,j) is zero

            // non-zero, must add it
            if(!haverow) {                   // add row i
               SparseVector<T> SVrow(ncols); // 'real' length ncols
               rowsMap[i] = SVrow;
               haverow = true;
            }
            rowsMap[i].vecMap[j] = M(i,j);
         }
      }
   }

   /// cast to Matrix<T>
   template <class T> SparseMatrix<T>::operator Matrix<T>() const
   {
      Matrix<T> toRet(nrows,ncols,T(0));
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      typename std::map< unsigned int, T >::const_iterator jt;
      for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {
         for(jt = it->second.vecMap.begin(); jt != it->second.vecMap.end(); ++jt) {
            toRet(it->first,jt->first) = jt->second;
         }
      }

      return toRet;
   }

   /// zeroize - remove elements that are less than tolerance in abs value
   template <class T> void SparseMatrix<T>::zeroize(const T tol)
   {
      std::vector<unsigned int> toDelete;    // row indexes
      typename std::map< unsigned int, SparseVector<T> >::iterator it;
      for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {
         it->second.zeroize(tol);

         // remove row if its empty - but not inside the iteration loop
         if(it->second.datasize() == 0)
            toDelete.push_back(it->first);
      }

      // now remove the marked rows
      for(unsigned int i=0; i<toDelete.size(); i++) {
         rowsMap.erase(toDelete[i]);
      }
   }

   /// transpose
   template <class T> SparseMatrix<T> transpose(const SparseMatrix<T>& M)
   {
      SparseMatrix<T> toRet(M.cols(),M.rows());

      // loop over rows of M = columns of toRet - faster
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      typename std::map< unsigned int, SparseVector<T> >::iterator jt;
      for(it = M.rowsMap.begin(); it != M.rowsMap.end(); ++it) {
         for(unsigned int j=0; j<M.cols(); j++) {
            if(it->second.isFilled(j)) {           // M(i,j)
               jt = toRet.rowsMap.find(j);
               if(jt == toRet.rowsMap.end()) {     // add the row
                  SparseVector<T> rowSV(M.rows());
                  toRet.rowsMap[j] = rowSV;
                  jt = toRet.rowsMap.find(j);
               }
               jt->second.vecMap[it->first] = it->second[j];
            }
         }
      }

      return toRet;
   }

   /// Maximum element - return 0 if empty
   template <class T> T min(const SparseMatrix<T>& SM)
   {
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      it = SM.rowsMap.begin();
      if(it == SM.rowsMap.end()) return T(0);

      T value(min(it->second));                    // first non-zero row
      for(++it ; it != SM.rowsMap.end(); ++it ) {  // other rows
         T rowval(min(it->second));
         if(rowval < value) value = rowval;
      }

      return value;
   }

   /// Maximum element - return 0 if empty
   template <class T> T max(const SparseMatrix<T>& SM)
   {
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      it = SM.rowsMap.begin();
      if(it == SM.rowsMap.end()) return T(0);

      T value(max(it->second));                    // first non-zero row
      for(++it ; it != SM.rowsMap.end(); ++it ) {  // other rows
         T rowval(max(it->second));
         if(rowval > value) value = rowval;
      }

      return value;
   }

   /// Minimum absolute value - return 0 if empty
   template <class T> T minabs(const SparseMatrix<T>& SM)
   {
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      it = SM.rowsMap.begin();
      if(it == SM.rowsMap.end()) return T(0);

      T value(minabs(it->second));                 // first non-zero row
      for(++it ; it != SM.rowsMap.end(); ++it ) {  // other rows
         T rowval(minabs(it->second));
         if(rowval < value) value = rowval;
      }

      return value;
   }

   /// Maximum absolute value - return 0 if empty
   template <class T> T maxabs(const SparseMatrix<T>& SM)
   {
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      it = SM.rowsMap.begin();
      if(it == SM.rowsMap.end()) return T(0);

      T value(maxabs(it->second));                 // first non-zero row
      for(++it ; it != SM.rowsMap.end(); ++it ) {  // other rows
         T rowval(maxabs(it->second));
         if(rowval > value) value = rowval;
      }

      return value;
   }

   //---------------------------------------------------------------------------------
   /// stream output operator
   template <class T>
   std::ostream& operator<<(std::ostream& os, const SparseMatrix<T>& SM)
   {
      std::ofstream savefmt;
      savefmt.copyfmt(os);

      unsigned int i, j;             // the "real" vector row and column index
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;

      it = SM.rowsMap.begin();
      //if(it == SM.rowsMap.end()) { os << "empty"; return os; }

      for(i=0; i<SM.nrows; i++) {
         if(it != SM.rowsMap.end() && i == it->first) {
            os << std::setw(1) << ' '; os.copyfmt(savefmt);
            os << it->second;                // write the whole row
            ++it;
         }
         else {
            for(j=0; j<SM.ncols; j++) {      // write a row of '0'
               os << std::setw(1) << ' '; os.copyfmt(savefmt);
               os << "0";
            }
         }
         if(i < SM.nrows-1) os << "\n";
      }

      return os;
   }

   //---------------------------------------------------------------------------
   // SparseMatrix operators
   //---------------------------------------------------------------------------

   /// Matrix,Vector multiply: SparseVector = SparseMatrix * SparseVector
   template <class T>
   SparseVector<T> operator*(const SparseMatrix<T>& L, const SparseVector<T>& V)
   {
      try {
         if(L.cols() != V.size())
            GPSTK_THROW(Exception("Incompatible dimensions op*(SM,SV)"));

         SparseVector<T> retSV(L.rows());
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;

         // loop over rows of L = rows of answer
         for(it = L.rowsMap.begin(); it != L.rowsMap.end(); ++it)
            retSV.vecMap[it->first] = dot(it->second,V);

         retSV.zeroize(T(0));
         return retSV;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Matrix,Vector multiply: SparseVector = Matrix * SparseVector
   template <class T>
   SparseVector<T> operator*(const Matrix<T>& L, const SparseVector<T>& V)
   {
      try {
         if(L.cols() != V.size())
            GPSTK_THROW(Exception("Incompatible dimensions op*(M,SV)"));

         SparseVector<T> retSV(L.rows());

         // loop over rows of L = rows of answer
         for(unsigned int i=0; i<L.rows(); i++) {
            T sum(0);
            // loop over elements of V
            typename std::map<unsigned int, T>::const_iterator it;
            for(it = V.vecMap.begin(); it != V.vecMap.end(); ++it)
               sum += L(i,it->first) * it->second;
            retSV.vecMap[i] = sum;
         }

         retSV.zeroize(T(0));
         return retSV;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Matrix,Vector multiply: SparseVector = SparseMatrix * Vector
   template <class T>
   SparseVector<T> operator*(const SparseMatrix<T>& L, const Vector<T>& V)
   {
      try {
         if(L.cols() != V.size())
            GPSTK_THROW(Exception("Incompatible dimensions op*(SM,V)"));

         SparseVector<T> retSV(L.rows());
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;

         // loop over rows of L = rows of answer
         for(it = L.rowsMap.begin(); it != L.rowsMap.end(); ++it)
            retSV.vecMap[it->first] = dot(it->second,V);

         retSV.zeroize(T(0));
         return retSV;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Vector,Matrix multiply: SparseVector = SparseVector * SparseMatrix
   template <class T>
   SparseVector<T> operator*(const SparseVector<T>& V, const SparseMatrix<T>& R)
   {
      if(V.size() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op*(SV,SM)"));

      SparseVector<T> retSV(R.cols());

      // first build a "column map" for R - vectors (of row-index) for each column
      // colMap[column index] = vector of all row indexes, in ascending order
      std::map< unsigned int, std::vector<unsigned int> >::iterator jt;
      std::map< unsigned int, std::vector<unsigned int> > colMap = R.columnMap();

      // loop over columns of R = elements of answer
      for(jt = colMap.begin(); jt != colMap.end(); ++jt) {
         T sum(0);
         // loop over rows of R and elements of V
         for(unsigned int k=0; k<jt->second.size(); k++)
            if(V.isFilled(jt->second[k]))
               sum += V[jt->second[k]] * R(k,jt->first);
         if(sum != T(0)) retSV.vecMap[jt->first] = sum;
      }

      return retSV;
   }

   /// Vector,Matrix multiply: SparseVector = SparseVector * Matrix
   template <class T>
   SparseVector<T> operator*(const SparseVector<T>& V, const Matrix<T>& R)
   {
      try {
         if(V.size() != R.rows())
            GPSTK_THROW(Exception("Incompatible dimensions op*(SV,M)"));

         T sum;
         SparseVector<T> retSV(R.cols());

         // loop over columns of R = elements of answer
         for(unsigned int j=0; j<R.cols(); j++) {
            // copy out the whole column as a Vector
            Vector<T> colR = R.colCopy(j);
            sum = dot(colR,V);
            if(sum != T(0)) retSV.vecMap[j] = sum;
         }

         return retSV;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Vector,Matrix multiply: SparseVector = Vector * SparseMatrix
   template <class T>
   SparseVector<T> operator*(const Vector<T>& V, const SparseMatrix<T>& R)
   {
      if(V.size() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op*(V,SM)"));

      SparseVector<T> retSV(R.cols());

      // first build a "column map" for R - vectors (of row-index) for each column
      // colMap[column index] = vector of all row indexes, in ascending order
      std::map< unsigned int, std::vector<unsigned int> >::iterator jt;
      std::map< unsigned int, std::vector<unsigned int> > colMap = R.columnMap();

      // loop over columns of R = elements of answer
      for(jt = colMap.begin(); jt != colMap.end(); ++jt) {
         T sum(0);
         // loop over rows of R and elements of V
         for(unsigned int k=0; k<jt->second.size(); k++)
            sum += V[jt->second[k]] * R(jt->second[k],jt->first);
         if(sum != T(0)) retSV.vecMap[jt->first] = sum;
      }

      return retSV;
   }

   /// Matrix multiply: SparseMatrix = SparseMatrix * SparseMatrix
   template <class T>
   SparseMatrix<T> operator*(const SparseMatrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.cols() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op*(SM,SM)"));

      const unsigned int nr(L.rows()), nc(R.cols());
      SparseMatrix<T> retSM(nr,nc);                // empty but with correct dimen.
      const SparseMatrix<T> RT(transpose(R));      // work with transpose
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it, jt;

      // loop over rows of L = rows of retSM
      for(it = L.rowsMap.begin(); it != L.rowsMap.end(); ++it) {
         bool haveRow(false);       // use to create the row
         // loop over rows of RT == columns of R
         for(jt = RT.rowsMap.begin(); jt != RT.rowsMap.end(); ++jt) {
            // answer(i,j) = dot product of Lrow and RTrow==Rcol
            T d(dot(it->second,jt->second));
            if(d != T(0)) {
               if(!haveRow) {
                  SparseVector<T> row(nc);
                  retSM.rowsMap[it->first] = row;
                  haveRow = true;
               }
               retSM.rowsMap[it->first].vecMap[jt->first] = d;
            }
         }
      }

      return retSM;
   }

   /// Matrix multiply: SparseMatrix = SparseMatrix * Matrix
   template <class T>
   SparseMatrix<T> operator*(const SparseMatrix<T>& L, const Matrix<T>& R)
   {
      try {
         if(L.cols() != R.rows())
            GPSTK_THROW(Exception("Incompatible dimensions op*(SM,M)"));

         const unsigned int nr(L.rows()), nc(R.cols());
         SparseMatrix<T> retSM(nr,nc);
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;

         // loop over rows of L = rows of answer
         for(it = L.rowsMap.begin(); it != L.rowsMap.end(); ++it) {
            bool haveRow(false);                      // use to create the row
            // loop over columns of R = cols of answer
            for(unsigned int j=0; j<R.cols(); j++) {
               Vector<T> colR(R.colCopy(j));
               T d(dot(it->second,colR));
               if(d != T(0)) {
                  if(!haveRow) {
                     SparseVector<T> row(nc);
                     retSM.rowsMap[it->first] = row;
                     haveRow = true;
                  }
                  retSM.rowsMap[it->first].vecMap[j] = d;
               }
            }
         }

         return retSM;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Matrix multiply: SparseMatrix = Matrix * SparseMatrix
   template <class T>
   SparseMatrix<T> operator*(const Matrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.cols() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op*(M,SM)"));

      const unsigned int nr(L.rows()), nc(R.cols());
      SparseMatrix<T> retSM(nr,nc);
      const SparseMatrix<T> RT(transpose(R));      // work with transpose
      typename std::map< unsigned int, SparseVector<T> >::const_iterator jt;

      // loop over rows of L = rows of retSM
      for(unsigned int i=0; i < nr; i++) {
         bool haveRow(false);                      // use to create the row in retSM
         const Vector<T> rowL(L.rowCopy(i));       // copy out the row in L

         // loop over rows of RT == columns of R
         for(jt = RT.rowsMap.begin(); jt != RT.rowsMap.end(); ++jt) {
            // answer(i,j) = dot product of Lrow and RTrow==Rcol
            T d(dot(rowL,jt->second));
            if(d != T(0)) {
               if(!haveRow) {
                  SparseVector<T> row(nc);
                  retSM.rowsMap[i] = row;
                  haveRow = true;
               }
               retSM.rowsMap[i].vecMap[jt->first] = d;
            }
         }
      }

      return retSM;
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   /// Concatenation SparseMatrix || Vector    TD the rest of them...
   template <class T>
   SparseMatrix<T> operator||(const SparseMatrix<T>& L, const Vector<T>& V)
   {
      if(L.rows() != V.size())
         GPSTK_THROW(Exception("Incompatible dimensions op||(SM,V)"));
      
      SparseMatrix<T> toRet(L);
      toRet.ncols++;

      unsigned int i,n(toRet.ncols-1);
      typename std::map< unsigned int, SparseVector<T> >::iterator jt;
      for(i=0; i<V.size(); i++) {
         if(V(i) != T(0)) {                     // add it
            jt = toRet.rowsMap.find(i);         // find the row
            if(jt == toRet.rowsMap.end()) {     // add the row
               SparseVector<T> V(toRet.ncols);
               toRet.rowsMap[i] = V;
            }
            toRet.rowsMap[i].vecMap[n] = V(i);
         }
      }
      return toRet;
   }

   //---------------------------------------------------------------------------
   template <class T>
   SparseMatrix<T> operator||(const SparseMatrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op||(SM,SM)"));

      SparseMatrix<T> toRet(L);
      toRet.ncols += R.ncols;

      unsigned int i, N(L.ncols);
      typename std::map< unsigned int, SparseVector<T> >::iterator it;
      typename std::map< unsigned int, SparseVector<T> >::const_iterator jt;
      it = toRet.rowsMap.begin();
      jt = R.rowsMap.begin();
      while(it != toRet.rowsMap.end() && jt != R.rowsMap.end()) {
         if(it->first < jt->first) {         // R has no row here - do nothing
            it->second.len += N;
            ++it;
         }
         else if(it->first > jt->first) {    // R has a row where toRet does not
            toRet.rowsMap[jt->first] = jt->second;
            toRet.rowsMap[jt->first].len += N;
            ++jt;
         }
         else {                              // equal indexes - both have rows
            it->second.len += N;
            typename std::map< unsigned int, T >::const_iterator vt;
            for(vt = jt->second.vecMap.begin(); vt != jt->second.vecMap.end(); ++vt) {
               toRet.rowsMap[it->first].vecMap[N+vt->first] = vt->second;
            }
            ++it;
            ++jt;
         }
      }

      return toRet;
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   /// Matrix subtraction: SparseMatrix -= SparseMatrix
   template <class T>
   SparseMatrix<T>& SparseMatrix<T>::operator-=(const SparseMatrix<T>& SM)
   {
      if(SM.cols() != cols() || SM.rows() != rows())
         GPSTK_THROW(Exception("Incompatible dimensions op-=(SM)"));
      //std::cout << "SM::op-=(SM)" << std::endl;

      // loop over rows of SM
      typename std::map< unsigned int, SparseVector<T> >::const_iterator sit;
      for(sit = SM.rowsMap.begin(); sit != SM.rowsMap.end(); ++sit) {
         // find same row in this
         if(rowsMap.find(sit->first) == rowsMap.end())
            rowsMap[sit->first] = -sit->second;
         else
            rowsMap[sit->first] -= sit->second;
      }
      
      zeroize(T(0));
      return *this;
   }

   /// Matrix subtraction: SparseMatrix -= Matrix
   template <class T>
   SparseMatrix<T>& SparseMatrix<T>::operator-=(const Matrix<T>& M)
   {
      if(M.cols() != cols() || M.rows() != rows())
         GPSTK_THROW(Exception("Incompatible dimensions op-=(M)"));
      //std::cout << "SM::op-=(M)" << std::endl;
      
      // loop over rows of M
      for(unsigned int i=0; i<M.rows(); i++) {
         Vector<T> rowV(M.rowCopy(i));
         if(rowsMap.find(i) == rowsMap.end()) {
            SparseVector<T> SV(rowV);
            rowsMap[i] = -SV;
         }
         else
            rowsMap[i] -= rowV;
      }

      zeroize(T(0));
      return *this;
   }
   
   /// Matrix subtraction: SparseMatrix = SparseMatrix - SparseMatrix
   template <class T>
   SparseMatrix<T> operator-(const SparseMatrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.cols() != R.cols() || L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op-(SM,SM)"));
      //std::cout << "SM::op-(SM,SM)" << std::endl;

      SparseMatrix<T> retSM(L);
      retSM -= R;                      // will zeroize(T(0))

      return retSM;
   }

   /// Matrix subtraction: SparseMatrix = SparseMatrix - Matrix
   template <class T>
   SparseMatrix<T> operator-(const SparseMatrix<T>& L, const Matrix<T>& R)
   {
      if(L.cols() != R.cols() || L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op-(SM,M)"));
      //std::cout << "SM::op-(SM,M)" << std::endl;

      SparseMatrix<T> retSM(L);
      retSM -= R;                      // will zeroize(T(0))

      return retSM;
   }

   /// Matrix subtraction: SparseMatrix = Matrix - SparseMatrix
   template <class T>
   SparseMatrix<T> operator-(const Matrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.cols() != R.cols() || L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op-(M,SM)"));
      //std::cout << "SM::op-(M,SM)" << std::endl;

      SparseMatrix<T> retSM(R);
      retSM = -retSM;
      retSM += L;                      // will zeroize(T(0))

      return retSM;
   }

   //---------------------------------------------------------------------------
   /// Matrix addition: SparseMatrix += SparseMatrix
   template <class T>
   SparseMatrix<T>& SparseMatrix<T>::operator+=(const SparseMatrix<T>& SM)
   {
      if(SM.cols() != cols() || SM.rows() != rows())
         GPSTK_THROW(Exception("Incompatible dimensions op+=(SM)"));
      //std::cout << "SM::op+=(SM)" << std::endl;

      // loop over rows of SM
      typename std::map< unsigned int, SparseVector<T> >::const_iterator sit;
      for(sit = SM.rowsMap.begin(); sit != SM.rowsMap.end(); ++sit) {
         // find same row in this
         if(rowsMap.find(sit->first) == rowsMap.end())
            rowsMap[sit->first] = sit->second;
         else
            rowsMap[sit->first] += sit->second;
      }
      
      zeroize(T(0));
      return *this;
   }

   /// Matrix addition: SparseMatrix += Matrix
   template <class T>
   SparseMatrix<T>& SparseMatrix<T>::operator+=(const Matrix<T>& M)
   {
      if(M.cols() != cols() || M.rows() != rows())
         GPSTK_THROW(Exception("Incompatible dimensions op+=(M)"));
      //std::cout << "SM::op+=(M)" << std::endl;
      
      // loop over rows of M
      for(unsigned int i=0; i<M.rows(); i++) {
         Vector<T> rowV(M.rowCopy(i));
         if(rowsMap.find(i) == rowsMap.end()) {
            SparseVector<T> SV(rowV);
            rowsMap[i] = SV;
         }
         else
            rowsMap[i] += rowV;
      }
      
      zeroize(T(0));
      return *this;
   }

   //---------------------------------------------------------------------------
   /// Multiply all elements by a scalar T constant
   template <class T>
   SparseMatrix<T>& SparseMatrix<T>::operator*=(const T& value)
   {
      if(value == T(0)) {
         rowsMap.clear();
         return *this;
      }

      // loop over all elements
      typename std::map< unsigned int, SparseVector<T> >::iterator it;
      typename std::map< unsigned int, T >::iterator vt;
      for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {
         for(vt = it->second.vecMap.begin(); vt != it->second.vecMap.end(); ++vt)
            vt->second *= value;
      }

      return *this;
   }

   /// Divide all elements by a scalar T constant
   /// @throw if the constant is zero
   template <class T>
   SparseMatrix<T>& SparseMatrix<T>::operator/=(const T& value) throw(Exception)
   {
      if(value == T(0)) GPSTK_THROW(Exception("Divide by zero"));

      // loop over all elements
      typename std::map< unsigned int, SparseVector<T> >::iterator it;
      typename std::map< unsigned int, T >::iterator vt;
      for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {
         for(vt = it->second.vecMap.begin(); vt != it->second.vecMap.end(); ++vt)
            vt->second /= value;
      }

      return *this;
   }

   /// Matrix addition: SparseMatrix = SparseMatrix + SparseMatrix : copy, += SM
   template <class T>
   SparseMatrix<T> operator+(const SparseMatrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.cols() != R.cols() || L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op+(SM,SM)"));
      //std::cout << "SM::op+(SM,SM)" << std::endl;

      SparseMatrix<T> retSM(L);
      retSM -= R;                   // will zeroize(T(0))

      return retSM;
   }

   /// Matrix addition: SparseMatrix = SparseMatrix + Matrix : copy, += M
   template <class T>
   SparseMatrix<T> operator+(const SparseMatrix<T>& L, const Matrix<T>& R)
   {
      if(L.cols() != R.cols() || L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op+(SM,M)"));
      //std::cout << "SM::op+(SM,M)" << std::endl;

      SparseMatrix<T> retSM(L);
      retSM -= R;                   // will zeroize(T(0))

      return retSM;
   }

   /// Matrix addition: SparseMatrix = Matrix + SparseMatrix : copy, += M in rev order
   template <class T>
   SparseMatrix<T> operator+(const Matrix<T>& L, const SparseMatrix<T>& R)
   {
      if(L.cols() != R.cols() || L.rows() != R.rows())
         GPSTK_THROW(Exception("Incompatible dimensions op+(M,SM)"));
      //std::cout << "SM::op+(M,SM)" << std::endl;

      SparseMatrix<T> retSM(L);
      retSM -= R;                   // will zeroize(T(0))

      return retSM;
   }

   //---------------------------------------------------------------------------
   // row, col and diagonal copy, swap
   //---------------------------------------------------------------------------
   /// return row i of this SparseMatrix as a SparseVector
   template <class T>
   SparseVector<T> SparseMatrix<T>::rowCopy(const unsigned int i) const
   {
      SparseVector<T> retSV;
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      it = rowsMap.find(i);
      if(it != rowsMap.end())
         retSV = it->second;
      return retSV;
   }

   /// return col j of this SparseMatrix as a SparseVector
   template <class T>
   SparseVector<T> SparseMatrix<T>::colCopy(const unsigned int j) const
   {
      SparseVector<T> retSV;

      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {   // loop over rows
         if(it->second.isFilled(j))
            retSV.vecMap[it->first] = it->second[j];
      }
      retSV.len = rows();

      return retSV;
   }

   /// return diagonal of this SparseMatrix as a SparseVector
   template <class T>
   SparseVector<T> SparseMatrix<T>::diagCopy(void) const
   {
      SparseVector<T> retSV;

      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      for(it = rowsMap.begin(); it != rowsMap.end(); ++it) {   // loop over rows
         if(it->second.isFilled(it->first))
            retSV.vecMap[it->first] = it->second[it->first];
      }
      retSV.len = rows();

      return retSV;
   }

   /// swap rows of this SparseMatrix
   template <class T>
   void SparseMatrix<T>::swapRows(const unsigned int& ii, const unsigned int& jj)
   {
      if(ii >= nrows || jj >= nrows) GPSTK_THROW(Exception("Invalid indexes"));
      
      typename std::map< unsigned int, SparseVector<T> >::iterator it, jt;
      it = rowsMap.find(ii);
      jt = rowsMap.find(jj);
      if(it == rowsMap.end()) {
         if(jt == rowsMap.end()) return;           // nothing to do
         else {
            rowsMap[ii] = rowsMap[jj];
            rowsMap.erase(jt);
         }
      }
      else {
         if(jt == rowsMap.end()) {
            rowsMap[jj] = rowsMap[ii];
            rowsMap.erase(it);
         }
         else {
            SparseVector<T> save(it->second);
            rowsMap[ii] = rowsMap[jj];
            rowsMap[jj] = save;
         }
      }
   }

   /// swap columns of this SparseMatrix
   template <class T>
   void SparseMatrix<T>::swapCols(const unsigned int ii, const unsigned int jj)
   {
      if(ii >= ncols || jj >= ncols) GPSTK_THROW(Exception("Invalid indexes"));

      // may not be the fastest, but may be fast enough - tranpose() is fast
      SparseMatrix<T> trans(transpose(*this));
      trans.swapRows(ii,jj);
      *this = transpose(*this);
   }

   //---------------------------------------------------------------------------------
   // special matrices
   //---------------------------------------------------------------------------------
   /// Compute the identity matrix of dimension dim x dim
   /// @param dim dimension of desired identity matrix (dim x dim)
   /// @return identity matrix
   template <class T>
   SparseMatrix<T> identSparse(const unsigned int dim) throw()
   {
      if(dim == 0) return SparseMatrix<T>();

      SparseMatrix<T> toRet(dim,dim);
      for(unsigned int i=0; i<dim; i++) {
         SparseVector<T> SV(dim);
         SV.vecMap[i] = T(1);
         toRet.rowsMap[i] = SV;
      }

      return toRet;
   }

   //---------------------------------------------------------------------------------
   // matrix products and transformations
   //---------------------------------------------------------------------------------

   //---------------------------------------------------------------------------------
   // SM * transpose(SM)
   // NB this is barely faster than just forming SM * transpose(SM)
   template <class T>
   SparseMatrix<T> matrixTimesTranspose(const SparseMatrix<T>& SM) throw(Exception)
   {
      try {
         SparseMatrix<T> toRet(SM.rows(),SM.rows());

         typename std::map< unsigned int, SparseVector<T> >::const_iterator it, jt;
         for(it = SM.rowsMap.begin(); it != SM.rowsMap.end(); ++it) {
            SparseVector<T> Vrow(SM.rows());
            toRet.rowsMap[it->first] = Vrow;
            for(jt = SM.rowsMap.begin(); jt != SM.rowsMap.end(); ++jt) {
               T d(dot(it->second,jt->second));
               if(d != T(0)) toRet.rowsMap[it->first][jt->first] = d;
            }
         }

         return toRet;

      } catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   //---------------------------------------------------------------------------------
   /// Compute diagonal of P*C*P^T, ie diagonal of transform of square Matrix C.
   template <class T>
   Vector<T> transformDiag(const SparseMatrix<T>& P, const Matrix<T>& C)
      throw(Exception)
   {
      try {
         if(P.cols() != C.rows() || C.rows() != C.cols())
            GPSTK_THROW(Exception("Incompatible dimensions transformDiag()"));

         const unsigned int n(P.cols());
         typename std::map< unsigned int, SparseVector<T> >::const_iterator jt;
         typename std::map< unsigned int, T >::const_iterator vt;

         Vector<T> toRet(P.rows(),T(0));
         T sum;

         // loop over rows of P = columns of transpose(P)
         for(jt = P.rowsMap.begin(); jt != P.rowsMap.end(); ++jt) {
            unsigned int j = jt->first;         // row of P, col of P^T
            Vector<T> prod(n,T(0));

            // loop over columns of C up to and including j,
            // forming dot product prod(k) = P(rowj) * C(colk)
            for(unsigned int k=0; k<n; k++) {
               sum = T(0);
               for(vt=jt->second.vecMap.begin(); vt!=jt->second.vecMap.end(); ++vt)
                  sum += vt->second * C(vt->first,k);   
               if(sum != T(0)) prod(k) = sum;
            }
            toRet(j) = dot(jt->second,prod);
         }
         return toRet;

      } catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------
   // inverse (via Gauss-Jordan)
   //---------------------------------------------------------------------------
   /// inverse via Gauss-Jordan; NB GJ involves only row operations.
   /// NB not the best numerically; for high condition number, use inverseViaCholesky,
   /// or cast to Matrix, use either LUD or SVD, then cast back to SparseMatrix.
   template <class T>
   SparseMatrix<T> inverse(const SparseMatrix<T>& A) throw(Exception)
   {
      try {
         if(A.rows() != A.cols() || A.rows() == 0) {
            std::ostringstream oss;
            oss << "Invalid input dimensions: " << A.rows() << "x" << A.cols();
            GPSTK_THROW(Exception(oss.str()));
         }

         unsigned int i,k;
         T dtmp;
         //T big, small;
         typename std::map< unsigned int, SparseVector<T> >::const_iterator it;

         // does A have any zero rows?
         for(it = A.rowsMap.begin(), i=0; it != A.rowsMap.end(); i++, ++it) {
            if(i != it->first) {
               std::ostringstream oss;
               oss << "Singular matrix - zero row at index " << i << ")";
               GPSTK_THROW(Exception(oss.str()));
            }
         }

         const unsigned int N(A.rows());
         typename std::map< unsigned int, SparseVector<T> >::iterator jt,kt;
         typename std::map< unsigned int, T >::iterator vt;
         SparseMatrix<T> GJ(A || identSparse<T>(N));

         //std::cout << "\nInitial:\n" << std::scientific
         //            << std::setprecision(2) << std::setw(10) << GJ << std::endl;

         // loop over rows of work matrix, making lower left triangular = unity
         for(jt = GJ.rowsMap.begin(); jt != GJ.rowsMap.end(); ++jt) {

            // divide row by diagonal element; if diagonal is zero, add another row
            vt = jt->second.vecMap.find(jt->first);      // diagonal element GJ(j,j)
            if(vt == jt->second.vecMap.end() || vt->second == T(0)) {
               // find a lower row with non-zero element (same col); add to this row
               for((kt=jt)++; kt != GJ.rowsMap.end(); ++kt) {
                  vt = kt->second.vecMap.find(jt->first);      // GJ(k,j)
                  if(vt == kt->second.vecMap.end() || vt->second == T(0))
                     continue;                                 // nope, its zero

                  // add the kt row to the jt row
                  jt->second += kt->second;
                  break;
               }
               if(kt == GJ.rowsMap.end())
                  GPSTK_THROW(Exception("Singular matrix"));
            }

            dtmp = vt->second;
            // are these scales 1/dtmp related to condition number? eigenvalues? det?
            // they are close to condition number....
            //if(jt == GJ.rowsMap.begin()) big = small = ::fabs(dtmp);
            //if(::fabs(dtmp) > big) big = ::fabs(dtmp);
            //if(::fabs(dtmp) < small) small = ::fabs(dtmp);

            // normalize the j row
            if(dtmp != T(1)) jt->second *= T(1)/dtmp;

            //std::cout << "\nRow " << jt->first << " scaled with " << std::scientific
            //   << std::setprecision(2) << T(1)/dtmp << "\n"
            //   << std::setw(10) << GJ << std::endl;

            // now zero out the column below the j diagonal
            for((kt=jt)++; kt != GJ.rowsMap.end(); ++kt) {
               vt = kt->second.vecMap.find(jt->first);      // GJ(k,j)
               if(vt == kt->second.vecMap.end() || vt->second == T(0))
                  continue;                                 // already zero

               kt->second.addScaledSparseVector(-vt->second, jt->second);
            }

            //std::cout << "\nRow " << jt->first << " left-zeroed:\n"
            //   << std::scientific << std::setprecision(2) << std::setw(10)
            //   << GJ << std::endl;
         }

         // loop over rows of work matrix in reverse order,
         // zero-ing out the column above the diag
         typename std::map< unsigned int, SparseVector<T> >::reverse_iterator rjt,rkt;
         for(rjt = GJ.rowsMap.rbegin(); rjt != GJ.rowsMap.rend(); ++rjt) {
            // now zero out the column above the j diagonal
            for((rkt=rjt)++; rkt != GJ.rowsMap.rend(); ++rkt) {
               vt = rkt->second.vecMap.find(rjt->first);    // GJ(k,j)
               if(vt == rkt->second.vecMap.end() || vt->second == T(0))
                  continue;                                 // already zero
               rkt->second.addScaledSparseVector(-vt->second, rjt->second);
            }

            //std::cout << "\nRow " << rjt->first << " right-zeroed:\n"
            //   << std::scientific << std::setprecision(2) << std::setw(10)
            //   << GJ << std::endl;
         }

         //std::cout << "\nbig and small for this matrix are: "
         //   << std::scientific << std::setprecision(2)
         //   << big << " " << small << " with ratio " << big/small << std::endl;

         return (SparseMatrix<T>(GJ,0,N,N,N));

      } catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------
   // Factorization, decomposition and other algorithms
   //---------------------------------------------------------------------------

   //---------------------------------------------------------------------------------
   // Compute Cholesky decomposition of symmetric positive definite matrix using Crout
   // algorithm. A = L*L^T where A and L are (nxn) and L is lower triangular reads:
   // [ A00 A01 A02 ... A0n ] = [ L00  0   0  0 ...  0 ][ L00 L10 L20 ... L0n ]
   // [ A10 A11 A12 ... A1n ] = [ L10 L11  0  0 ...  0 ][  0  L11 L21 ... L1n ]
   // [ A20 A21 A22 ... A2n ] = [ L20 L21 L22 0 ...  0 ][  0   0  L22 ... L2n ]
   //           ...                      ...                  ...
   // [ An0 An1 An2 ... Ann ] = [ Ln0 Ln1 Ln2 0 ... Lnn][  0   0   0  ... Lnn ]
   //   but multiplying out gives
   //          A              = [ L00^2
   //                           [ L00*L10  L11^2+L10^2
   //                           [ L00*L20  L11*L21+L10*L20 L22^2+L21^2+L20^2
   //                                 ...
   //    Aii = Lii^2 + sum(k=0,i-1) Lik^2
   //    Aij = Lij*Ljj + sum(k=0,j-1) Lik*Ljk
   // These can be inverted by looping over columns, and filling L from diagonal down.
   // So fill L in this way
   //     d         do diagonal element first, then the column below it
   //     1d        at each row i below the diagonal, save the element^2 in rowSums[i]
   //     12d
   //     123d
   //     123 d
   //     123  d
   //     123   d
   //     123    d
   //     123 etc d
   
   /// Compute lower triangular square root of a symmetric positive definite matrix
   /// (Cholesky decomposition) Crout algorithm.
   /// @param A SparseMatrix to be decomposed; symmetric and positive definite, const
   /// @return SparseMatrix lower triangular square root of input matrix
   /// @throw if input SparseMatrix is not square
   /// @throw if input SparseMatrix is not positive definite
   template <class T>
   SparseMatrix<T> lowerCholesky(const SparseMatrix<T>& A) throw(Exception)
   {
      if(A.rows() != A.cols() || A.rows() == 0) {
         std::ostringstream oss;
         oss << "Invalid input dimensions: " << A.rows() << "x" << A.cols();
         GPSTK_THROW(Exception(oss.str()));
      }
   
      const unsigned int n=A.rows();
      unsigned int i,j,k;
      T d, diag;
      SparseMatrix<T> L(n,n);          // compute the answer
      std::vector<T> rowSums;          // keep sum(k=0..j-1)[L(j,k)^2] for each row j
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it, jt;
      typename std::map< unsigned int, SparseVector<T> >::iterator Lit, Ljt;

      // A must have all rows - a zero row in A means its singular
      // create all the rows in L; all exist b/c if any diagonal is 0 -> singular
      // fill rowSums vector with zeros
      for(it=A.rowsMap.begin(), i=0; it!=A.rowsMap.end(); i++, ++it) {
         if(i != it->first) {
            std::ostringstream oss;
            oss << "lowerCholesky() requires positive-definite input:"
               << " (zero rows at index " << i << ")";
            GPSTK_THROW(Exception(oss.str()));
         }

         SparseVector<T> Vrow(n);
         L.rowsMap[i] = Vrow;

         rowSums.push_back(T(0));
      }

      // loop over columns of A, at the same time looping over rows of L
      // use jt to iterate over the columns of A, keeping count with (column) j
      for(jt = A.rowsMap.begin(), Ljt = L.rowsMap.begin();
            jt != A.rowsMap.end() && Ljt != L.rowsMap.end();  ++jt, ++Ljt)
      {
         j = jt->first;                            // column j (A) or row i (L)

         // compute the j,j diagonal element of L
         // start with diagonal of A(j,j)
         d = jt->second[j];                     // A(j,j)

         // subtract sum(k=0..j-1)[L(j,k)^2]
         d -= rowSums[j];

         // d is the eigenvalue - must not be zero
         if(d <= T(0)) {
            std::ostringstream oss;
            oss << "Non-positive eigenvalue " << std::scientific << d << " at col "
               << j << ": lowerCholesky() requires positive-definite input";
            GPSTK_THROW(Exception(oss.str()));
         }

         diag = SQRT(d);
         L.rowsMap[j].vecMap[j] = diag;         // L(j,j)

         // now loop over rows below the diagonal, filling in this column
         Lit = Ljt;
         it = jt;
         for(++Lit, ++it; Lit != L.rowsMap.end(); ++Lit, ++it) {
            i = Lit->first;
            d = (it->second.isFilled(j) ?  it->second[j] : T(0));
            d -= dot_lim(Lit->second, Ljt->second, 0, j);

            if(d != T(0)) {
               d /= diag;
               Lit->second.vecMap[j] = d;
               rowSums[i] += d*d;  // save L(i,j)^2 term
            }

         }  // end loop over rows below the diagonal

      }  // end loop over column j of A
      
      return L;
   }

   //---------------------------------------------------------------------------------
   /// Compute inverse of lower-triangular SparseMatrix
   template <class T>
   SparseMatrix<T> inverseLT(const SparseMatrix<T>& L, T *ptrSmall, T *ptrBig)
      throw(Exception)
   {
      if(L.rows() != L.cols() || L.rows() == 0) {
         std::ostringstream oss;
         oss << "Invalid input dimensions: " << L.rows() << "x" << L.cols();
         GPSTK_THROW(Exception(oss.str()));
      }

      const unsigned int n(L.rows());
      unsigned int i,j,k;
      T big(0), small(0), dum, sum;

      // trick is to fill transpose(inverse) and then transpose at the end
      SparseMatrix<T> invLT(L.cols(),L.rows()); 
      typename std::map< unsigned int, SparseVector<T> >::const_iterator it;
      typename std::map< unsigned int, SparseVector<T> >::iterator jt;

      // do the diagonal first; this finds singularities and defines all rows in InvLT
      for(i=0, it = L.rowsMap.begin(); i < n; ++it, ++i) {
         if(it == L.rowsMap.end() || it->first != i ||
               !it->second.isFilled(i) || it->second[i]==T(0))
         {
            std::ostringstream oss;
            oss << "Singular matrix - zero diagonal at row " << i;
            GPSTK_THROW(Exception(oss.str()));
         }

         dum = it->second[i];
         if(ptrSmall) {
            if(ABS(dum) > big) big = ABS(dum);
            if(ABS(dum) < small) small = ABS(dum);
         }

         // create row i and element i,i in the answer
         dum = T(1)/dum;
         SparseVector<T> SV(L.cols());
         SV.vecMap[i] = dum;
         invLT.rowsMap[i] = SV;
      }

      // loop over rows again, filling in below the diagonal
      // (L has all rows present, else its singular above)
      //for(i=1; i<n; i++
      it = L.rowsMap.begin();
      for(++it; it != L.rowsMap.end(); ++it) {
         dum = T(1)/it->second[it->first];      // has to be there, and non-zero
         // loop over columns of invL (rows of invLT) before the diagonal
         // store results temporarily in a map
         std::map<unsigned int,T> tempMap;
         //for(j=0; j<i; j++)
         for(jt = invLT.rowsMap.begin(); jt != invLT.rowsMap.end(); ++jt) {
            if(jt->first >= it->first) break;
            //sum=0; for(k=j;k<i;k++) sum += L(i,k)*invLT(j,k)
            sum = dot_lim(it->second, jt->second, jt->first, it->first);
            //invLT(j,i) = -sum*dum
            if(sum != T(0)) tempMap[jt->first] = -dum*sum;
            //jt->second.vecMap[it->first] = -dum*sum;
         }
         // now move contents of tempMap to invLT
         typename std::map<unsigned int,T>::iterator tt = tempMap.begin();
         for( ; tt != tempMap.end(); ++tt)
            invLT.rowsMap[tt->first].vecMap[it->first] = tt->second; // invLT(j,i)
      }

      if(ptrSmall) *ptrSmall = small;
      if(ptrBig) *ptrBig = big;

      return transpose(invLT);
   }

   //---------------------------------------------------------------------------------
   /// Compute upper triangular square root of a symmetric positive definite matrix
   /// (Cholesky decomposition) Crout algorithm; that is A = transpose(U)*U.
   /// Note that this result will be equal to
   /// transpose(lowerCholesky(A)) == transpose(Ch.L from class Cholesky), NOT Ch.U;
   /// class Cholesky computes L,U where A = L*LT = U*UT [while A=UT*U here].
   /// @param A SparseMatrix to be decomposed; symmetric and positive definite, const
   /// @return SparseMatrix upper triangular square root of input matrix
   /// @throw if input SparseMatrix is not square
   /// @throw if input SparseMatrix is not positive definite
   template <class T>
   SparseMatrix<T> upperCholesky(const SparseMatrix<T>& A) throw(Exception)
      { return transpose(lowerCholesky(A)); }

   //---------------------------------------------------------------------------------
   /// Compute inverse of a symmetric positive definite matrix using Cholesky
   /// decomposition.
   /// @param A SparseMatrix to be inverted; symmetric and positive definite, const
   /// @return SparseMatrix inverse of input matrix
   /// @throw if input SparseMatrix is not square, not positive definite, or singular
   template <class T>
   SparseMatrix<T> inverseViaCholesky(const SparseMatrix<T>& A) throw(Exception)
   {
      try {
         //SparseMatrix<T> L(lowerCholesky(A));
         //SparseMatrix<T> Linv(inverseLT(L));
         //SparseMatrix<T> Ainv(matrixTimesTranspose(transpose(Linv)));
         //return Ainv;
         return (matrixTimesTranspose(transpose(inverseLT(lowerCholesky(A)))));
      }
      catch(Exception& me) {
         me.addText("Called by inverseViaCholesky()");
         GPSTK_RETHROW(me);
      }
   }

   //---------------------------------------------------------------------------------
   /// Householder transformation of a matrix.
   template <class T>
   SparseMatrix<T> SparseHouseholder(const SparseMatrix<T>& A) throw(Exception)
   {
      unsigned int i,j,k;
      typename std::map< unsigned int, SparseVector<T> >::iterator jt,kt,it;
      typename std::map< unsigned int, T >::iterator vt;

      SparseMatrix<T> AT(transpose(A));      // perform the algorithm on the transpose

      // loop over rows (columns of input A)
      for(j=0; (j<A.cols()-1 && j<A.rows()-1); j++) {
         jt = AT.rowsMap.find(j);            // is column j there?
         if(jt == AT.rowsMap.end())          // no, so A is already zero below diag
            continue;

         // pull out column j (use only below and including diagonal, ignore V(i<j))
         SparseVector<T> V(jt->second);
         T sum(0);
         for(vt=V.vecMap.begin(); vt!=V.vecMap.end(); ++vt) {
            if(vt->first < j) continue;
            sum += vt->second * vt->second;
         }
         if(sum < T(1.e-20)) continue;       // col j is already zero below diag

         //zero out below diagonal - must remove element
         vt = jt->second.vecMap.lower_bound(jt->first);
         if(vt != jt->second.vecMap.end())
            jt->second.vecMap.erase(vt,jt->second.vecMap.end());

         sum = SQRT(sum);
         vt = V.vecMap.find(j);
         if(vt != V.vecMap.end()) {
            if(vt->second > T(0)) sum = -sum;
            jt->second[j] = sum;             // A(j,j) = sum
            vt->second -= sum;               // V(j) -= sum
            sum = T(1)/(sum*vt->second);
         }
         else {
            jt->second[j] = sum;             // A(j,j) = sum
            V.vecMap[j] = -sum;              // V(j) -= sum
            sum = T(-1)/(sum*sum);
         }

         // loop over columns beyond j
         kt = jt;
         for(++kt; kt != AT.rowsMap.end(); ++kt) {
            T alpha(0);
            for(vt=kt->second.vecMap.begin(); vt!=kt->second.vecMap.end(); ++vt) {
               if(vt->first < j) continue;
               i = vt->first;
               if(V.isFilled(i))             // alpha += A(i,k)*V(i)
                  alpha += vt->second * V.vecMap[i];
            }
            alpha *= sum;
            if(alpha == T(0)) continue;
            // modify column k at and below j
            for(i=jt->first; i<AT.cols(); i++) {
               if(!V.isFilled(i)) continue;
               vt = kt->second.vecMap.find(i);
               if(vt == kt->second.vecMap.end()) {    // create element
                  kt->second.vecMap[i] = alpha * V.vecMap[i];
               }
               else {
                  kt->second.vecMap[i] += alpha * V.vecMap[i];
               }
            }
         }
      }

      return (transpose(AT));
   }

   //---------------------------------------------------------------------------------
   // This routine uses the Householder algorithm to update the SRI state+covariance.
   // Input:
   //    R  a priori SRI matrix (upper triangular, dimension N)
   //    Z  a priori SRI data vector (length N)
   //    A  concatentation of H and D : A = H || D, where
   //    H  Measurement partials, an M by N matrix.
   //    D  Data vector, of length M
   // Output: Updated R and Z.  H is trashed, but the data vector D
   //    contains the residuals of fit (D - A*state).
   // Return values: SrifMU returns void, but throws exceptions if the input matrices
   //    or vectors have incompatible dimensions.
   // 
   // Measurment noise associated with H and D must be white with unit covariance.
   // If necessary, the data can be 'whitened' before calling this routine in order
   // to satisfy this requirement. This is done as follows.
   // Compute the lower triangular square root of the covariance matrix, L,
   // and replace H with inverse(L)*H and D with inverse(L)*D.
   // 
   //    The Householder transformation is simply an orthogonal transformation
   // designed to make the elements below the diagonal zero. It works by explicitly
   // performing the transformation, one column at a time, without actually
   // constructing the transformation matrix. The matrix is transformed as follows
   //   [  A(m,n) ] => [ sum       a       ]
   //   [         ] => [  0    A'(m-1,n-1) ]
   // after which the same transformation is applied to A' matrix, until A' has only
   // one row or column. The transformation that zeros the diagonal below the (k,k)
   // element also replaces the (k,k) element and modifies the matrix elements for
   // columns >= k and rows >=k, but does not affect the matrix for columns < k
   // or rows < k.
   //    Column k (=0..min(m,n)-1) of the input matrix A(m,n) can be zeroed
   // below the diagonal (columns < k have already been so zeroed) as follows:
   //    let y be the vector equal to column k at the diagonal and below,
   //       ( so y(j)==A(k+j,k), y(0)==A(k,k), y.size = m-k )
   //    let sum = -sign(y(0))*|y|,
   //    define vector u by u(0) = y(0)-sum, u(j)=y(j) for j>0 (j=1..m-k)
   //    and finally define b = 1/(sum*u(0)).
   // Redefine column k with A(k,k)=sum and A(k+j,k)=0, j=1..m, and then for
   // each column j > k, (j=k+1..n)
   //    compute g = b*sum[u(i)*A(k+i,j)], i=0..m-k-1,
   //    replace A(k+i,j) with A(k+i,j)+g*u(i), for i=0..m-k-1
   // Most algorithms don't handle special cases:
   // 1. If column k is already zero below the diagonal, but A(k,k)!=0, then
   // y=[A(k,k),0,0,...0], sum=-A(k,k), u(0)=2A(k,k), u=[2A(k,k),0,0,...0]
   // and b = -1/(2*A(k,k)^2). Then, zeroing column k only changes the sign
   // of A(k,k), and for the other columns j>k, g = -A(k,j)/A(k,k) and only
   // row k is changed.
   // 2. If column k is already zero below the diagonal, AND A(k,k) is zero,
   // then y=0,sum=0,u=0 and b is infinite...the transformation is undefined.
   // However this column should be skipped (Biermann Appendix VII.B).
   //
   // Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
   //      Estimation," Academic Press, 1977.
   //
   /// Square root information measurement update, with new data in the form of a
   /// single SparseMatrix concatenation of H and D: A = H || D.
   /// See doc for the overloaded SrifMU().
   template <class T>
   void SrifMU(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& A, const unsigned int M)
      throw(Exception)
   {
      // if necessary, create R and Z
      if(A.cols() > 1 && R.rows() == 0 && Z.size() == 0) {
         R = Matrix<double>(A.cols()-1,A.cols()-1,0.0);
         Z = Vector<double>(A.cols()-1,0.0);
      }

      if(A.cols() <= 1 || A.cols() != R.cols()+1 || Z.size() < R.rows()) {
         std::ostringstream oss;
         oss << "Invalid input dimensions:\n  R has dimension "
            << R.rows() << "x" << R.cols() << ",\n  Z has length "
            << Z.size() << ",\n  and A has dimension "
            << A.rows() << "x" << A.cols();
         GPSTK_THROW(Exception(oss.str()));
      }
   
      const T EPS=T(1.e-20);
      const unsigned int m(M==0 || M>A.rows() ? A.rows() : M), n(R.rows());
      const unsigned int np1(n+1);  // if np1 = n, state vector Z is not updated
      unsigned int i,j,k;
      T dum, delta, beta;
      typename std::map< unsigned int, SparseVector<T> >::iterator jt,kt,it;
      typename std::map< unsigned int, T >::iterator vt;

      SparseMatrix<T> AT(transpose(A));         // work with the transpose
   
      for(j=0; j<n; j++) {          // loop over columns
         jt = AT.rowsMap.find(j);   // is column j empty?
         if(jt == AT.rowsMap.end()) //   no, so A is already zero below the diagonal
            continue;

         // pull out column j of A; it is entirely below the diagonal
         SparseVector<T> Vj(jt->second);
         T sum(dot(Vj,Vj));
         //T sum(0);
         //for(i=0; i<m; i++)
         //   sum += A(i,j)*A(i,j); // sum of squares of elements in column below diag
         if(sum < EPS) continue;    // sum is positive
   
         dum = R(j,j);
         sum += dum * dum;          // add diagonal element
         sum = (dum > T(0) ? -T(1) : T(1)) * SQRT(sum);
         delta = dum - sum;
         R(j,j) = sum;
   
         //if(j+1 > np1) break;       // this in case np1 is ever set to n ....
   
         beta = sum*delta;          // beta by construction must be negative
         if(beta > -EPS) continue;
         beta = T(1)/beta;

         kt = jt;
         for(k=j+1; k<np1; k++) {   // columns to right of diagonal (j,j)
            SparseVector<T> Vk(m);     // will be column k of A

            if(kt->first == k-1) ++kt; // kt now points to next column -- is it k?
            if(kt->first != k) {       // no col k - should create a column in A...
               AT.rowsMap[k] = Vk;
               kt = AT.rowsMap.find(k);
            }
            else
               Vk = kt->second;        // now Vk is column k of A, perhaps empty

            sum = delta * (k==n ? Z(j) : R(j,k));
            sum += dot(Vk,Vj);
       //     for(i=0; i<m; i++)
       //        sum += A(i,j) * A(i,k);
            if(sum == T(0)) continue;
   
            sum *= beta;
            if(k==n) Z(j) += sum*delta;
            else   R(j,k) += sum*delta;
   
            vt = kt->second.vecMap.begin();
            for(i=0; i<m; i++) {       // loop over rows in column k
       //      A(i,k) += sum * A(i,j);
               if(vt != kt->second.vecMap.end() && vt->first == i) {
                  vt->second += sum * Vj.vecMap[i];
                  ++vt;
               }
               else
                  kt->second.vecMap[i] = sum * Vj.vecMap[i];
            }
         }
      }

      // must put last row of AT (last column of A) back into A - these are residuals
      jt = AT.rowsMap.find(AT.rows()-1);
      // should never happen
      if(jt == AT.rowsMap.end()) GPSTK_THROW(Exception("Failure on last column"));

      // put this row, jt->second, into the last column of A
      j = A.cols()-1;
      i = 0;
      it = A.rowsMap.begin();
      vt = jt->second.vecMap.begin();
      while(it != A.rowsMap.end() && vt != jt->second.vecMap.end()) {
         if(it->first > vt->first) {         // A has no row at index vt->first
            SparseVector<T> SV(A.cols());
            SV.vecMap[j] = vt->second;
            A.rowsMap[vt->first] = SV;
            ++vt;
         }
         else if(vt->first > it->first) {    // resids are missing at this row
            ++it;
         }
         else {                              // match - equal indexes
            A.rowsMap[vt->first].vecMap[j] = vt->second;
            ++it;
            ++vt;
         }
      }

   }  // end SrifMU

   //---------------------------------------------------------------------------
   template <class T>
   void SrifMU(Matrix<T>& R, Vector<T>& Z, SparseMatrix<T>& P,
                             Vector<T>& D, const unsigned int M) throw(Exception)
   {
      try {
         SparseMatrix<T> A(P||D);
         SrifMU(R,Z,A,M);
         // copy residuals out of A into D
         D = Vector<T>(A.colCopy(A.cols()-1));
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------

}  // namespace

#endif   // define SPARSE_MATRIX_INCLUDE
