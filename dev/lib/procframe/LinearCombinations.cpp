#pragma ident "$Id$"

/**
 * @file LinearCombinations.cpp
 * This class defines handy linear combinations of GDS data.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "LinearCombinations.hpp"


namespace gpstk
{

   LinearCombinations::LinearCombinations()
   {

      double a(+GAMMA_GPS/0.646944444);
      double b(1.0/0.646944444);

         // Definition to compute prefit residual of C1
      c1Prefit.header                     = TypeID::prefitC;
      c1Prefit.body[TypeID::C1]           = +1.0;
      c1Prefit.body[TypeID::rho]          = -1.0;
      c1Prefit.body[TypeID::dtSat]        = +1.0;
      c1Prefit.body[TypeID::rel]          = -1.0;
      c1Prefit.body[TypeID::tropoSlant]   = -1.0;
      c1Prefit.body[TypeID::ionoSlant]    = -1.0;
      c1Prefit.body[TypeID::instC1]       = -1.0;

         // Definition to compute PC combination
      pcCombination.header                = TypeID::PC;
      pcCombination.body[TypeID::P1]      = +a;
      pcCombination.body[TypeID::P2]      = -b;

         // Definition to compute PC combination, using C1 instead of P1
      pcCombWithC1.header                 = TypeID::PC;
      pcCombWithC1.body[TypeID::C1]       = +a;
      pcCombWithC1.body[TypeID::P2]       = -b;

         // Definition to compute prefit residual of PC
      pcPrefit.header                     = TypeID::prefitC;
      pcPrefit.body[TypeID::PC]           = +1.0;
      pcPrefit.body[TypeID::rho]          = -1.0;
      pcPrefit.body[TypeID::dtSat]        = +1.0;
      pcPrefit.body[TypeID::rel]          = -1.0;
      pcPrefit.body[TypeID::tropoSlant]   = -1.0;

         // Definition to compute LC combination
      lcCombination.header                = TypeID::LC;
      lcCombination.body[TypeID::L1]      = +a;
      lcCombination.body[TypeID::L2]      = -b;

         // Definition to compute prefit residual of LC
      lcPrefit.header                     = TypeID::prefitL;
      lcPrefit.body[TypeID::LC]           = +1.0;
      lcPrefit.body[TypeID::rho]          = -1.0;
      lcPrefit.body[TypeID::dtSat]        = +1.0;
      lcPrefit.body[TypeID::rel]          = -1.0;
      lcPrefit.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for LC windUp is LC wavelenght/2*PI
      lcPrefit.body[TypeID::windUp]       = -0.107/6.2831853071796;

         // Definition to compute PI combination
      piCombination.header                = TypeID::PI;
      piCombination.body[TypeID::P1]      = -1.0;
      piCombination.body[TypeID::P2]      = +1.0;

         // Definition to compute PI combination, using C1 instead of P1
      piCombWithC1.header                 = TypeID::PI;
      piCombWithC1.body[TypeID::C1]       = -1.0;
      piCombWithC1.body[TypeID::P2]       = +1.0;

         // Definition to compute LI combination
      liCombination.header                = TypeID::LI;
      liCombination.body[TypeID::L1]      = +1.0;
      liCombination.body[TypeID::L2]      = -1.0;


      double c( L1_FREQ/(L1_FREQ + L2_FREQ) );
      double d( L2_FREQ/(L1_FREQ + L2_FREQ) );
      double e( L1_FREQ/(L1_FREQ - L2_FREQ) );
      double f( L2_FREQ/(L1_FREQ - L2_FREQ) );

         // Definition to compute Pdelta (PW) combination
      pdeltaCombination.header            = TypeID::Pdelta;
      pdeltaCombination.body[TypeID::P1]  = +c;
      pdeltaCombination.body[TypeID::P2]  = +d;

         // Definition to compute Pdelta (PW) combination, using C1 instead
         // of P1
      pdeltaCombWithC1.header             = TypeID::Pdelta;
      pdeltaCombWithC1.body[TypeID::C1]   = +c;
      pdeltaCombWithC1.body[TypeID::P2]   = +d;

         // Definition to compute Ldelta (LW) combination
      ldeltaCombination.header            = TypeID::Ldelta;
      ldeltaCombination.body[TypeID::L1]  = +e;
      ldeltaCombination.body[TypeID::L2]  = -f;

         // Definition to compute the Melbourne-Wubbena (W) combination
      mwubbenaCombination.header           = TypeID::MWubbena;
      mwubbenaCombination.body[TypeID::L1] = +e;
      mwubbenaCombination.body[TypeID::L2] = -f;
      mwubbenaCombination.body[TypeID::P1] = -c;
      mwubbenaCombination.body[TypeID::P2] = -d;

         // Definition to compute the Melbourne-Wubbena (W) combination,
         // using C1 instead of P1
      mwubbenaCombWithC1.header           = TypeID::MWubbena;
      mwubbenaCombWithC1.body[TypeID::L1] = +e;
      mwubbenaCombWithC1.body[TypeID::L2] = -f;
      mwubbenaCombWithC1.body[TypeID::C1] = -c;
      mwubbenaCombWithC1.body[TypeID::P2] = -d;


   }

} // end namespace gpstk
