#pragma ident "$Id$"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "icd_200_constants.hpp"
#include <iostream>

using namespace std;
using namespace gpstk;

// ISO C++ forbids declaration of `main' with no type
int main(int argc, char *argv[])
{
    int myprn;

    if (argc<2)
    {
       cout << "Required argument is a RINEX obs file." << endl;
       exit(-1);
    }

    cout << "Name your PRN of interest (by number: 1 through 32): ";
    cin  >> myprn;

    double gamma = (L1_FREQ / L2_FREQ)*(L1_FREQ / L2_FREQ);

    try
    {
       cout << "Reading " << argv[1] << "." << endl;

//Declare RINEX observation file streams and data objects
//-------------------------------------------------------
       RinexObsStream roffs(argv[1]);
       // It is necessary to set the failbit in order to throw exceptions 
       roffs.exceptions(ios::failbit);
       RinexObsHeader roh;
       RinexObsData roe;
       RinexObsData::RinexDatum dataobj;

//Read the RINEX header (this could be skipped).
//--------------------------------------------       
       roffs >> roh;

//Print RINEX header to terminal screen
//-------------------------------
       roh.dump(cout);

//Loop through epochs and process data for each.
//----------------------------------------------
       while (roffs >> roe)
       {

            cout << roe.time  << " ";

//Make a GPSTK SatID object for your PRN so you can search for it.
//-----------------------------------------------------------------
            SatID prn(myprn, SatID::systemGPS);

//Check to see if your PRN is in view at this epoch (ie. search for the PRN).
//-------------------------------------------------------------------------
            RinexObsData::RinexSatMap::iterator pointer = roe.obs.find(prn);
            if( pointer == roe.obs.end() ) cout << "PRN " << myprn << " not in view " << endl;
            else
            {

//Get P1 pseudorange code.
//Here are two equivalent ways to get the RinexDatum from the RinexObsData object
//-------------------------------------------------------------------------------
                dataobj = roe.obs[prn][RinexObsHeader::P1];  //The intuitive way

                RinexObsData::RinexDatum dataobj2 = (*pointer).second[RinexObsHeader::P1];  //The more efficient STL way

                // Another way to do the same that above
                //RinexObsData::RinexDatum dataobj2 = pointer->second[RinexObsHeader::P1];

                if( dataobj.data != dataobj2.data) cout << "STL has a bug! (Type crtl-C now or else orcs will crawl from you ears!)" << endl;

                double P1 = dataobj.data;

//Get P2 pseudorange and L1 phase measurement.
//We will stick with the intuitive way.
//-------------------------------------
                dataobj = roe.obs[prn][RinexObsHeader::P2];
                double P2 = dataobj.data;

                dataobj = roe.obs[prn][RinexObsHeader::L1];
                double L1 = dataobj.data;

//Compute multipath
//-----------------
                double mu = P1 -L1*(C_GPS_M/L1_FREQ) -2*(P1 -P2)/(1-gamma);
 
                // The following line makes sure that you get a proper output format
                // The line above with "roh.dump" sets this, but just in case...               
                cout << fixed << setw(7) << setprecision(3);

                cout << " PRN " << myprn << " biased multipath " <<  mu << endl;
            }

       }

       cout << "Read " << roffs.recordNumber << " epochs.  Cheers."  << endl;
       exit(0);
    }
    catch(FFStreamError& e)
    {
       cout << e;
       exit(1);
    }
    catch(Exception& e)
    {
       cout << e;
       exit(1);
    }
    catch (...)
    {
       cout << "unknown error.  I don't feel so well..." << endl;
       exit(1);
    }

    exit(0);
 }
