#pragma ident "$Id$"

#include "BasicFramework.hpp"
#include "MSCStore.hpp"
#include "Position.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "SystemTime.hpp"

using namespace std;
using namespace gpstk;

class PosMSC : public BasicFramework
{
public:
   PosMSC(char *arg0);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual bool initialize(int argc, char *argv[])
      throw();
#pragma clang diagnostic pop
protected:
   virtual void process();

private:
   CommandOptionWithSimpleTimeArg timeOption;
   CommandOptionWithAnyArg stationOption;
   CommandOptionWithAnyArg mscFileOption;
   CommandOptionNoArg listFormatsOption;
   CommandOptionWithAnyArg outputFormatOption;

   MSCStore mscs;
};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
PosMSC::PosMSC(char* arg0)
      : BasicFramework(arg0, "Produce a position using a MSC file, station ID,"
                       " and a time."),
        stationOption('s', "station", "Station for which to calculate position."
                      " String mnemonic and numerals work", true),
        mscFileOption('f', "file", "The MSC file to load.", true),
        timeOption('t', "time", "Time at which to calculate the position. "
                   " format = \"YYYY DDD SSSSS\""),
        listFormatsOption('l', "list-formats", "list the available format codes"
                          " for use by the output format option."),
        outputFormatOption('F', "output-format", "Write the position with the"
                           " given format. (Default: ECEF in meters)")
{
   stationOption.setMaxCount(1);
   mscFileOption.setMaxCount(1);
   timeOption.setMaxCount(1);
   listFormatsOption.setMaxCount(1);
   outputFormatOption.setMaxCount(1);
}
#pragma clang diagnostic pop
bool PosMSC::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv))
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


void PosMSC::process()
{
   mscs.loadFile(mscFileOption.getValue().front());

   CommonTime time = SystemTime();
   if (timeOption.getCount())
      time = timeOption.getTime().front();
   
      // Change the TimeSystem of time to "Any" because the difference between
      // the time systems doesn't matter much for this application.
   time.setTimeSystem(TimeSystem::Any);

   Position pos( mscs.getXvt(stationOption.getValue().front(), time) );
   
   if (outputFormatOption.getCount())
   {
      cout << pos.printf(outputFormatOption.getValue().front()) << endl;
   }
   else
   {
      cout << pos.printf("%.4x %.4y %.4z") << endl;
   }
   
}

int main(int argc, char *argv[])
{
   try
   {
      PosMSC foo(argv[0]);
      if (!foo.initialize(argc, argv))
         return 0;
      if (!foo.run())
         return 1;
      return 0;
   }
   catch (Exception& e)
   {
      cout << e << endl;
   }
   catch (exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "Caught an unknown exception." << endl;
   }
   
   return 1;
}
