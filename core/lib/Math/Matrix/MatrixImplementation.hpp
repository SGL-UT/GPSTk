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

/**
 * @file MatrixImplementation.hpp
 * Implementation of Matrix algorithms
 */

#ifndef GPSTK_MATRIX_IMPLEMENTATION_HPP
#define GPSTK_MATRIX_IMPLEMENTATION_HPP

namespace gpstk
{

      /// @ingroup MathGroup
      //@{

   template <class T>
   Matrix<T>::Matrix()
         : v((size_t)0), r(0), c(0), s(0)
   {}


   template <class T>
   Matrix<T>::Matrix(size_t rows, size_t cols)
         : v(rows * cols), r(rows), c(cols), s(rows * cols)
   {}

   template <class T>
   Matrix<T>::Matrix(size_t rows, size_t cols,
                     T initialValue)
         : v(rows * cols, initialValue), r(rows), c(cols), s(rows * cols)
   {}

   template <class T>
   Matrix<T>::Matrix(size_t rows, size_t cols,
                     const T* vec)
         : v(rows * cols), r(rows), c(cols), s(rows * cols)
   {
      this->assignFrom(vec);
   }

   template <class T>
   MatrixRowSlice<T> Matrix<T>::rowRef(size_t rowNum, const std::slice& s)
   {
      return MatrixRowSlice<T>(*this, rowNum, s);
   }

   template <class T>
   MatrixRowSlice<T> Matrix<T>::rowRef(size_t rowNum, size_t colNum)
   {
      return MatrixRowSlice<T>(*this, rowNum, 
                               std::slice(colNum, cols()-colNum, 1));
   }

   template <class T>
   ConstMatrixRowSlice<T> Matrix<T>::row(size_t rowNum, const std::slice& s) 
      const
   {
      return ConstMatrixRowSlice<T>(*this, rowNum, s);
   }

   template <class T>
   ConstMatrixRowSlice<T> Matrix<T>::row(size_t rowNum, size_t colNum)
      const
   {
      return ConstMatrixRowSlice<T>(*this, rowNum, 
                                    std::slice(colNum, cols()-colNum, 1));
   }

   template <class T>
   MatrixColSlice<T> Matrix<T>::colRef(size_t colNum, const std::slice& s)
   {
      return MatrixColSlice<T>(*this, colNum, s);
   }

   template <class T>
   MatrixColSlice<T> Matrix<T>::colRef(size_t colNum, size_t rowNum)
   {
      return MatrixColSlice<T>(*this, colNum, 
                               std::slice(rowNum, rows() - rowNum, 1));
   }

   template <class T>
   ConstMatrixColSlice<T> Matrix<T>::col(size_t colNum, 
                                         const std::slice& s) const
   {
      return ConstMatrixColSlice<T>(*this, colNum, s);
   }

   template <class T>
   ConstMatrixColSlice<T> Matrix<T>::col(size_t colNum, 
                                         size_t rowNum) const
   {
      return ConstMatrixColSlice<T>(*this, colNum,
                                    std::slice(colNum * r + rowNum, r - rowNum, 1));
   }

   template <class T>
   Matrix<T>& Matrix<T>::resize(size_t rows, size_t cols)
   {
      v.resize(rows * cols);
      c = cols;
      r = rows;
      s = rows * cols;
      return *this;
   }

   template <class T>
   Matrix<T>& Matrix<T>::resize(size_t rows, size_t cols,
                                const T initialValue)
   {
      v.resize(rows * cols, initialValue);
      c = cols;
      r = rows;
      s = rows * cols;
      return *this;
   }

      //@}

}  // namespace

#endif
