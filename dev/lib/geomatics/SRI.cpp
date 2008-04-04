#pragma ident "$Id$"

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
// GPSTk
#include "SRI.hpp"
#include "StringUtils.hpp"

using namespace std;

namespace gpstk
{
using namespace StringUtils;

   // --------------------------------------------------------------------------------
   // used to mark optional input
   const Matrix<double> SRINullMatrix;

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
            MatrixException me("Input Namelist must be a subset of this one");
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
               MatrixException me("Input Namelist is not a non-trivial subset");
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
         unsigned int i,j,k,n,m,sm;
         n = all.labels.size();
         m = R.rows();
         sm = S.R.rows();
         Matrix<double> A(m+sm,n+1,0.0);

            // copy R into A, permuting columns as names differs from all
            // loop over columns of R; do Z at the same time using j=row
         for(j=0; j<m; j++) {
               // find where this column of R goes in A
               // (should never throw..)
            k = all.index(names.labels[j]);
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
            k = all.index(S.names.labels[j]);
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
         Vector<double> T = Vector<double>(HA.A.colCopy(n));
         Z = Vector<double>(T,0,n);
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
   void SRI::zeroAll(const int n)
      throw()
   {
      if(n <= 0) {
         R = 0.0;
         Z = 0.0;
         return;
      }

      if(n >= R.rows())
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
   void SRI::biasFix(const unsigned int& in,
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
   // Vector version of biasFix with several states given in a Namelist.
   void SRI::biasFix(const Namelist& drops, const Vector<double>& biases)
      throw(MatrixException,VectorException)
   {
      try {
         unsigned int i,j,k,ii,jj,n=R.rows();
            // create a vector of indexes and corresponding biases
         vector<int> indx;
         vector<double> bias;
         for(i=0; i<drops.size(); i++) {
            j = names.index(drops.getName(i));
            if(j > -1) {
               indx.push_back(j);
               bias.push_back(biases(i));
            }
         }
         const unsigned int m = indx.size();
         if(m == 0) return;
         if(m == n) {            // error?
            *this = SRI(0);
            return;
         }

         Vector<double> Znew(n-m,0.0);
         Matrix<double> Rnew(n-m,n-m,0.0);
            // move the X(in) terms to the data vector on the RHS
         for(k=0; k<m; k++)
            for(i=0; i<indx[k]; i++)
               Z(i) -= R(i,indx[k])*bias[k];
            // remove row/col in and collapse
         for(i=0,ii=0; i<n; i++) {
            for(k=0; k<m; k++)
               if(i == indx[k]) continue;
            Znew(ii) = Z(i);
            for(j=i,jj=ii; j<n; j++) {
               for(k=0; k<m; k++)
                  if(j == indx[k]) continue;
               Rnew(ii,jj) = R(i,j);
               jj++;
            }
            ii++;
         }
         R = Rnew;
         Z = Znew;
         for(k=0; k<m; k++)
            names -= names.labels[indx[k]];
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
         Matrix<double> InvCov = inverse(Cov);
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
         Cholesky<double> Ch;
         Ch(InvCov);
         Vector<double> apZ = Ch.U * X;      // R = UT(inv(Cov)) and z = R*X
         SrifMU(R, Z, Ch.U, apZ);
      }
      catch(MatrixException& me) {
         GPSTK_THROW(me);
      }
   }

   // --------------------------------------------------------------------------------
   // get the state X and the covariance matrix C of the state, where
   // C = transpose(inverse(R))*inverse(R) and X = inverse(R) * Z.
   // Throws MatrixException if R is singular.
   // NB this is the most efficient way to invert the SRI problem.
   void SRI::getStateAndCovariance(Vector<double>& X,
                                   Matrix<double>& C,
                                   double *ptrSmall,
                                   double *ptrBig)
      throw(MatrixException,VectorException)
   {
      try {
         Matrix<double> invR;
         invR = inverseUT(R,ptrSmall,ptrBig);
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
      Namelist NL(S.names);
      NL += string("State");
      Matrix<double> A;
      A = S.R || S.Z;
      LabelledMatrix LM(NL,A);

      LM.setw(os.width());
      LM.setprecision(os.precision());
      os << LM;
      return os;
   }

   //---------------------------------------------------------------------------------
   // This routine uses the Householder algorithm to update the SRI
   // state and covariance.
   // Input:
   //    R  a priori SRI matrix (upper triangular, dimension N)
   //    Z  a priori SRI data vector (length N)
   //    A  concatentation of H and D : A = H || D, where
   //    H  Measurement partials, an M by N matrix.
   //    D  Data vector, of length M
   //       H and D may have row dimension > M; then pass M:
   //    M  (optional) Row dimension of H and D
   // Output:
   //    Updated R and Z.  H is trashed, but the data vector D
   //    contains the residuals of fit (D - A*state).
   // Return values:
   //    SrifMU returns void, but throws exceptions if the input matrices
   // or vectors have incompatible dimensions.
   // 
   // Measurment noise associated with H and D must be white
   // with unit covariance.  If necessary, the data can be 'whitened'
   // before calling this routine in order to satisfy this requirement.
   // This is done as follows.  Compute the lower triangular square root 
   // of the covariance matrix, L, and replace H with inverse(L)*H and
   // D with inverse(L)*D.
   // 
   //    The Householder transformation is simply an orthogonal
   // transformation designed to make the elements below the diagonal
   // zero.  It works by explicitly performing the transformation, one
   // column at a time, without actually constructing the transformation
   // matrix.  Let y be column k of the input matrix.  y can be zeroed
   // below the diagonal as follows:  let sum=sign(y(k))*sqrt(y*y), and
   // define vector u(k)=y(k)+sum, u(j)=y(j) for j>k.  This defines the
   // transformation matrix as (1-bu*u), with b=2/u*u=1/sum*u(k).
   // Redefine y(k)=u(k) and apply the transformation to elements of the
   // input matrix below and to the right of the (k,k) element.  This 
   // algorithm for each column k=0,n-1 in turn is equivalent to a single
   // orthogonal transformation which triangularizes the matrix.
   //
   // Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
   //      Estimation," Academic Press, 1977.
   template <class T>
   void SrifMU(Matrix<T>& R,
               Vector<T>& Z,
               Matrix<T>& A,
               unsigned int M)
      throw(MatrixException)
   {
      if(A.cols() <= 1 || A.cols() != R.cols()+1 || Z.size() < R.rows()) {
         if(A.cols() > 1 && R.rows() == 0 && Z.size() == 0) {
            // create R and Z
            R = Matrix<double>(A.cols()-1,A.cols()-1,0.0);
            Z = Vector<double>(A.cols()-1,0.0);
         }
         else {
            MatrixException me("Invalid input dimensions:\n  R has dimension "
               + asString<int>(R.rows()) + "x"
               + asString<int>(R.cols()) + ",\n  Z has length "
               + asString<int>(Z.size()) + ",\n  and A has dimension "
               + asString<int>(A.rows()) + "x"
               + asString<int>(A.cols()));
            GPSTK_THROW(me);
         }
      }

      const T EPS=-T(1.e-200);
      unsigned int m=M, n=R.rows();
      if(m==0 || m > A.rows()) m=A.rows();
      unsigned int np1=n+1;         // if np1 = n, state vector Z is not updated
      unsigned int i,j,k;
      T dum, delta, beta;

      for(j=0; j<n; j++) {          // loop over columns
         T sum = T(0);
         for(i=0; i<m; i++)
            sum += A(i,j)*A(i,j);   // sum squares of elements in this column
         if(sum <= T(0)) continue;

         dum = R(j,j);
         sum += dum * dum;
         sum = (dum > T(0) ? -T(1) : T(1)) * ::sqrt(sum);
         delta = dum - sum;
         R(j,j) = sum;

         if(j+1 > np1) break;

         beta = sum*delta;
         if(beta > EPS) continue;
         beta = T(1)/beta;

         for(k=j+1; k<np1; k++) {   // columns to right of diagonal
            sum = delta * (k==n ? Z(j) : R(j,k));
            for(i=0; i<m; i++)
               sum += A(i,j) * A(i,k);
            if(sum == T(0)) continue;

            sum *= beta;
            if(k==n) Z(j) += sum*delta;
            else   R(j,k) += sum*delta;

            for(i=0; i<m; i++)
               A(i,k) += sum * A(i,j);
         }
      }
   }  // end SrifMU
    
   //---------------------------------------------------------------------------------
   // This is simply SrifMU(R,Z,A) with H and D passed in rather
   // than concatenated into a single Matrix A = H || D.
   template <class T>
   void SrifMU(Matrix<T>& R,
               Vector<T>& Z,
               Matrix<T>& H,
               Vector<T>& D,
               unsigned int M)
      throw(MatrixException)
   {
      Matrix<double> A;
      try { A = H || D; }
      catch(MatrixException& me) { GPSTK_RETHROW(me); }

      SrifMU(R,Z,A,M);

         // copy residuals out of A into D
      D = Vector<double>(A.colCopy(A.cols()-1));
   }

   //---------------------------------------------------------------------------------
   // Invert the upper triangular matrix stored in the square matrix UT, using a very
   // efficient algorithm. Throw MatrixException if the matrix is singular.
   // If the pointers are defined, on exit (but not if an exception is thrown),
   // they return the smallest and largest eigenvalues of the matrix.
   template <class T>
   Matrix<T> inverseUT(const Matrix<T>& UT,
                       T *ptrSmall,
                       T *ptrBig)
      throw(MatrixException)
   {
      if(UT.rows() != UT.cols() || UT.rows() == 0) {
         MatrixException me("Invalid input dimensions: "
               + asString<int>(UT.rows()) + "x"
               + asString<int>(UT.cols()));
         GPSTK_THROW(me);
      }

      unsigned int i,j,k,n=UT.rows();
      T big,small,sum,dum;
      Matrix<T> Inv(UT);

         // start at the last row,col
      dum = UT(n-1,n-1);
      if(dum == T(0))
         throw SingularMatrixException("Singular matrix");

      big = small = dum;
      Inv(n-1,n-1) = T(1)/dum;
      if(n == 1) return Inv;                 // 1x1 matrix
      for(i=0; i<n-1; i++) Inv(n-1,i)=0;

         // now move to rows i = n-2 to 0
      for(i=n-2; i>=0; i--) {
         if(UT(i,i) == T(0))
            throw SingularMatrixException("Singular matrix");

         if(fabs(UT(i,i)) > big) big = fabs(UT(i,i));
         if(fabs(UT(i,i)) < small) small = fabs(UT(i,i));
         dum = T(1)/UT(i,i);
         Inv(i,i) = dum;                        // diagonal element first

            // now do off-diagonal elements (i,i+1) to (i,n-1)
         for(j=i+1; j<n; j++) {
            sum = T(0);
            for(k=i+1; k<=j; k++)
               sum += Inv(k,j) * UT(i,k);
            Inv(i,j) = - sum * dum;
         }
         for(j=0; j<i; j++) Inv(i,j)=0;

         if(i==0) break;         // NB i is unsigned, hence 0-1 = 4294967295!
      }

      if(ptrSmall) *ptrSmall=small;
      if(ptrBig) *ptrBig=big;

      return Inv;
   }

   //---------------------------------------------------------------------------------
   // Given an upper triangular matrix UT, compute the symmetric matrix
   // UT * transpose(UT) using a very efficient algorithm.
   template <class T>
   Matrix<T> UTtimesTranspose(const Matrix<T>& UT)
      throw(MatrixException)
   {
      unsigned int n=UT.rows();
      if(n == 0 || UT.cols() != n) {
         MatrixException me("Invalid input dimensions: "
               + asString<int>(UT.rows()) + "x"
               + asString<int>(UT.cols()));
         GPSTK_THROW(me);
      }

      unsigned int i,j,k;
      T sum;
      Matrix<T> S(n,n);

      for(i=0; i<n-1; i++) {        // loop over rows of UT, except the last
         sum = T(0);                // diagonal element (i,i)
         for(j=i; j<n; j++)
            sum += UT(i,j)*UT(i,j);
         S(i,i) = sum;
         for(j=i+1; j<n; j++) {     // loop over columns to right of (i,i)
            sum = T(0);
            for(k=j; k<n; k++)
               sum += UT(i,k) * UT(j,k);
            S(i,j) = S(j,i) = sum;
         }
      }
      S(n-1,n-1) = UT(n-1,n-1)*UT(n-1,n-1);   // the last diagonal element

      return S;
   }

} // end namespace gpstk

//------------------------------------------------------------------------------------
