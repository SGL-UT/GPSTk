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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
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
      c1Prefit.body[TypeID::gravDelay]    = -1.0;
      c1Prefit.body[TypeID::satPCenter]   = -1.0;
      c1Prefit.body[TypeID::tropoSlant]   = -1.0;
      c1Prefit.body[TypeID::ionoL1]       = -1.0;
         // The instrumental delay for C1 is not exactly TGD, but it is close
      c1Prefit.body[TypeID::instC1]       = -1.0;
      c1Prefit.body[TypeID::mpC1]         = -1.0;

         // Definition to compute prefit residual of P1
      p1Prefit.header                     = TypeID::prefitC;
      p1Prefit.body[TypeID::P1]           = +1.0;
      p1Prefit.body[TypeID::rho]          = -1.0;
      p1Prefit.body[TypeID::dtSat]        = +1.0;
      p1Prefit.body[TypeID::rel]          = -1.0;
      p1Prefit.body[TypeID::gravDelay]    = -1.0;
      p1Prefit.body[TypeID::satPCenter]   = -1.0;
      p1Prefit.body[TypeID::tropoSlant]   = -1.0;
      p1Prefit.body[TypeID::ionoL1]       = -1.0;
         // Differential code biases (DCBs) for P1-P2
      p1Prefit.body[TypeID::instC1]       = -1.0;
      p1Prefit.body[TypeID::mpC1]         = -1.0;

         // Definition to compute prefit residual of L1
      l1Prefit.header                     = TypeID::prefitL;
      l1Prefit.body[TypeID::L1]           = +1.0;
      l1Prefit.body[TypeID::rho]          = -1.0;
      l1Prefit.body[TypeID::dtSat]        = +1.0;
      l1Prefit.body[TypeID::rel]          = -1.0;
      l1Prefit.body[TypeID::gravDelay]    = -1.0;
      l1Prefit.body[TypeID::satPCenter]   = -1.0;
      l1Prefit.body[TypeID::tropoSlant]   = -1.0;
      l1Prefit.body[TypeID::ionoL1]       = +1.0;
         // Coefficient for L1 windUp is L1 wavelength/2*PI
      l1Prefit.body[TypeID::windUp]       = -L1_WAVELENGTH/TWO_PI;
      l1Prefit.body[TypeID::mpL1]         = -1.0;

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
      pcPrefit.body[TypeID::gravDelay]    = -1.0;
      pcPrefit.body[TypeID::satPCenter]   = -1.0;
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
      lcPrefit.body[TypeID::gravDelay]    = -1.0;
      lcPrefit.body[TypeID::satPCenter]   = -1.0;
      lcPrefit.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for LC windUp is LC wavelenght/2*PI
      lcPrefit.body[TypeID::windUp]       = -0.1069533781421467/TWO_PI;

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

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L1 frequency
      GRAPHIC1Combination.header           = TypeID::GRAPHIC1;
      GRAPHIC1Combination.body[TypeID::P1] = +0.5;
      GRAPHIC1Combination.body[TypeID::L1] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L1 frequency (using C1 instead of P1)
      GRAPHIC1CombinationWithC1.header           = TypeID::GRAPHIC1;
      GRAPHIC1CombinationWithC1.body[TypeID::C1] = +0.5;
      GRAPHIC1CombinationWithC1.body[TypeID::L1] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L2 frequency
      GRAPHIC2Combination.header           = TypeID::GRAPHIC2;
      GRAPHIC2Combination.body[TypeID::P2] = +0.5;
      GRAPHIC2Combination.body[TypeID::L2] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L5 frequency
      GRAPHIC5Combination.header           = TypeID::GRAPHIC5;
      GRAPHIC5Combination.body[TypeID::C5] = +0.5;
      GRAPHIC5Combination.body[TypeID::L5] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L6 frequency
      GRAPHIC6Combination.header           = TypeID::GRAPHIC6;
      GRAPHIC6Combination.body[TypeID::C6] = +0.5;
      GRAPHIC6Combination.body[TypeID::L6] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L7 frequency
      GRAPHIC7Combination.header           = TypeID::GRAPHIC7;
      GRAPHIC7Combination.body[TypeID::C7] = +0.5;
      GRAPHIC7Combination.body[TypeID::L7] = +0.5;

         // Definition to compute the GRoup And PHase Ionospheric
         // Combination (GRAPHIC) in the L8 frequency
      GRAPHIC8Combination.header           = TypeID::GRAPHIC8;
      GRAPHIC8Combination.body[TypeID::C8] = +0.5;
      GRAPHIC8Combination.body[TypeID::L8] = +0.5;

         // Definition to compute WL combination
      wlCombination.header            = TypeID::WL;
      wlCombination.body[TypeID::L1]  = +e;
      wlCombination.body[TypeID::L2]  = -f;

      const double WL_WAVELENGTH = L1_WAVELENGTH*L2_WAVELENGTH/(L2_WAVELENGTH-L1_WAVELENGTH);
      const double WL4_WAVELENGTH = L1_WAVELENGTH*L2_WAVELENGTH/(4.0*L2_WAVELENGTH-5.0*L1_WAVELENGTH);

         // Definition to compute prefit residual of WL
      wlPrefit.header                     = TypeID::prefitWL;
      wlPrefit.body[TypeID::WL]           = +1.0;
      wlPrefit.body[TypeID::rho]          = -1.0;
      wlPrefit.body[TypeID::dtSat]        = +1.0;
      wlPrefit.body[TypeID::rel]          = -1.0;
      wlPrefit.body[TypeID::gravDelay]    = -1.0;
      wlPrefit.body[TypeID::satPCenter]   = -1.0;
      wlPrefit.body[TypeID::tropoSlant]   = -1.0;
      // Coefficient for LC windUp is wavelenght/2*PI
      wlPrefit.body[TypeID::windUp]       = -WL_WAVELENGTH/TWO_PI;

         // Definition to compute WL4 combination
      wl4Combination.header            = TypeID::WL4;
      wl4Combination.body[TypeID::L1]  = +4.0*L1_FREQ/(4.0*L1_FREQ - 5.0*L2_FREQ);
      wl4Combination.body[TypeID::L2]  = -5.0*L2_FREQ/(4.0*L1_FREQ - 5.0*L2_FREQ);

      // Definition to compute prefit residual of WL4
      wl4Prefit.header                     = TypeID::prefitWL4;
      wl4Prefit.body[TypeID::WL4]           = +1.0;
      wl4Prefit.body[TypeID::rho]          = -1.0;
      wl4Prefit.body[TypeID::dtSat]        = +1.0;
      wl4Prefit.body[TypeID::rel]          = -1.0;
      wl4Prefit.body[TypeID::gravDelay]    = -1.0;
      wl4Prefit.body[TypeID::satPCenter]   = -1.0;
      wl4Prefit.body[TypeID::tropoSlant]   = -1.0;
      // Coefficient for LC windUp is wavelenght/2*PI
      wl4Prefit.body[TypeID::windUp]       = -WL4_WAVELENGTH/TWO_PI;

   }  // End of constructor 'LinearCombinations::LinearCombinations()'

} // End of namespace gpstk
