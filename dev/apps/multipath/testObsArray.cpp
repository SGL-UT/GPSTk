#include <iostream>

#include "ObsArray.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try {

      ObsArray oa;

      cout << oa.add(RinexObsHeader::C1) << endl;
      cout << oa.add(RinexObsHeader::P2) << endl;
      cout << oa.add("C1-P2") << endl;
      cout << oa.add("C1-1/(gamma-1)*(wl1*L1-wl2*L2)");
      cout << "There are " << oa.getIndexCount() << " obs indices." << endl;

      cout << "Reading input files." << endl;
      oa.load("netrs027.06o.30s","netrs027.06n");
      cout << "Done reading." << endl;

      cout << "Values directly using operator(size_t, size_t): " << endl;
      for (int i=0; i<12; i++)
         cout << setprecision(12) << oa(i,3) << endl;

      cout << "Getting values with a slice: " << endl;
      slice myslice(3,20,oa.getIndexCount()); // This slice corresponds to all P1-C2 
      valarray <double> copyObs = oa.data[myslice];
      slice_array <double> obsSample = oa.data[myslice];
      cout << "There are " << copyObs.size() << " elements in this slice." << endl;
      for (int i=0; i<12; i++)
         cout << copyObs[i] << endl;

      cout << "Operations on a slice: " << endl;
      cout << "Mean value is: " << copyObs.sum() / copyObs.size() << endl;
      //cout << "Mean value is: " << oa.data[myslice].sum() / oa.data[myslice].size() << endl;

      RinexPrn thisPrn(9,systemGPS);
      valarray<bool> prnIdx = (oa.prn==thisPrn);
      valarray<double> prnObs = oa.data[prnIdx];
      valarray<DayTime> prnTime = oa.time[prnIdx];
      cout << "Data for PRN 9:" << endl;
      for (int i=0; i<12; i++)
      {
         cout << prnTime[i].GPSfullweek() << " ";
         cout << prnTime[i].GPSsow() << " ";
         cout << prnObs[i] << endl;
      }

      exit(0);
   }
   catch(Exception& ex)
   {
      cerr << "Exception caught" << endl;
      cerr << ex << endl;
   }
}
