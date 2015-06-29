/// @file SparseVector.cpp  Class for a sparse vector.

#include "SparseVector.hpp"
namespace gpstk
{
   /// tolerance in considering element to be zero is std::abs(elem) < tolerance
   /// see zeroize(), where this is the default input value
   template <class T> const double SparseVector<T>::zeroTolerance=1.0e-14;
}
