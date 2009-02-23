// file: xSP3EphemerisStore.cpp

#include "xSP3EphemerisStore.hpp"
#include "SatID.hpp"
#include "Exception.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3Stream.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xSP3EphemerisStore);


void xSP3EphemerisStore :: setUp (void)
{ 
}

void xSP3EphemerisStore :: RESTest (void)
{
      using namespace std;
      cout << "Reading " << "NGA15081Test.SP3" << endl;
      Rinex3::SP3Stream roffs("NGA15081Test.SP3");
      cout << "Writing " << "Output1.txt" << endl;
      Rinex3::SP3Stream out("Output1.txt",ios::out);
      //out.exceptions(fstream::failbit);
      Rinex3::SP3Header roh;
      Rinex3::SP3Data roe;

      roffs >> roh;
      out << roh;
      //roh.dump(cout);

      while (roffs >> roe)
      {
         out << roe;
	 //roe.dump(cout);
      }
}

void xSP3EphemerisStore :: getXvtTest (void)
{
      using namespace std;
      cout << "Reading " << "Output1.txt" << endl;
      Rinex3::SP3Stream roffs("Output1.txt");
      cout << "Writing " << "Output2.txt" << endl;
      Rinex3::SP3Stream out("Output2.txt",ios::out);
      out.exceptions(fstream::failbit);
      Rinex3::SP3Header roh;
      Rinex3::SP3Data roe;

      roffs >> roh;
      out << roh;
      //roh.dump(cout);

      while (roffs >> roe)
      {
         out << roe;
	 //roe.dump(cout);
      }
}

void xSP3EphemerisStore :: dumpTest (void)
{

}

void xSP3EphemerisStore :: addEphemerisTest (void)
{

}


void xSP3EphemerisStore :: editTest (void)
{

}

void xSP3EphemerisStore :: clearTest (void)
{

}

void xSP3EphemerisStore :: dumpBadPositionsTest (void)
{
}

void xSP3EphemerisStore :: dumpBadClocksTest (void)
{
}
void xSP3EphemerisStore :: maxIntervalTest (void)
{
}
void xSP3EphemerisStore :: getTimeTest (void)
{
}

bool xSP3EphemerisStore :: fileEqualTest (char* handle1, char* handle2)
{
	bool isEqual = false;
	
	ifstream File1;
	ifstream File2;
	
	std::string File1Line;
	std::string File2Line;
	
	File1.open(handle1);
	File2.open(handle2);
	
	while (!File1.eof())
	{
		if (File2.eof()) 
			return isEqual;
		getline (File1, File1Line);
		getline (File2, File2Line);

		if (File1Line != File2Line)
			return isEqual;
	}
	if (!File2.eof())
		return isEqual;
	else
		return isEqual = true;
}
