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
 * @file SRI.cpp
 * Implementation of class SRI.
 * class SRI implements the square root information methods, used for least squares
 * estimation and the SRI form of the Kalman filter.
 *
 * Reference: "Factorization Methods for Discrete Sequential Estimation,"
 *             by G.J. Bierman, Academic Press, 1977.
 */

// -----------------------------------------------------------------------------------
// system
#include <string>
#include <vector>
#include <algorithm>
#include <ostream>
// geomatics
#include "SRI.hpp"
#include "Namelist.hpp"
#include "logstream.hpp"
// GPSTk
#include "StringUtils.hpp"

using namespace std;

namespace gpstk
{
using namespace StringUtils;

   // --------------------------------------------------------------------------------
   // used to mark optional input
   const Matrix<double> SRINullMatrix;
   const SparseMatrix<double> SRINullSparseMatrix;

   //---------------------------------------------------------------------------------
   // constructor given the dimension N.
   SRI::SRI(const unsigned int N)
      throw()
   {
      R = Matrix<double>(N,N,0.0);
      Z = Vector<double>(N,0.0);
      names = Namelist(N);
   }

   // --------------------------------------------------------------------------------
   // constructor given a Namelist, its dimension determines the SRI dimension.
   SRI::SRI(const Namelist& nl)
      throw()
   {
      if(nl.size() <= 0) return;
      R = Matrix<double>(nl.size(),nl.size(),0.0);
      Z = Vector<double>(nl.size(),0.0);
      names = nl;
   }

   // --------------------------------------------------------------------------------
   // explicit constructor - throw if the dimensions are inconsistent.
   SRI::SRI(const Matrix<double>& r,
            const Vector<double>& z,
            const Namelist& nl)
      throw(MatrixException)
   {
      if(r.rows() != r.cols() || r.rows() != z.size() || r.rows() != nl.size()) {
         MatrixException me("Invalid dimensions in explicit SRI constructor:\n R is "
               + asString<int>(r.rows()) + "x"
               + asString<int>(r.cols()) + ", Z has length "
               + asString<int>(z.size()) + " and NL has length "
               + asString<int>(nl.size())
               );
         GPSTK_THROW(me);
      }
      if(r.rows() <= 0) return;
      R = r;
      Z = z;
      names = nl;
   }

   // --------------------------------------------------------------------------------
   // copy constructor
   SRI::SRI(const SRI& s)
      throw()
   {
      R = s.R;
      Z = s.Z;
      names = s.names;
   }

   // --------------------------------------------------------------------------------
   // operator=
   SRI& SRI::operator=(const SRI& right)
      throw()
   {
      R = right.R;
      Z = right.Z;
      names = right.names;
      return *this;
   }

   // ---------------------------------------------------------------------------
   // modify SRIs
   // --------------------------------------------------------------------------------
   // Permute the SRI elements to match the input Namelist, which may differ with
   // the SRI Namelist by AT MOST A PERMUTATION, throw if this is not true.
   void SRI::permute(const Namelist& nl)
      throw(MatrixException,VectorException)
   {
      if(identical(names,nl)) return;
      if(names != nl) {
         MatrixException me("Invalid input: Namelists must be == to permute");
         GPSTK_THROW(me);
      }

      try {
         unsigned int i,j;
         // build a permutation matrix
         Matrix<double> P(R.rows(),R.rows(),0.0);
         for(i=0; i<R.rows(); i++) {
            j = nl.index(names.getName(i));
            P(j,i) = 1;
         }

         Matrix<double> B;
         Vector<double> Q;
         B = P * R * transpose(P);
         Q = P * Z;

         // re-triangularize
         R = 0.0;
         Z = 0.0;
         SrifMU(R,Z,B,Q);
         names = nl;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // Split this SRI (call it S) into two others, S1 and Sleft, where S1 has
   // a Namelist identical to the input Namelist (NL); set *this = S1 at the
   // end. NL must be a non-empty subset of names, and (names ^ NL) also must
   // be non-empty; throw MatrixException if this is not true. The second
   // output SRI, Sleft, will have the same names as S, but perhaps permuted.
   //
   // The routine works by first permuting S so that its Namelist if of the
   // form {N2,NL}, where N2 = (names ^ NL); this is possible only if NL is
   // a non-trivial subset of names. Then, the rows of S (rows of R and elements
   // of Z) naturally separate into the two component SRIs, with zeros in the
   // elements of the first SRI which correspond to N2, and those in Sleft
   // which correspond to NL.
   //
   //    Example:    S.name = A B C D E F G and NL = D E F G.
   // (Obviously, S may be permuted into such an order whenever this is needed.)
   // Note that here the R,Z pair is written in a format reminiscent of the
   // set of equations implied by R*X=Z, i.e. 1A+2B+3C+4D+5E+6F+7G=a, etc.
   //
   //          S (R Z)       =         S1            +         Sleft
   // with    names                       NL                  names
   //     A B C D E F G           . . . D E F G           A B C D E F G   
   //     - - - - - - -  -        - - - - - - -  -        - - - - - - -  -
   //     1 2 3 4 5 6 7  a   =    . . . . . . .  .   +    1 2 3 4 5 6 7  a
   //       8 9 1 2 3 4  b          . . . . . .  .          8 9 1 2 3 4  b
   //         5 6 7 8 9  c            . . . . .  .            5 6 7 8 9  c
   //           1 2 3 4  d              1 2 3 4  d              . . . .  d
   //             5 6 7  e                5 6 7  e                . . .  e
   //               8 9  f                  8 9  f                  . .  f
   //                 1  g                    1  g                    .  g
   //
   // where "." denotes a zero.  The split is simply separating the linear
   // equations which make up R*X=Z into two groups; because of the ordering,
   // one of the groups of equations (S1) depends only on a particular subset
   // of the elements of the state vector, i.e. the elements labelled by the
   // Namelist NL.
   //
   // The equation shown here is an information equation; if the two SRIs S1
   // and Sleft were merged again, none of the information would be lost.
   // Note that S1 has no dependence on A B C (hence the .'s), and therefore
   // its size can be reduced. However S2 still depends on the full names
   // Namelist. Sleft is necessarily singular, but S1 is not.
   //
   // Note that the SRI contains information about both the solution and
   // the covariance, i.e. state and noise, and therefore one must be very careful
   // in interpreting the results of split and merge (operator+=). [Be especially
   // careful about the idea that a merge might be reversible with a split() or
   // vice-versa - strictly this is never possible unless the Namelists are
   // mutually exclusive - two separate problems.]
   //
   // For example, suppose two different SRI's, which have some elements in common,
   // are merged. The combined SRI will have more information (it can't have less)
   // about the common elements, and therefore the solution will be 'better'
   // (assuming the underlying model equations for those elements are identical).
   // However the noises will also be combined, and the results you get might be
   // surprising. Also, note that if you then split the combined SRI again, the
   // solution won't change but the noises will be very different; in particular
   // the new split part will take all the information with it, so the common states
   // will have lower noise than they did in the original SRI.
   // See the test program tsri.cpp
   //
   void SRI::split(const Namelist& NL, SRI& Sleft)
      throw(MatrixException,VectorException)
   {
      try {
         Sleft = SRI(0);
         unsigned int n,m;
         n = NL.size();
         m = names.size();
         if(n >= m) {
            MatrixException me("split: Input Namelist must be a subset of this one");
            GPSTK_THROW(me);
         }

         unsigned int i,j;
            // copy names and permute it so that its end matches NL 
         Namelist N0(names);
         for(i=1; i<=n; i++) {           // loop (backwards) over names in NL
            for(j=1; j<=m; j++) {        // search (backwards) in NO for a match
               if(NL.labels[n-i] == N0.labels[m-j]) {  // if found a match
                  N0.swap(m-i,m-j);      // then move matching name to end
                  break;                 // and go on to next name in NL
               }
            }
            if(j > m) {
               MatrixException me("split: Input Namelist is not non-trivial subset");
               GPSTK_THROW(me);
            }
         }

            // copy *this into Sleft, then do the permutation
         Sleft = *this;
         Sleft.permute(N0);

            // copy parts of Sleft into S1, and then zero out those parts of Sleft
         SRI S1(NL);
         S1.R = Matrix<double>(Sleft.R,m-n,m-n,n,n);
         //S1.Z = Vector<double>(Sleft.Z,m-n,n);
         S1.Z.resize(n);
         for(i=0; i<n; i++) S1.Z(i) = Sleft.Z(m-n+i);
         for(i=m-n; i<m; i++) Sleft.zeroOne(i);

         *this = S1;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // extend this SRI to include the given Namelist, with no added information;
   // names in the input namelist which are not unique are ignored.
   SRI& SRI::operator+=(const Namelist& NL)
      throw(MatrixException,VectorException)
   {
      try {
         Namelist B(names);
            // NB assume that Namelist::operator|=() adds at the _end_
            // NB if there are duplicate names, |= will not add them
         B |= NL;
            // NB assume that this zeros A.R and A.Z
         SRI A(B);
            // should do this with slices..
            // copy into the new SRI
         for(unsigned int i=0; i<R.rows(); i++) {
            A.Z(i) = Z(i);
            for(unsigned int j=0; j<R.cols(); j++) A.R(i,j) = R(i,j);
         }
         *this = A;
         return *this;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // reshape this SRI to match the input Namelist, by calling other member
   // functions, including split(), operator+() and permute()
   // Given this SRI and a new Namelist NL, if NL does not match names,
   // transform names to match it, using (1) drop elements (this is probably
   // optional - you can always keep 'dead' elements), (2) add new elements
   // (with zero information), and (3) permute to match NL.
   void SRI::reshape(const Namelist& NL)
      throw(MatrixException,VectorException)
   {
      try {
         if(names == NL) return;
         Namelist keep(names);
         keep &= NL;                // keep only those in both names and NL
         //Namelist drop(names);    // (drop is unneeded - split would do it)
         //drop ^= keep;            // lose those in names but not in keep
         Namelist add(NL);
         add ^= keep;               // add those in NL but not in keep
         SRI Sdrop;                 // make a new SRI to hold the losers
         // would like to allow caller access to Sdrop..
         split(keep,Sdrop);         // split off only the losers
                                    // NB names = drop | keep; drop & keep is empty
         *this += add;              // add the new ones
         this->permute(NL);         // permute it to match NL
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // append an SRI onto this SRI. Similar to opertor+= but simpler; input SRI is
   // simply appended, first using operator+=(Namelist), then filling the new portions
   // of R and Z, all without final Householder transformation of result.
   // Do not allow a name that is already present to be added: throw.
   SRI& SRI::append(const SRI& S)
      throw(MatrixException,VectorException)
   {
      try {
         // do not allow duplicates
         if((names & S.names).size() > 0) {
            Exception e("Cannot append duplicate names");
            GPSTK_THROW(e);
         }

         // append to names at the end, and to R Z, zero filling
         const size_t I(names.size());
         *this += S.names;

         // just in case...to avoid overflow in loop below
         if(I+S.names.size() != names.size()) {
            Exception e("Append failed");
            GPSTK_THROW(e);
         }

         // loop over new names, copying data from input into the new SRI
         for(size_t i=0; i<S.names.size(); i++) {
            Z(I+i) = S.Z(i);
            for(size_t j=0; j<S.names.size(); j++)
               R(I+i,I+j) = S.R(i,j);
         }

         return *this;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // merge this SRI with the given input SRI. ? should this be operator&=() ?
   // NB may reorder the names in the resulting Namelist.
   SRI& SRI::operator+=(const SRI& S)
      throw(MatrixException,VectorException)
   {
      try {
         Namelist all(names);
         all |= S.names;      // assumes Namelist::op|= adds unique S.names to _end_

         //all.sort();        // TEMP - for testing with old version

            // stack the (R|Z)'s from both in one matrix;
            // all determines the columns, plus last column is for Z
         unsigned int i,j,n,m,sm;
         n = all.labels.size();
         m = R.rows();
         sm = S.R.rows();
         Matrix<double> A(m+sm,n+1,0.0);

            // copy R into A, permuting columns as names differs from all
            // loop over columns of R; do Z at the same time using j=row
         for(j=0; j<m; j++) {
               // find where this column of R goes in A
               // (should never throw..)
            int k = all.index(names.labels[j]);
            if(k == -1) {
               MatrixException me("Algorithm error 1");
               GPSTK_THROW(me);
            }

               // copy this col of R into A (R is UT)
            for(i=0; i<=j; i++) A(i,k) = R(i,j);
               // also the jth element of Z
            A(j,n) = Z(j);
         }

            // now do the same for S, but put S.R|S.Z below R|Z
         for(j=0; j<sm; j++) {
            int k = all.index(S.names.labels[j]);
            if(k == -1) {
               MatrixException me("Algorithm error 2");
               GPSTK_THROW(me);
            }
            for(i=0; i<=j; i++) A(m+i,k) = S.R(i,j);
            A(m+j,n) = S.Z(j);
         }
            // now triangularize A and pull out the new R and Z
         Householder<double> HA;
         HA(A);
         // submatrix args are matrix,toprow,topcol,numrows,numcols
         R = Matrix<double>(HA.A,0,0,n,n);
         Z = Vector<double>(HA.A.colCopy(n));
         Z.resize(n);
         names = all;

         return *this;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // merge two SRIs to produce a third. ? should this be operator&() ?
   SRI operator+(const SRI& Sleft,
                 const SRI& Sright)
      throw(MatrixException,VectorException)
   {
      try {
         SRI S(Sleft);
         S += Sright;
         return S;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // Zero out the nth row of R and the nth element of Z, removing all
   // information about that element.
   void SRI::zeroOne(const unsigned int n)
      throw()
   {
      if(n >= R.rows())
         return;

      //TD this is not right -- you should permute the element
      //to the first row, then zero
      for(unsigned int j=n; j<R.cols(); j++) 
         R(n,j) = 0.0;
      Z(n) = 0.0;
   }

   // --------------------------------------------------------------------------------
   // Zero out all the first n rows of R and elements of Z, removing all
   // information about those elements. Default value of the input is 0,
   // meaning zero out the entire SRI.
   void SRI::zeroAll(const unsigned int n)
      throw()
   {
      if(n <= 0) {
         R = 0.0;
         Z = 0.0;
         return;
      }

      if(n >= int(R.rows()))
         return;

      for(unsigned int i=0; i<n; i++) {
         for(unsigned int j=i; j<R.cols(); j++) 
            R(i,j) = 0.0;
         Z(i) = 0.0;
      }
   }

   // --------------------------------------------------------------------------------
   // Shift the state vector by a constant vector X0; does not change information
   // i.e. let R * X = Z => R * (X-X0) = Z'
   // throw on invalid input dimension
   void SRI::shift(const Vector<double>& X0)
      throw(MatrixException)
   {
      if(X0.size() != R.cols()) {
         MatrixException me("Invalid input dimension: SRI has dimension "
               + asString<int>(R.rows()) + " while input has length "
               + asString<int>(X0.size())
               );
         GPSTK_THROW(me);
      }
      Z = Z - R * X0;
   }

   // --------------------------------------------------------------------------------
   // Shift the SRI state vector (Z) by a constant vector Z0;
   // does not change information. i.e. let Z => Z-Z0
   // throw on invalid input dimension
   void SRI::shiftZ(const Vector<double>& Z0)
      throw(MatrixException)
   {
      if(Z0.size() != R.cols()) {
         MatrixException me("Invalid input dimension: SRI has dimension "
               + asString<int>(R.rows()) + " while input has length "
               + asString<int>(Z0.size())
               );
         GPSTK_THROW(me);
      }
      Z = Z - Z0;
   }

   // --------------------------------------------------------------------------------
   // Transform this SRI with the transformation matrix T;
   // i.e. R -> T * R * inverse(T) and Z -> T * Z. The matrix inverse(T)
   // may optionally be supplied as input, otherwise it is computed from
   // T. NB names in this SRI are most likely changed; but this routine does
   // not change the Namelist. Throw MatrixException if the input has
   // the wrong dimension or cannot be inverted.
   void SRI::transform(const Matrix<double>& T,
                       const Matrix<double>& invT)
      throw(MatrixException,VectorException)
   {
      if(T.rows() != R.rows() ||
         T.cols() != R.cols() ||
         (&invT != &SRINullMatrix && (invT.rows() != R.rows() ||
         invT.cols() != R.cols()))) {
            MatrixException me("Invalid input dimension:\n  SRI has dimension "
               + asString<int>(R.rows()) + " while T has dimension "
               + asString<int>(T.rows()) + "x"
               + asString<int>(T.cols()));
            if(&invT != &SRINullMatrix) me.addText("\n  and invT has dimension "
                           + asString<int>(invT.rows()) + "x"
                           + asString<int>(invT.cols()));
            GPSTK_THROW(me);
      }

      try {
            // get the inverse matrix
         Matrix<double> Ti(T);
         if(&invT == &SRINullMatrix)
            Ti = inverseSVD(T);
         else
            Ti = invT;

            // transform
         Matrix<double> B = T * R * Ti;
         Vector<double> Q = T * Z;

            // re-triangularize
         R = 0.0;
         Z = 0.0;
         SrifMU(R,Z,B,Q);
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // Transform the state by the transformation matrix T; i.e. X -> T*X,
   // without transforming the SRI; this is done by right multiplying R by
   // inverse(T), which is the input. Thus R -> R*inverse(T),
   // so R*inverse(T)*T*X = Z.  Input is the _inverse_ of the transformation.
   // throw MatrixException if input dimensions are wrong.
   void SRI::transformState(const Matrix<double>& invT)
      throw(MatrixException)
   {
      if(invT.rows() != R.rows() || invT.cols() != R.rows()) {
         MatrixException me("Invalid input dimension: SRI has dimension "
            + asString<int>(R.rows()) + " while invT has dimension "
            + asString<int>(invT.rows()) + "x"
            + asString<int>(invT.cols()));
         GPSTK_THROW(me);
      }

         // transform
      Matrix<double> A = R * invT;
         // re-triangularize
      Householder<double> HA;
      HA(A);
      R = HA.A;
   }

   // --------------------------------------------------------------------------------
   // Decrease the information in this SRI for, or 'Q bump', the element
   // with the input index.  This means that the uncertainty and the state
   // element given by the index are divided by the input factor q; the
   // default input is zero, which means zero out the information (q = infinite).
   // A Q bump by factor q is equivalent to 'de-weighting' the element by q.
   // No effect if input index is out of range.
   //
   // Use a specialized form of the time update, with Phi=unity, G(N x 1) = 0
   // except 1 for the element (in) getting bumped, and Rw(1 x 1) = 1 / q.
   // Note that this bump of the covariance for element k results in
   // Cov(k,k) += q (plus, not times!).
   // if q is 0, replace q with 1/q, i.e. lose all information, covariance
   // goes singular; this is equivalent to (1) permute so that the 'in'
   // element is first, (2) zero out the first row of R and the first element
   // of Z, (3) permute the first row back to in.
   void SRI::Qbump(const unsigned int& in,
                   const double& q)
      throw(MatrixException,VectorException)
   {
      try {
         if(in >= R.rows()) return;
         double factor=0.0;
         if(q != 0.0) factor=1.0/q;

         unsigned int ns=1,i,j,n=R.rows();

         Matrix<double> A(n+ns,n+ns+1,0.0), G(n,ns,0.0);
         A(0,0) = factor;           // Rw, dimension ns x ns = 1 x 1
         G(in,0) = 1.0;
         G = R * G;                 // R*Phi*G
         for(i=0; i<n; i++) {
            A(ns+i,0) = -G(i,0);               //     A =   Rw       0       zw=0
            for(j=0; j<n; j++)                 //          -R*Phi*G  R*Phi   Z
               if(i<=j) A(ns+i,ns+j) = R(i,j); //
            A(ns+i,ns+n) = Z(i);
         }

            // triangularize and pull out the new R and Z
         Householder<double> HA;                //    A  =  Rw  Rwx  zw
         HA(A);                                 //          0    R   z
         R = Matrix<double>(HA.A,ns,ns,n,n);
         Vector<double> T=HA.A.colCopy(ns+n);
         Z = Vector<double>(T,ns,n);
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // Fix the state element with the input index to the input value, and
   // collapse the SRI by removing that element.
   // No effect if index is out of range.
   void SRI::stateFix(const unsigned int& in,
                      const double& bias)
      throw(MatrixException,VectorException)
   {
      if(in >= R.rows()) return;

      try {
         unsigned int i,j,ii,jj,n=R.rows();
         Vector<double> Znew(n-1,0.0);
         Matrix<double> Rnew(n-1,n-1,0.0);
            // move the X(in) terms to the data vector on the RHS
         for(i=0; i<in; i++) Z(i) -= R(i,in)*bias;
            // remove row/col in and collapse
         for(i=0,ii=0; i<n; i++) {
            if(i == in) continue;
            Znew(ii) = Z(i);
            for(j=i,jj=ii; j<n; j++) {
               if(j == in) continue;
               Rnew(ii,jj) = R(i,j);
               jj++;
            }
            ii++;
         }
         R = Rnew;
         Z = Znew;
         names -= names.labels[in];
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   // --------------------------------------------------------------------------------
   // Vector version of stateFix with several states given in a Namelist.
   void SRI::stateFix(const Namelist& dropNL, const Vector<double>& values_in)
      throw(MatrixException,VectorException)
   {
      try {
         if(dropNL.size() != values_in.size()) {
            VectorException e("Input has inconsistent lengths");
            GPSTK_THROW(e);
         }
/*
         // build a vector of indexes to keep
         int i,j;
         vector<int> indexes;
         for(i=0; i<names.size(); i++) {
            j = dropNL.index(names.getName(i)); // index in dropNL, this state
            if(j == -1) indexes.push_back(i);// not found in dropNL, so keep
         }

         const int n=indexes.size();         // new dimension
         if(n == 0) {
            Exception e("Cannot drop all states");
            GPSTK_THROW(e);
         }

         Vector<double> X,newX(n);
         Matrix<double> C,newC(n,n);
         Namelist newNL;

         double big,small;
         getStateAndCovariance(X,C,&small,&big);

         for(i=0; i<n; i++) {
            newX(i) = X(indexes[i]);
            for(j=0; j<n; j++) newC(i,j) = C(indexes[i],indexes[j]);
            newNL += names.getName(indexes[i]);
         }

         R = Matrix<double>(inverseUT(upperCholesky(newC)));
         Z = Vector<double>(R*newX);
         names = newNL;
*/
         size_t i,j,k;
            // create a vector of indexes and corresponding values
         vector<int> indx;
         vector<double> value;
         for(i=0; i<dropNL.size(); i++) {
            int in = names.index(dropNL.getName(i));   // in must be allowed to be -1
            if(in > -1) {
               indx.push_back(in);
               value.push_back(values_in(i));
            }
            //else nothing happens
         }
         const unsigned int m = indx.size();
         const unsigned int n = R.rows();
         if(m == 0) return;
         if(m == n) {
            *this = SRI(0);
            return;
         }
            // move the X(in) terms to the data vector on the RHS
         for(k=0; k<m; k++)
            for(i=0; i<indx[k]; i++)
               Z(i) -= R(i,indx[k])*value[k];

            // first remove the rows in indx
         bool skip;
         Vector<double> Ztmp(n-m,0.0);
         Matrix<double> Rtmp(n-m,n,0.0);
         for(i=0,k=0; i<n; i++) {
            skip = false;
            for(j=0; j<m; j++) if((int)i == indx[j]) { skip=true; break; }
            if(skip) continue;      // skip row to be dropped

            Ztmp(k) = Z(i);
            for(j=i; j<n; j++) Rtmp(k,j) = R(i,j);
            k++;
         }

            // Z is now done
         Z = Ztmp;

            // now remove columns in indx
         R = Matrix<double>(n-m,n-m,0.0);
         for(j=0,k=0; j<n; j++) {
            skip = false;
            for(i=0; i<m; i++) if((int)j == indx[i]) { skip=true; break; }
            if(skip) continue;      // skip col to be dropped

            for(i=0; i<=j; i++) R(i,k) = Rtmp(i,j);
            k++;
         }

            // remove the names
         for(k=0; k<dropNL.size(); k++) {
            std::string name(dropNL.getName(k));
            names -= name;
         }
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   //---------------------------------------------------------------------------------
   // Add a priori or 'constraint' information
   // Prefer addAPrioriInformation(inverse(Cov), inverse(Cov)*X);
   void SRI::addAPriori(const Matrix<double>& Cov, const Vector<double>& X)
      throw(MatrixException)
   {
      if(Cov.rows() != Cov.cols() || Cov.rows() != R.rows() || X.size() != R.rows()) {
         MatrixException me("Invalid input dimensions:\n  SRI has dimension "
            + asString<int>(R.rows()) + ",\n  while input is Cov("
            + asString<int>(Cov.rows()) + "x"
            + asString<int>(Cov.cols()) + ") and X("
            + asString<int>(X.size()) + ")."
            );
         GPSTK_THROW(me);
      }

      try {
         Matrix<double> InvCov = inverseLUD(Cov);
         addAPrioriInformation(InvCov, X);
      }
      catch(MatrixException& me) {
         GPSTK_THROW(me);
      }
   }

   // --------------------------------------------------------------------------------
   void SRI::addAPrioriInformation(const Matrix<double>& InvCov,
                                   const Vector<double>& X)
      throw(MatrixException)
   {
      if(InvCov.rows() != InvCov.cols() || InvCov.rows() != R.rows()
            || X.size() != R.rows()) {
         MatrixException me("Invalid input dimensions:\n  SRI has dimension "
            + asString<int>(R.rows()) + ",\n  while input is InvCov("
            + asString<int>(InvCov.rows()) + "x"
            + asString<int>(InvCov.cols()) + ") and X("
            + asString<int>(X.size()) + ")."
            );
         GPSTK_THROW(me);
      }

      try {
         Matrix<double> L(lowerCholesky(InvCov));
         Matrix<double> apR(transpose(L));     // R = UT(inv(Cov))
         Vector<double> apZ(apR*X);            // Z = R*X
         SrifMU(R, Z, apR, apZ);
      }
      catch(MatrixException& me) {
         GPSTK_THROW(me);
      }
   }

   // --------------------------------------------------------------------------------
   void SRI::getConditionNumber(double& small, double& big)
      throw(MatrixException)
   {
      try {
         small = big = 0.0;
         const int n=R.rows();
         if(n == 0) return;
         SVD<double> svd;
         svd(R);
         svd.sort(true);   // now the last s.v. is the smallest
         small = svd.S(n-1);
         big = svd.S(0);
      }
      catch(MatrixException& me) {
         me.addText("Called by getConditionNumber");
         GPSTK_RETHROW(me);
      }
   }

   // --------------------------------------------------------------------------------
   // Compute state without computing covariance. Use the fact that R is upper
   // triangular. Throw if and when a zero diagonal element is found; values at larger
   // index are still valid. On output *ptr is the largest singular index
   void SRI::getState(Vector<double>& X, int *ptr)
      throw(MatrixException)
   {
      const int n=Z.size();
      X = Vector<double>(n,0.0);
      if(ptr) *ptr = -1;
      if(n == 0) return;
      int i,j;
      for(i=n-1; i>=0; i--) {             // loop over rows, in reverse order
         if(R(i,i) == 0.0) {
            if(ptr) *ptr = i;
            MatrixException me("Singular matrix; zero diagonal element at index "
               + asString<int>(i));
            GPSTK_THROW(me);
         }
         double sum=Z(i);
         for(j=i+1; j<n; j++)             // sum over columns to right of diagonal
            sum -= R(i,j)*X(j);
         X(i) = sum/R(i,i);
      }
   }

   // --------------------------------------------------------------------------------
   // get the state X and the covariance matrix C of the state, where
   // C = transpose(inverse(R))*inverse(R) and X = inverse(R) * Z.
   // Throws MatrixException if R is singular.
   void SRI::getStateAndCovariance(Vector<double>& X,
                                   Matrix<double>& C,
                                   double *ptrSmall,
                                   double *ptrBig)
      throw(MatrixException,VectorException)
   {
      try {
         double small,big;
         Matrix<double> invR(inverseUT(R,&small,&big));
         if(ptrSmall) *ptrSmall = small;
         if(ptrBig) *ptrBig = big;

         //cout << " small is " << scientific << setprecision(3) << small
         //   << " and big is " << big;
         //cout << " exponent is " << ::log(big) - ::log(small) << endl;
         // how best to test?
         //  ::log(big) - ::log(small) + 1 >= numeric_limits<double>::max_exponent
         if(small <= 10*numeric_limits<double>::epsilon()) {
            MatrixException me("Singular matrix: condition number is "
                  + asString<double>(big) + " / " + asString<double>(small));
            GPSTK_THROW(me);
         }

         C = UTtimesTranspose(invR);
         X = invR * Z;
      }
      catch(MatrixException& me) {
         GPSTK_RETHROW(me);
      }
      catch(VectorException& ve) {
         GPSTK_RETHROW(ve);
      }
   }

   //---------------------------------------------------------------------------------
   // output operator
   ostream& operator<<(ostream& os, const SRI& S)
   {
      Namelist NLR(S.names);
      Namelist NLC(S.names);
      NLC += string("State");
      Matrix<double> A;
      A = S.R || S.Z;
      LabeledMatrix LM(NLR,NLC,A);

      ios_base::fmtflags flags = os.flags();
      if(flags & ios_base::scientific) LM.scientific();
      LM.setw(os.width());
      LM.setprecision(os.precision());
      //LM.message("NL");
      //LM.linetag("tag");

      os << LM;

      return os;
   }

} // end namespace gpstk

//------------------------------------------------------------------------------------
