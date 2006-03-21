#pragma ident "$Id: //depot/sgl/gpstk/dev/src/RACRotation.hpp#1 $"
//
//
#ifndef GPSTK_RACROTATION_HPP
#define GPSTK_RACROTATION_HPP

// gpstk
#include "Triple.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "Xvt.hpp"

namespace gpstk
{
   class RACRotation : public gpstk::Matrix<double>
   {
      public:
            // Constructors
         RACRotation( const gpstk::Triple& SVPositionVector,
                      const gpstk::Triple& SVVelocityVector);
         RACRotation(const gpstk::Xvt& xvt);
         
            // Methods
         gpstk::Vector<double> convertToRAC( const gpstk::Vector<double>& inV );
         gpstk::Triple         convertToRAC( const gpstk::Triple& inVec );
         gpstk::Xvt            convertToRAC( const gpstk::Xvt& in );
                                  
            // Utilities
      protected:
         void compute( const gpstk::Triple& SVPositionVector,
                       const gpstk::Triple& SVVelocityVector);
   };
}   
#endif      
