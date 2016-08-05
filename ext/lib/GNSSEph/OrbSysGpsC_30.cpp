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
 * @file OrbSysGpsC_30.cpp
 * OrbSysGpsC_30 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsC_30.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   OrbSysGpsC_30::OrbSysGpsC_30()
      :OrbSysGpsC(),
       Tgd(0.0), 
       ISC_L1CA(0.0),
       ISC_L2C(0.0),
       ISC_L5I5(0.0),
       ISC_L5Q5(0.0),
       avail_Tgd(false),
       avail_L1CA(false),
       avail_L2C(false),
       avail_L5I5(false),
       avail_L5Q5(false)
   {
      for (int i=0;i<4;i++)
      {
         alpha[i] = 0.0;
         beta[i]  = 0.0;
      }
   }

   OrbSysGpsC_30::OrbSysGpsC_30(const PackedNavBits& msg)
      throw( InvalidParameter):
       OrbSysGpsC(),
       Tgd(0.0), 
       ISC_L1CA(0.0),
       ISC_L2C(0.0),
       ISC_L5I5(0.0),
       ISC_L5Q5(0.0),
       avail_Tgd(false),
       avail_L1CA(false),
       avail_L2C(false),
       avail_L5I5(false),
       avail_L5Q5(false)
   {
      for (int i=0;i<4;i++)
      {
         alpha[i] = 0.0;
         beta[i]  = 0.0;
      }
      loadData(msg);
   }

   OrbSysGpsC_30* OrbSysGpsC_30::clone() const
   {
      return new OrbSysGpsC_30 (*this); 
   }

   bool OrbSysGpsC_30::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsC_30 object.
      const OrbSysGpsC_30* p = dynamic_cast<const OrbSysGpsC_30*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsC::isSameData(p)) return false;
       
            // Finally, examine the contents
      if (avail_Tgd  != p->avail_Tgd)  return false;
      if (avail_L1CA != p->avail_L1CA) return false;
      if (avail_L2C  != p->avail_L2C)  return false;
      if (avail_L5I5 != p->avail_L5I5) return false;
      if (avail_L5Q5 != p->avail_L5Q5) return false;

         // Whether avail values are true or false, 
         // the actual values should match.  That is to say,
         // if avail = false, the corresponding ISC is 0.0.
      if (Tgd      != p->Tgd)      return false;
      if (ISC_L1CA != p->ISC_L1CA) return false;
      if (ISC_L2C  != p->ISC_L2C)  return false;
      if (ISC_L5I5 != p->ISC_L5I5) return false;
      if (ISC_L5I5 != p->ISC_L5I5) return false;

      for (int i=0;i<4;i++)
      {
         if (alpha[i] != p->alpha[i]) return false;
         if ( beta[i] != p->beta[i]) return false;
      }
         
      return true;      
   }
   
   void OrbSysGpsC_30::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=30)
      {
         char errStr[80];
         std::string msgString("Expected GPS CNAV MT 30.  Found unique ID ");
         msgString += StringUtils::asString(UID);
         InvalidParameter exc(msgString);
         GPSTK_THROW(exc);    
      } 
      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

           // Message Type 30 data
      unsigned long testAvail = 4096;    // Pattern in message of 0x1000
                                         // if data quantity not available
                                          
      unsigned long avail = msg.asUnsignedLong(127,13,1);
      avail_Tgd = false;
      if (avail!=testAvail)
      {
         avail_Tgd = true;
         Tgd       = msg.asSignedDouble(127, 13, -35);
      }

      avail = msg.asUnsignedLong(140,13,1);
      avail_L1CA = false;
      if (avail!=testAvail)
      {
         avail_L1CA = true;
         ISC_L1CA  = msg.asSignedDouble(140, 13, -35);
      }
      
      avail = msg.asUnsignedLong(153,13,1);
      avail_L2C = false;
      if (avail!=testAvail)
      {
         avail_L2C = true;
         ISC_L2C   = msg.asSignedDouble(153, 13, -35);
      }
      
      avail = msg.asUnsignedLong(166,13,1);
      avail_L5I5 = false;
      if (avail!=testAvail)
      {
         avail_L5I5 = true;
         ISC_L5I5  = msg.asSignedDouble(166, 13, -35);
      }

      avail = msg.asUnsignedLong(179,13,1);
      avail_L5Q5 = false;
      if (avail!=testAvail)
      {
         avail_L5Q5 = true;
         ISC_L5Q5  = msg.asSignedDouble(179, 13, -35);
      }

      alpha[0] = msg.asSignedDouble(192, 8, -30);
      alpha[1] = msg.asSignedDouble(200, 8, -27);
      alpha[2] = msg.asSignedDouble(208, 8, -24);
      alpha[3] = msg.asSignedDouble(216, 8, -24);
      beta[0]  = msg.asSignedDouble(224, 8,  11);
      beta[1]  = msg.asSignedDouble(232, 8,  14);
      beta[2]  = msg.asSignedDouble(240, 8,  16);
      beta[3]  = msg.asSignedDouble(248, 8,  16);

         // Need to convert from sec/semi-circle to sec/rad
      double conversion = 1.0 / PI; 
      alpha[1] *= conversion;
      beta[1]  *= conversion;
      alpha[2] *= conversion * conversion;
      beta[2]  *= conversion * conversion;
      alpha[3] *= conversion * conversion * conversion;
      beta[3]  *= conversion * conversion * conversion;

      dataLoadedFlag = true;   
   } // end of loadData()

   void OrbSysGpsC_30::dumpTerse(std::ostream& s) const
         throw(InvalidRequest)
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      string ssys = SatID::convertSatelliteSystemToString(satID.system); 
      s << setw(7) << ssys;
      s << " " << setw(2) << satID.id;

      s << "  30";      // UID
      s << " " << printTime(beginValid,"%02m/%02d/%4Y %03j %02H:%02M:%02S") << "  ";

      //s.setf(ios::scientific, ios::floatfield);
      //s.setf(ios::right, ios::adjustfield);
      //s.setf(ios::uppercase);
      //s.precision(10);
      //s.fill(' ');

      //s.setf(ios::fixed, ios::floatfield);
      //s.precision(0);
   } // end of dumpTerse()

   void OrbSysGpsC_30::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           GROUP DELAY CORRECTIONS"
        << endl
        << "Parameter    Avail?     Value" << endl;

      s.setf(ios::scientific, ios::floatfield);
      s.precision(8);
      s.setf(ios::right, ios::adjustfield);
      s.fill(' ');

      s << "Tgd            ";
      if (avail_Tgd)
         s << "Y       " << setw(16) << Tgd << endl;
      else 
         s << "N" << endl;

      s << "ISC(L1CA)      ";
      if (avail_L1CA)
         s << "Y       " << setw(16) << ISC_L1CA << endl;
      else 
         s << "N" << endl;

      s << "ISC(L2C)       ";
      if (avail_L2C)
         s << "Y       " << setw(16) << ISC_L2C << endl;
      else 
         s << "N" << endl;

      s << "ISC(L5I5)      ";
      if (avail_L5I5)
         s << "Y       " << setw(16) << ISC_L5I5 << endl;
      else 
         s << "N" << endl;

      s << "ISC(L5Q5)      ";
      if (avail_L5Q5)
         s << "Y       " << setw(16) << ISC_L5Q5 << endl;
      else 
         s << "N" << endl;

      s << endl
        << "           IONOSPHERIC PARAMETERS"
        << endl;
      s << "  Alpha 0: " << setw(16) << alpha[0] << " sec       "
        << "   Beta 0: " << setw(16) << beta[0]  << " sec       " << endl;
      s << "  Alpha 1: " << setw(16) << alpha[1] << " sec/rad   "
        << "   Beta 1: " << setw(16) << beta[1]  << " sec/rad   " << endl;
      s << "  Alpha 2: " << setw(16) << alpha[2] << " sec/rad**2"
        << "   Beta 2: " << setw(16) << beta[2]  << " sec/rad**2" << endl;
      s << "  Alpha 3: " << setw(16) << alpha[3] << " sec/rad**3"
        << "   Beta 3: " << setw(16) << beta[3]  << " sec/rad**3" << endl;
   } // end of dumpBody()   

} // end namespace gpstk
