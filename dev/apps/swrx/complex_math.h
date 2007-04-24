#pragma ident "$Id"
#ifndef COMPLEXMATH_HPP
#define COMPLEXMATH_HPP

#include <complex>

static inline std::complex<double> sincos(double radians) 
{
   std::complex<double> ret;
   sincos(radians,&(ret.imag()),&(ret.real()));
   return ret;
}

static inline std::complex<int> quantize(const std::complex<double> x)
{
   return std::complex<int>(
      static_cast<int>(::round(x.real())),
      static_cast<int>(::round(x.imag()))
      );
}

const std::complex<double> zero(0,0);
const std::complex<double> plusOne(1,0);
const std::complex<double> minusOne(-1,0);


//-----------------------------------------------------------------------------
// An function object that implements a second order filter for the PLL
// Specifically the filter in figure 8.2 of Tsui's book
//-----------------------------------------------------------------------------
template <class C>
class LoopFilter
{
public:
   LoopFilter() : c1(1), c2(1), z1(0) {};

   std::complex<C> operator()(std::complex<C> in) throw()
   {
      std::complex <C> out,z;
      z = in*c2+z1;
      out = in * c1 + z;
      z1 = z;
      return out;
   };

   C c1, c2;
   std::complex<C> z1;
};

#endif
