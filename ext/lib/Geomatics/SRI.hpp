//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file SRI.hpp
 * Include file defining class SRI.
 * class SRI implements the square root information methods, used for least squares
 * estimation and the SRI form of the Kalman filter.
 *
 * Reference: "Factorization Methods for Discrete Sequential Estimation,"
 *             by G.J. Bierman, Academic Press, 1977.
 */

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef CLASS_SQUAREROOTINFORMATION_INCLUDE
#define CLASS_SQUAREROOTINFORMATION_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <string>
// GPSTk
#include "Matrix.hpp"
#include "StringUtils.hpp"
// geomatics
#include "Namelist.hpp"
#include "SRIMatrix.hpp"
#include "SparseMatrix.hpp"

namespace gpstk
{

//------------------------------------------------------------------------------------
/// constant (empty) Matrix used for default input arguments
extern const Matrix<double> SRINullMatrix;
/// constant (empty) SparseMatrix used for default input arguments
extern const SparseMatrix<double> SRINullSparseMatrix;

//------------------------------------------------------------------------------------
/// class SRI encapsulates all the information associated with the solution of a set
/// of simultaneous linear equations. It is used in least squares estimation (linear
/// and linearized) and is the basis of the preferred implementation of Kalman
/// filtering. An SRI consists of just three things:
/// (1) 'R', the 'information matrix', which is an upper triangular matrix of
/// dimension N, equal to the inverse of the square root (or Cholesky decomposition)
/// of the solution covariance matrix,
/// (2) 'Z', the 'SRI state vector' of length N (parallels the components of R),
/// (not to be confused with the regular state vector X), and
/// (3) 'names', a Namelist used to label the elements of R and Z (parallels and
/// labels rows and columns of R and elements of Z). A Namelist is part of class SRI
/// because the manipulations of SRI (see functions below) requires a consistent way
/// of manipulating the different individual elements of R and Z, in addition it
/// allows the user to attach 'human-readable' labels to the elements of the state
/// vector, which is useful in adding, dropping and bumping states, and it makes
/// printed results more readable (see the LabeledMatrix class in Namelist.hpp).
///
/// The set of simultaneous equations represented by an SRI is R * X = Z, where X is
/// the (unknown) state vector (the conventional solution vector) also of dimension N.
/// The state X is solved for as X = inverse(R) * Z, and the covariance matrix of the
/// state X is equal to transpose(inverse(R))*inverse(R).
///
/// Least squares estimation via SRI is very simple and efficient; it uses the
/// Householder transformation to convert the problem to upper triangular form, and
/// then uses very efficient algorithms to invert the information matrix to find the
/// solution and its covariance. The usual matrix equation is H * X = D,
/// where H is the 'design matrix' or the 'partials matrix', of dimension M x N,
/// X is the (unknown) solution vector of length N, and D is the 'data' or
/// 'measurement' vector of length M. In the least squares 'update' of the SRI,
/// this set of information {H,D} is concatenated with the existing SRI {R,Z} to
/// form an (N+M x N+1) matrix Q which has R in the upper left, Z upper right,
/// H lower left and D lower right. This extended matrix is then subjected to a
/// Householder transformation (see class Matrix), which will put (at least the
/// first N columns of) Q into upper triangular form. The result is a new, updated
/// SRI (R and Z) in the place of the old, while in place of D are residuals of fit
/// corresponding to the measurements in D (the H part of Q is trashed). This result,
/// in fact (see the reference), produces an updated SRI which gives precisely the
/// usual least squares solution for the combined 'a priori SRI + new data' problem.
/// This algorithm is called a 'measurement update' of the SRI.
///
/// It is most enlightening to think of the SRI and this process in terms of
/// 'information'. The SRI contains all the 'information' which has come from
/// updates that have been made to it using (H,D) pairs. Initially, the SRI is all
/// zeros, which corresponds to 'no information'. This overcomes one serious problem
/// with conventional least squares and the Kalman algorithm, namely that a
/// 'zero information' starting value cannot be correctly expressed, because in that
/// case the covariance matrix is singular and the state vector is indeterminate;
/// in the SRI method this is perfectly consistent - the covariance matrix is
/// singular because the information matrix (R) is zero, and thus the state
/// is entirely indeterminate. As new 'information' (in the form of data D and
/// partials matrix H pairs) is added to the SRI (via the Householder algorithm),
/// the 'information' stored in R and Z is increased and they become non-zero.
/// (By the way note that the number of rows in the {H,D} information is arbitrary -
/// information can be added in 'batches' - M large - or one - M=1 - piece at a time.)
/// When there is enough information, R becomes non-singular, and so can be inverted
/// and the solution and covariance can be computed. As the amount of information
/// becomes large, elements of R become large, and thus elements of the covariance
/// (think of covariance as a measure of uncertainty - the opposite or inverse of
/// information) become small.
///
/// The structure of the SRI method allows some powerful techniques to be used in 
/// manipulating, combining and separating state elements and the information
/// associated with them in SRIs. For example, if the measurement updates have
/// failed to increase the information about one particular state element, then
/// that element, and its information, may be removed from the problem by deleting
/// that element's row and column of R, and its element of Z (and then
/// re-triangularizing the SRI). In general, any subset of an SRI may be separated,
/// or the SRI split (see the routine of that name below - note the caveats) into
/// two separate SRIs. For another example, SRI allows the information of a each
/// state element to be selectively reduced or even zeroed, simply by multiplying
/// the corresponding elements of R and Z by a factor; in Kalman filtering this
/// is called a 'Q bump' of the element and is very important in some filtering
/// applications. There are methods (see below) consistently to merge (operator+()),
/// split, and permute elements of, SRIs.
///
/// Kalman filtering is an important application of SRI methods (actually it is
/// called 'square root information filtering' or SRIF - technically the term
/// 'Kalman filter algorithm' is reserved for the classical algorithm just as
/// Kalman presented it, in terms of a state vector and its covariance matrix).
/// The measurment update described above (which is actually just linear least
/// squares) is half of the SRIF (Kalman filter) - there is a 'time update' that
/// propagates the SRI (and thus the state and covariance) forward in time using
/// the dynamical model of the filter. These are algebraically equivalent to the
/// classical Kalman algorithm, but are more efficient and numerically stable
/// (actually the Kalman algorithm has been shown to be numerically unstable!).
/// There are even SRI smoothing algorithms, corresponding to Kalman smoothers,
/// which consist of a 'backwards' filter, implemented by applying a
/// 'smoother update' to the SRI at each point in reverse order.
///
/// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential Estimation,"
///      Academic Press, 1977.
class SRI {
public:
      /// empty constructor
   SRI(void) throw() { }

      /// constructor given the dimension N.
      /// @param N the dimension to assign: R(N,N) Z(N) names(N)
   SRI(const unsigned int N)
      throw();

      /// constructor given a Namelist, its dimension determines the SRI dimension.
      /// @param NL Namelist to give the SRI; this sets the dimension
   SRI(const Namelist& NL)
      throw();

      /// explicit constructor - throw if the dimensions are inconsistent.
      /// User is responsible for ensuring the input is self-consistent.
      /// @param R upper triangular R matrix
      /// @param Z SRI state vector
      /// @param NL namelist to give the SRI
      /// @throw MatrixException if dimensions are not consistent
   SRI(const Matrix<double>& R, const Vector<double>& Z, const Namelist& NL);

      /// explicit constructor from covariance and state
      /// User is responsible for ensuring the input is self-consistent.
      /// @param Cov covariance matrix
      /// @param State state vector
      /// @param NL namelist to give the SRI
      /// @throw MatrixException if dimensions are not consistent
   void setFromCovState(const Matrix<double>& Cov,
                        const Vector<double>& State,
                        const Namelist& NL);

      /// copy constructor
   SRI(const SRI&)
      throw();

      /// operator=
   SRI& operator=(const SRI& right)
      throw();

   // modify SRIs

      /// Permute the SRI elements to match the input Namelist, which may differ with
      /// the SRI Namelist by AT MOST A PERMUTATION; throw if this is not true.
      /// Replaces names with NL.
      /// @param NL Namelist desired for output SRI, unchanged on output
      /// @throw SomeUnknownException if NL != names, i.e. NL is other than a permutation of this->names
      /// @throw MatrixException
      /// @throw VectorException
   void permute(const Namelist& NL);

      /// split an SRI into two others, this one matching the input Namelist, the
      /// other containing whatever is left. The input Namelist must be a non-trivial
      /// subset of this->names; throw MatrixException if it is not. NB. Interpreting
      /// the results of a split() and merge (operator+()) operations should be done
      /// very carefully; remember that the SRI contains both solution and noise,
      /// and that the results of these operations are not always as expected,
      /// particularly note that split() and operator+() are usually NOT reversible.
      /// NB output SRI S will be singular.
      /// @param NL new namelist to be given to this object
      /// @param S this SRI after the NL part has been removed
      /// @throw MatrixException
      /// @throw VectorException
   void split(const Namelist& NL, SRI& S);

      /// extend this SRI to include the given Namelist, with no added information;
      /// names in the input namelist which are not unique are ignored.
      /// @param NL namelist with which to extend this SRI.
      /// @throw MatrixException
      /// @throw VectorException
   SRI& operator+=(const Namelist& NL);

      /// reshape this SRI to match the input Namelist, by calling other member
      /// functions, including split(), operator+() and permute()
      /// @param NL namelist with which to reshape this SRI.
      /// @throw MatrixException
      /// @throw VectorException
   void reshape(const Namelist&);

      /// merge an SRI into this one. NB names may be reordered in the result.
      /// NB this is just operator+=()
      /// @param S SRI to be merged into this
      /// @throw MatrixException
      /// @throw VectorException
   void merge(const SRI& S)
   { *this += S; }

      /// merge this SRI with the given input SRI. ? should this be operator&=() ?
      /// NB may reorder the names in the resulting Namelist.
      /// @param S SRI to be merged into this
      /// @throw MatrixException
      /// @throw VectorException
   SRI& operator+=(const SRI& S);

      /// merge two SRIs to produce a third. ? should this be operator&() ?
      /// @param S1 first SRI to be merged
      /// @param S2 second SRI to be merged
      /// @throw MatrixException
      /// @throw VectorException
   friend SRI operator+(const SRI& S1, const SRI& S2);

      /// append an SRI onto this SRI. Similar to opertor+= but simpler; input SRI is
      /// simply appended, first using operator+=(Namelist), then filling the new
      /// portions of R and Z, all without final Householder transformation of result.
      /// Do not allow a name that is already present to be added: throw.
      /// @param S input SRI to be appended
      /// @return appended SRI
      /// @throw SomeUnknownException if a name is repeated
      /// @throw MatrixException
      /// @throw VectorException
   SRI& append(const SRI& S);

      /// Zero out the nth row of R and the nth element of Z, removing all
      /// information about that element.
      /// @param n index of row or R and element of Z to be zeroed
   void zeroOne(const unsigned int n)
      throw();

      /// Zero out all the first n rows of R and elements of Z, removing all
      /// information about those elements. Default value of the input is 0,
      /// meaning zero out the entire SRI.
      /// @param n last index of row or R and element of Z to be zeroed
   void zeroAll(const unsigned int n=0)
      throw();

      /// Zero out (set all elements to zero) the state (Vector Z) only.
   void zeroState(void)
      throw()
   { Z = 0.0; }

      /// Reset the SRI, meaning zero it and optionally change the dimension to n.
      /// @param n Dimension of the new object (optional).
   //void reset(int n=0)
   //   throw();

      /// Shift the state vector by a constant vector X0; does not change information
      /// i.e. let R * X = Z => R' * (X-X0) = Z'
      /// @param X0 vector by which to shift the state
      /// @throw MatrixException on invalid input dimension
   void shift(const Vector<double>& X0);

      /// Shift the SRI state vector (Z) by a constant vector Z0;
      /// does not change information. i.e. let Z => Z-Z0
      /// @param Z0 vector by which to shift the Z state
      /// @throw MatrixException on invalid input dimension
   void shiftZ(const Vector<double>& Z0);

      /// Retriangularize the SRI, when it has been modified to a non-UT
      /// matrix (e.g. by transform()). Given the matrix A=[R||Z], apply HH transforms
      /// to retriangularize it and pull out new R and Z.
      /// NB caller must modify names, if necessary
      /// @param A Matrix<double> which is [R || Z] to be retriangularizied.
      /// @throw MatrixException if dimensions are wrong.
   void retriangularize(const Matrix<double>& A);

      /// Retriangularize the SRI, that is assuming R has been modified to a non-UT
      /// matrix (e.g. by transform()). Given RR and ZZ, apply HH transforms to 
      /// retriangularize, and store as R,Z.
      /// NB caller must modify names, if necessary
      /// @param R Matrix<double> input the modified (non-UT) R
      /// @param Z Vector<double> input the (potentially) modified Z
      /// @throw MatrixException if dimensions are wrong.
   void retriangularize(Matrix<double> RR, Vector<double> ZZ);

      /// Transform the state by the transformation matrix T; i.e. X -> T*X;
      /// this is done by right multiplying R by inverse(T), which is the input.
      /// Thus R -> R*inverse(T), so Z -> R*inverse(T)*T*X = Z. [R|Z] -> [R*invT|Z].
      /// NB Input is the _inverse_ of the transformation.
      /// @param invT Matrix<double> inverse of the transformation T : X->T*X
      /// @param NL Namelist of the transformed SRI, SRI.names is set to this
      /// @throw MatrixException if input dimensions are wrong.
   void transform(const Matrix<double>& invT, const Namelist& NL);

      /// Decrease the information in this SRI for, or 'Q bump', the element
      /// with the input index.  This means that the uncertainty and the state
      /// element given by the index are divided by the input factor q; the
      /// default input is zero, which means zero out the information (q = infinite).
      /// A Q bump by factor q is equivalent to 'de-weighting' the element by q.
      /// No effect if in is out of range.
      /// @param in index to bump
      /// @param q factor by which to 'de-weight' the element(in), default 0.0 which
      ///    implies all information removed.
      /// @throw MatrixException
      /// @throw VectorException
   void Qbump(const unsigned int& in, const double& q=0.0);

      /// Fix one state element (with the given name) to a given value, and set the
      /// information for that element (== 1/sigma) to a given value.
      /// No effect if name is not found
      /// @param name string labeling the state in Namelist names
      /// @param value to which the state element is fixed
      /// @param sigma (1/information) assigned to the element
      /// @param restore if true, permute back to the original form after fixing
      /// @throw Exception
   void stateFix(const std::string& name,
                 const double& value, const double& sigma, bool restore);

      /// Fix one state element (at the given index) to a given value, and set the
      /// information for that element (== 1/sigma) to a given value.
      /// No effect if index is out of range.
      /// @param index of the element to fix
      /// @param value to which the state element is fixed
      /// @param sigma (1/information) assigned to the element
      /// @param restore if true, permute back to the original form after fixing
      /// @throw Exception
   void stateFix(const unsigned int& index,
                 const double& value, const double& sigma, bool restore);

      /// Fix the state element with the input index to the input value, and
      /// collapse the SRI by removing that element.
      /// No effect if index is out of range.
      /// @param index of the element to fix
      /// @param value to which the state element is fixed
      /// @throw MatrixException
      /// @throw VectorException
   void stateFixAndRemove(const unsigned int& index, const double& value);

      /// Vector version of stateFixAndRemove, with Namelist identifying the states.
      /// Fix the given state elements to the input value, and
      /// collapse the SRI by removing those elements.
      /// No effect if name is not found.
      /// @param drops Namelist of states to fix
      /// @param values vector parallel to drops of values to which elements are fixed
      /// @throw MatrixException
      /// @throw VectorException
   void stateFixAndRemove(const Namelist& drops, const Vector<double>& values);

      /// Add a priori or constraint information in the form of an ordinary
      /// state vector and covariance matrix. The matrix must be non-singular.
      /// @param Cov Covariance matrix of same dimension as this SRIFilter
      /// @param X   State vector of same dimension as this SRIFilter
      /// @throw MatrixException if input is invalid: dimensions are
      ///   wrong or Cov is singular.
   void addAPriori(const Matrix<double>& Cov, const Vector<double>& X);

      /// Add a priori or constraint information in the form of an information
      /// matrix (inverse covariance) and ordinary state. ICov must be non-singular.
      /// @param ICov Inverse covariance matrix of same dimension as this SRIFilter
      /// @param X    State vector of same dimension as this SRIFilter
      /// @throw MatrixException if input is invalid: dimensions are wrong.
   void addAPrioriInformation(const Matrix<double>& ICov, const Vector<double>& X);

      /// SRIF (Kalman) measurement update, or least squares update
      /// Call the SRI measurement update for this SRI and the given input. See doc.
      /// for SrifMU().
      /// @param Partials matrix
      /// @param Data vector
      /// @throw Exception
   void measurementUpdate(Matrix<double>& Partials, Vector<double>& Data)
   {
      try {
         SrifMU(R, Z, Partials, Data);
      }
      catch(Exception& me) { GPSTK_RETHROW(me); }
   }

      /// SRIF (Kalman) measurement update, or least squares update, Sparse version.
      /// Call the SRI measurement update for this SRI and the given input. See doc.
      /// for SrifMU().
      /// @param Partials matrix
      /// @param Data vector
      /// @throw Exception
   void measurementUpdate(SparseMatrix<double>& Partials, Vector<double>& Data)
   {
      try {
         SrifMU(R, Z, Partials, Data);
      }
      catch(MatrixException& me) { GPSTK_RETHROW(me); }
   }

      /// Compute the condition number, or rather the largest and smallest eigenvalues
      /// of the SRI matrix R (the condition number is the ratio of the largest and
      /// smallest eigenvalues). Note that the condition number of the covariance
      /// matrix would be the square of the condition number of R.
      /// @param small smallest eigenvalue
      /// @param big largest eigenvalue, condition = big/small
      /// @throw MatrixException
   void getConditionNumber(double& small, double& big) const;

      /// Compute the state X without computing the covariance matrix C.
      /// R*X=Z so X=inverse(R)*Z; in this routine the state is computed explicitly,
      /// without forming the inverse of R, using the fact that R is upper triangular.
      /// NB. The matrix is singular if and only if one or more of the diagonal
      /// elements is zero; in the case the routine will still have valid entries in
      /// the state vector for index greater than the largest index with zero diagonal
      /// @param X                 State vector (output)
      /// @param ptrSingularIndex  if ptr is non-null, on output *ptr will be the
      ///                           largest index of singularity
      /// @throw MatrixException if R is singular.
   void getState(Vector<double>& X, int *ptrSingularIndex=NULL) const;

      /// Compute the state X and the covariance matrix C of the state, where
      /// C = transpose(inverse(R))*inverse(R) and X = inverse(R) * Z.
      /// Optional pointer arguments will return smallest and largest
      /// eigenvalues of the R matrix, which is a measure of singularity.
      /// NB this is the most efficient way to invert the SRI equation.
      /// @param X State vector (output)
      /// @param C Covariance of the state vector (output)
      /// @param ptrSmall Pointer to double, on output *ptrSmall set to smallest
      ///                 eigenvalue of R
      /// @param ptrBig Pointer to double, on output *ptrBig set to largest
      ///                 eigenvalue of R
      /// @throw MatrixException if R is singular.
      /// @throw VectorException
   void getStateAndCovariance(Vector<double>& X,
                              Matrix<double>& C,
                              double *ptrSmall=NULL,
                              double *ptrBig=NULL) const;

      // member access
      /// @return the size of the SRI, which is the dimension of R(rows and columns),
      /// Z and names.
   unsigned int size(void) const
      throw()
   { return R.rows(); }

      /// @return a copy of the Namelist of the SRI
   Namelist getNames(void) const
      throw()
   { return names; }

      /// access the name of a specific state element, given its index.
      /// @return 'out-of-range' if the index is out of range.
   std::string getName(const unsigned int in) const
      throw()
   { return names.getName(in); }

      /// assign the name of a specific state element, given its index;
      /// no effect, and return false, if the name is not unique;
      /// @param in index of name to be set
      /// @param label - name at index in is set to this label
      /// @return true if successful.
   bool setName(const unsigned int in, const std::string& label)
      throw()
   { return names.setName(in,label); }

      /// @return the index of the name in the Namelist that matches the input, or
      /// -1 if not found.
   unsigned int index(std::string& name)
      throw()
   { return names.index(name); }

      /// @return copy of the R matrix
   Matrix<double> getR(void) const
      throw()
   { return R; }

      /// @return copy of the Z vector
   Vector<double> getZ(void) const
      throw()
   { return Z; }

      /// output operator
   friend std::ostream& operator<<(std::ostream& s, const SRI&);

protected:
   // member data
      /// Information matrix, an upper triangular (square) matrix
   Matrix<double> R;

      /// SRI state vector, of length equal to the dimension (row and col) of R.
   Vector<double> Z;

      /// Namelist parallel to R and Z, labelling the elements of the state vector.
   Namelist names;

}; // end class SRI

} // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
