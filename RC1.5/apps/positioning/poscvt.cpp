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






#include "BasicFramework.hpp"
#include "Position.hpp"
#include "CommandOptionWithPositionArg.hpp"

using namespace std;
using namespace gpstk;

class PosCvt : public BasicFramework
{
public:
   PosCvt(char* arg0);
   virtual bool initialize(int argc, char *argv[])
      throw();

protected:
   virtual void process();
   
private:
   CommandOptionWithPositionArg ecefOption;
   CommandOptionWithPositionArg geodeticOption;
   CommandOptionWithPositionArg geocentricOption;
   CommandOptionWithPositionArg sphericalOption;
   CommandOptionNoArg listFormatsOption;
   CommandOptionWithAnyArg outputFormatOption;
   CommandOptionMutex mutexOption;

   string stringToParse;
   string posSpec;
};

PosCvt::PosCvt(char* arg0)
      : BasicFramework(arg0, "Converts from a given input position "
                       "specification to other position formats.  Include the "
                       "quotation marks."),
        ecefOption('\0', "ecef", "%x %y %z", "ECEF \"X Y Z\" in meters"),
        geodeticOption('\0', "geodetic", "%A %L %h", 
                       "Geodetic \"lat lon alt\" in deg, deg, meters"),
        geocentricOption('\0', "geocentric", "%a %l %r",
                         "Geocentric \"lat lon radius\" in deg, deg, meters"),
        sphericalOption('\0', "spherical", "%t %p %r",
                        "Spherical \"theta, phi, radius\" in deg, deg, meters"),
        listFormatsOption('l', "list-formats", "List the available format codes"
                          " for use by the input and output format options."),
        outputFormatOption('F', "output-format", "Write the position with the"
                           " given format.")
{
   ecefOption.setMaxCount(1);
   geodeticOption.setMaxCount(1);
   geocentricOption.setMaxCount(1);
   sphericalOption.setMaxCount(1);
   listFormatsOption.setMaxCount(1);
   outputFormatOption.setMaxCount(1);
   
   mutexOption.addOption(&ecefOption);
   mutexOption.addOption(&geodeticOption);
   mutexOption.addOption(&geocentricOption);
   mutexOption.addOption(&sphericalOption);
}

bool PosCvt::initialize(int argc, char *argv[])
   throw()
{
   if(!BasicFramework::initialize(argc, argv))
      return false;
   
   if(listFormatsOption.getCount())
   {
      cout << "  %X %Y %Z  (cartesian or ECEF in kilometers)" << endl
           << " %x %y %z  (cartesian or ECEF in meters)" << endl
           << " %a %l %r  (geocentric lat,lon,radius, longitude E, "
         "radius in meters)" << endl
           << " %A %L %h  (geodetic lat,lon,height, longitude E, " 
         "height in meters)" << endl
           << " %a %w %R  (geocentric lat,lon,radius, longitude W, "
         "radius in kilometers)" << endl
           << " %A %W %H  (geodetic lat,lon,height, longitude W, "
         "height in kilometers)" << endl
           << " %t %p %r  (spherical theta, phi, radius, " 
         "degrees and meters)" << endl
           << " %T %P %R  (spherical theta, phi, radius, " 
         "radians and kilometers)" << endl;
      
      return false;
   }
   
   return true;
}

void PosCvt::process()
{
   try
   {
      Position pos;
      
      CommandOptionWithPositionArg *whichOpt = 
         dynamic_cast<CommandOptionWithPositionArg *>(mutexOption.whichOne());
      
      if (whichOpt != NULL)
      {
         pos = whichOpt->getPosition().front();
      }
      
      if (outputFormatOption.getCount())
      {
         cout << pos.printf(outputFormatOption.getValue()[0]) << endl;
      }
      else
      {
         using StringUtils::leftJustify;
         string four(4, ' '); // four spaces!
         
         Position foo = pos.asECEF();
         
         cout << endl
              << four << leftJustify("ECEF (x,y,z) in meters", 36)
              << foo.printf("%.4x %.4y %.4z") << endl;

         foo = pos.asGeodetic();

         cout << four << leftJustify("Geodetic (llh) in deg, deg, m", 36)
              << foo.printf("%.8A %.8L %.4h") << endl;

            // no Position::asGeocentric() available
         foo = pos.transformTo(Position::Geocentric);
         
         cout << four << leftJustify("Geocentric (llr) in deg, deg, m", 36)
              << pos.printf("%.8a %.8L %.4r") << endl;

         foo = pos.transformTo(Position::Spherical);

         cout << four << leftJustify("Spherical (tpr) in deg, deg, m", 36)
              << pos.printf("%.8t %.8p %.4r") << endl
            
              << endl << endl;
      }
      
   }
   catch(GeometryException& ge)
   {
      cout << ge << endl;
   }
}
   
int main(int argc, char* argv[])
{
   try
   {
      PosCvt pc(argv[0]);

      if (!pc.initialize(argc, argv))
         return 0;
      
      if(!pc.run())
         return 1;

      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "Caught an unknown exception." << endl;
   }

   return 1;
}
