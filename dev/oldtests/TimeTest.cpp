#pragma ident "$Id$"


#include <iostream>

#include <BasicFramework.hpp>

#include <TimeString.hpp>
#include <TimeConstants.hpp>

#include <ANSITime.hpp>
#include <CivilTime.hpp>
#include <GPSEpochWeekSecond.hpp>
#include <GPSWeekSecond.hpp>
#include <GPSWeekZcount.hpp>
#include <GPSZcount29.hpp>
#include <GPSZcount32.hpp>
#include <JulianDate.hpp>
#include <MJD.hpp>
#include <UnixTime.hpp>
#include <YDSTime.hpp>
#include <SystemTime.hpp>

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // The following constants are as captured from the timcvt program output.
   //
   //  Month/Day/Year                  7/26/2005
   //  Hour:Min:Sec                    17:59:42
   //  Modified Julian Date            53577.7497975392
   //  GPSweek DayOfWeek SecOfWeek     309 2 237582.507390
   //  FullGPSweek Zcount              1333 158388
   //  Year DayOfYear SecondOfDay      2005 207 64782.507390
   //  Unix_sec Unix_usec              1122400782 507390
   //  FullZcount                      162163380

const int defaultMonth = 7;
const int defaultDay = 26;
const int defaultYear = 2005;
const int defaultHour = 17;
const int defaultMinute = 59;
const int defaultSecond = 42;
const double defaultSecondD = 42.507390;

const long double defaultMJD = 53577.7497975392;
const long double defaultJD = (defaultMJD + MJD_TO_JD);

const int defaultWeek = 309;
const int defaultDow = 2;
const double defaultSow = 237582.507390;

const int defaultFullWeek = 1333;
const int defaultZcount = 158388;

const int defaultDoy = 207;
const double defaultSod = 64782.507390;

const int defaultUnixSec = 1122400782;
const int defaultUnixUsec = 507390;
const int defaultFullZcount = 162163380;

const int defaultEpoch = 1;
const int defaultFullZcount32 = ( defaultEpoch << 29 ) | defaultFullZcount;

class Test : public BasicFramework
{
public:
   Test( const string& progName ) 
         : BasicFramework( progName, "Test the New Time Classes!" ),
           ansi( defaultUnixSec ),
           civil( defaultYear, defaultMonth, defaultDay, defaultHour,
                  defaultMinute, defaultSecondD ),
           gews( defaultEpoch, defaultWeek, defaultSow ),
           gws( defaultFullWeek, defaultSow ),
           gwz( defaultFullWeek, defaultZcount ),
           gz29( defaultEpoch, defaultFullZcount ),
           gz32( defaultFullZcount32 ),
           jd( defaultJD ),
           mjd( defaultMJD ),
           ut( defaultUnixSec, defaultUnixUsec ),
           yds( defaultYear, defaultDoy, defaultSod )
   {}
   
   ANSITime ansi;
   CivilTime civil;
   GPSEpochWeekSecond gews;
   GPSWeekSecond gws;
   GPSWeekZcount gwz;
   GPSZcount29 gz29;
   GPSZcount32 gz32;
   JulianDate jd;
   MJD mjd;
   UnixTime ut;
   YDSTime yds;
   SystemTime st;
   
protected:
   virtual void process();

   void dumpCommonTime( const TimeTag& t );
   
   bool simpleCopyTest();
   template<class T>
   bool isCopySuccess( T& t );

   bool conversionTest();
   template<class T>
   bool toCommonAndBack( T& t );

   bool simpleEqualityTest();
   template<class T>
   bool isEqual( T& t );

   bool systemTimeTest();

   bool simpleLessThanTest();

   bool otherTest();

};

void Test::process()
{
   simpleCopyTest();
   simpleEqualityTest();
   conversionTest();
   systemTimeTest();
   simpleLessThanTest();
//   otherTest();

   cout << "Test Processing Complete." << endl;
}

void Test::dumpCommonTime( const TimeTag& t )
{
   long day, sod;
   double fsod;

   CommonTime( t.convertToCommonTime() ).get( day, sod, fsod );

   cout << t << " -> " << day << "d " << sod << "s " 
        << setprecision(15) << fsod << "f" << endl;
}

bool Test::simpleCopyTest()
{
   cout << "Simple Copy Test:" << endl;
   cout << "ANSI:   " << ( isCopySuccess( ansi ) ? "PASS" : "FAIL" ) << endl;
   cout << "Civil:  " << ( isCopySuccess( civil ) ? "PASS" : "FAIL" ) << endl;
   cout << "GPSEWS: " << ( isCopySuccess( gews ) ? "PASS" : "FAIL" ) << endl;
   cout << "GPSWS:  " << ( isCopySuccess( gws ) ? "PASS" : "FAIL" ) << endl;
   cout << "GPSWZ:  " << ( isCopySuccess( gwz ) ? "PASS" : "FAIL" ) << endl;
   cout << "GZ29:   " << ( isCopySuccess( gz29 ) ? "PASS" : "FAIL" ) << endl;
   cout << "GZ32:   " << ( isCopySuccess( gz32 ) ? "PASS" : "FAIL" ) << endl;
   cout << "JD:     " << ( isCopySuccess( jd ) ? "PASS" : "FAIL" ) << endl;
   cout << "MJD:    " << ( isCopySuccess( mjd ) ? "PASS" : "FAIL" ) << endl;
   cout << "Unix:   " << ( isCopySuccess( ut ) ? "PASS" : "FAIL" ) << endl;
   cout << "YDS:    " << ( isCopySuccess( yds ) ? "PASS" : "FAIL" ) << endl;
   cout << endl;
   
   return true;
}

template<class T>
bool Test::isCopySuccess( T& t )
{
   T t1( t );
   if( t == t1 )
   {
      return true;
   }
   return false;
}

bool Test::conversionTest()
{
   cout << "Conversion Test:" << endl;
   cout << "ANSI:   " << ( toCommonAndBack( ansi ) ? "PASS" : "FAIL" ) << endl;
   cout << "Civil:  " << ( toCommonAndBack( civil ) ? "PASS" : "FAIL" ) << endl;
   cout << "GPSEWS: " << ( toCommonAndBack( gews ) ? "PASS" : "FAIL" ) << endl;
   cout << "GPSWS:  " << ( toCommonAndBack( gws ) ? "PASS" : "FAIL" ) << endl;
   cout << "GPSWZ:  " << ( toCommonAndBack( gwz ) ? "PASS" : "FAIL" ) << endl;
   cout << "GZ29:   " << ( toCommonAndBack( gz29 ) ? "PASS" : "FAIL" ) << endl;
   cout << "GZ32:   " << ( toCommonAndBack( gz32 ) ? "PASS" : "FAIL" ) << endl;
   cout << "JD:     " << ( toCommonAndBack( jd ) ? "PASS" : "FAIL" ) << endl;
   cout << "MJD:    " << ( toCommonAndBack( mjd ) ? "PASS" : "FAIL" ) << endl;
   cout << "Unix:   " << ( toCommonAndBack( ut ) ? "PASS" : "FAIL" ) << endl;
   cout << "YDS:    " << ( toCommonAndBack( yds ) ? "PASS" : "FAIL" ) << endl;
   cout << endl;

   if( verboseLevel )
   {
      dumpCommonTime( ansi );
      dumpCommonTime( civil );
      dumpCommonTime( gews );
      dumpCommonTime( gws );
      dumpCommonTime( gwz );
      dumpCommonTime( gz29 );
      dumpCommonTime( gz32 );
      dumpCommonTime( jd );
      dumpCommonTime( mjd );
      dumpCommonTime( ut );
      dumpCommonTime( yds );
      cout << endl;
   }

   return true;
}

template<class T>
bool Test::toCommonAndBack( T& t )
{
   if( verboseLevel )
   {
      cout << "myval: " << t << endl;
   }

   T t1;
   t1.convertFromCommonTime( t.convertToCommonTime() );
   if( t != t1 )
   {
      cout << t << " != " << t1 << endl;
      return false;
   }
   return true;
}

bool Test::simpleEqualityTest()
{
   cout << "Simple Equality Test" << endl;
   cout << "ANSI:   " << endl; isEqual( ansi );
   cout << "Civil:  " << endl; isEqual( civil );
   cout << "GPSEWS: " << endl; isEqual( gews );
   cout << "GPSWS:  " << endl; isEqual( gws );
   cout << "GPSWZ:  " << endl; isEqual( gwz );
   cout << "GZ29:   " << endl; isEqual( gz29 );
   cout << "GZ32:   " << endl; isEqual( gz32 );
   cout << "JD:     " << endl; isEqual( jd );
   cout << "MJD:    " << endl; isEqual( mjd );
   cout << "Unix:   " << endl; isEqual( ut );
   cout << "YDS:    " << endl; isEqual( yds );
   cout << endl;

}

template<class T>
bool Test::isEqual( T& t )
{
   cout << " to ANSI:   " << ( t == ANSITime( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to Civil:  " << ( t == CivilTime( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to GPSEWS: " << ( t == GPSEpochWeekSecond( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to GPSWS:  " << ( t == GPSWeekSecond( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to GPSWZ:  " << ( t == GPSWeekZcount( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to GZ29:   " << ( t == GPSZcount29( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to GZ32:   " << ( t == GPSZcount32( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to JD:     " << ( t == JulianDate( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to MJD:    " << ( t == MJD( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to Unix:   " << ( t == UnixTime( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << " to YDS:    " << ( t == YDSTime( t ) ? "PASS" : "FAIL" )
        << endl;
   cout << endl;
   
   return true;
}   

bool Test::systemTimeTest()
{
   cout << "SystemTime:  " << st << endl
        << "  to ANSI:   " << ANSITime( st ) << endl
        << "  to Civil:  " << CivilTime( st ) << endl
        << "  to GPSEWS: " << GPSEpochWeekSecond( st ) << endl
        << "  to GPSWS:  " << GPSWeekSecond( st ) << endl
        << "  to GPSWZ:  " << GPSWeekZcount( st ) << endl
        << "  to GZ29:   " << GPSZcount29( st ) << endl
        << "  to GZ32:   " << GPSZcount32( st ) << endl
        << "  to JD:     " << JulianDate( st ) << endl
        << "  to MJD:    " << MJD( st ) << endl
        << "  to Unix:   " << UnixTime( st ) << endl
        << "  to YDS:    " << YDSTime( st ) << endl;
   
}

bool Test::simpleLessThanTest()
{
   ansi = ANSITime( st );
   civil = CivilTime( st );
   gews = GPSEpochWeekSecond( st );
   gws = GPSWeekSecond( st );
   gwz = GPSWeekZcount( st );
   gz29 = GPSZcount29( st );
   gz32 = GPSZcount32( st );
   jd = JulianDate( st );
   mjd = MJD( st );
   ut = UnixTime( st );
   yds = YDSTime( st );
   
   CommonTime ct( st );
   ct += 5.0; // add five seconds

   cout << "SimpleLessThanTest: " << endl
        << " ANSI:   " << ( ansi < ct ? "PASS" : "FAIL" ) << endl
        << " Civil:  " << ( civil < ct ? "PASS" : "FAIL" ) << endl
        << " GPSEWS: " << ( gews < ct ? "PASS" : "FAIL" ) << endl
        << " GPSWS:  " << ( gws < ct ? "PASS" : "FAIL" ) << endl
        << " GPSWZ:  " << ( gwz < ct ? "PASS" : "FAIL" ) << endl
        << " GZ29:   " << ( gz29 < ct ? "PASS" : "FAIL" ) << endl
        << " GZ32:   " << ( gz32 < ct ? "PASS" : "FAIL" ) << endl
        << " JD:     " << ( jd < ct ? "PASS" : "FAIL" ) << endl
        << " MJD:    " << ( mjd < ct ? "PASS" : "FAIL" ) << endl
        << " Unix:   " << ( ut < ct ? "PASS" : "FAIL" ) << endl
        << " YDS:    " << ( yds < ct ? "PASS" : "FAIL" ) << endl;
}

bool Test::otherTest()
{
   try
   {
      YDSTime one( 2005, 1, 0 ), two( 2005, 234, 5648.09 );
      
      cout << "one: " << one << endl
           << "two: " << two << endl;
      
      YDSTime tre( one );
      
      cout << "tre: " << tre << endl;
      
      if( one == tre )
      {
         cout << " one == tre " << endl;
      }
      else
      {
         cout << " one != tre " << endl;
      }
      
      cout << endl
           << "--- scanf test ---" << endl;
      
      string str( "2005 234 5648.09" );
      string fmt( "%Y %j %s" );
      
      cout << "str: " << str << endl
           << "fmt: " << fmt << endl;
      
      TimeTag::IdToValue info;
      TimeTag::getInfo( str, fmt, info );
      cout << "parsed info:" << endl;
      
      for( TimeTag::IdToValue::iterator i = info.begin(); 
           i != info.end(); i++ )
      {
         cout << "info[" << i->first << "] = <" << i->second << ">" << endl
              << "  asInt() -> " << asInt( i->second ) << endl;
      }
      
      if( tre.setFromInfo( info ) )
      {
         cout << "setFromInfo() ok" << endl;
      }
      else
      {
         cout << "setFromInfo() failed" << endl;
      }
      cout << "tre: " << tre << endl;   
      
      cout << "Altogether in one scanf() call: " << endl;
      two = one;
      cout << "two = one -> two: " << two << endl;
      two.scanf( str, fmt );
      cout << "two.scanf( str, fmt ) -> two: " << two << endl;

      cout << "two " << ( (two == tre) ? "==" : "!=" ) << " tre" << endl;

      cout << endl
           << "--- scanTime test ---" << endl;
      
      string str2( "1337 92048.09" );
      string fmt2( "%F %g" );
      
      cout << "two: " << two << endl
           << "tre: " << tre << endl
           << "str: " << str2 << endl
           << "fmt: " << fmt2 << endl;
      
      scanTime( tre, str, fmt );
      
      cout << "scanTime( tre, str, fmt);" << endl
           << "tre: " << tre << endl;
      
      return true;
   }
   catch( gpstk::Exception& exc )
   {
      cout << exc << endl;
   }
   
   return false;
}

int main (int argc, char *argv[])
{
   try
   {
      Test t( argv[0] );
      
      if( !t.initialize( argc, argv ) )
      {
         return 0;
      }
      
      if( !t.run() )
      {
         return 1;
      }

      return 0;
   }
   catch( Exception& exc )
   {
      cout << exc << endl;
   }
   catch( exception& e )
   {
      cout << e.what() << endl;
   }
   catch( ... )
   {
      cout << "Caught an unknown exception." << endl;
   }
   return 1;
}

