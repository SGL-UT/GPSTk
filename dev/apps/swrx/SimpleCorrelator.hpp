#pragma ident "$Id$"

#ifndef SIMPLECORRELATOR_HPP
#define SIMPLECORRELATOR_HPP

#include <complex>
#include <queue>
#include <iostream>

//-----------------------------------------------------------------------------
// A correlator with a built in delay line to offset the incomming code.
//-----------------------------------------------------------------------------
template <class C>
class SimpleCorrelator
{
public:
   
   typedef std::complex<C> Ctype;

   /// param d this is the number of samples to delay the code by
   SimpleCorrelator(unsigned d=0) : delay(d+1), sum(0)
   {}
   
   inline void process(std::complex<C> in, Ctype code) throw()
   {
      shiftReg.push(code);
      sum += in * shiftReg.front();
      while (shiftReg.size() > delay)
         shiftReg.pop();
   }
   
   inline void dump() throw() {sum=Ctype(0,0);}

   inline Ctype operator()() const throw() {return sum;}

   void setDelay(unsigned d) throw() {delay=d+1;}
   unsigned getDelay() const throw() {return delay-1;}

private:
   unsigned delay;
   std::queue< Ctype > shiftReg;
   Ctype sum;
};

#endif
