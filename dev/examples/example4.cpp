#include <string>
#include <vector>

#include "RinexObsBase.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"

#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"

#include "TropModel.hpp" 
#include "BCEphemerisStore.hpp"
#include "PRSolution.hpp"
#include "icd_200_constants.hpp"

using namespace std;
using namespace gpstk;

main(int argc, char *argv[])
{
   BCEphemerisStore bcestore;
   PRSolution raimSolver;
   ZeroTropModel noTropModel;
   GGTropModel ggTropModel;
   TropModel *tropModelPtr=&noTropModel;


   if ((argc < 3) || (argc>4))
   {
      cerr <<  "Usage:" << endl; 
      cerr << "   " << argv[0] << " <RINEX Obs file>  <RINEX Nav file>  [<RINEX Met file>]" << endl;
      exit (-1);
   }

   const double gamma = (L1_FREQ / L2_FREQ)*(L1_FREQ / L2_FREQ);
   
   try
   {  
      // Read nav file and store unique list of ephemeredes
      RinexNavStream rnffs(argv[2]);
      RinexNavData rne;
      RinexNavHeader hdr;
      
      rnffs >> hdr;

      while (rnffs >> rne) bcestore.addEphemeris(rne);
      bcestore.SearchNear();
      
      // If provided, open and store met file into a linked list.
      list<RinexMetData> rml;
      if (argc==4)
      {
         RinexMetStream rms(argv[3]);
         RinexMetHeader rmh;
         rms >> rmh;
         
         RinexMetData rmd;
         tropModelPtr=&ggTropModel;
         while (rms >> rmd) rml.push_back(rmd);
      }

       // Open and read the observation file one epoch at a time.
      // For each epoch, compute and print a position solution
      RinexObsStream roffs(argv[1]);
      roffs.exceptions(ios::failbit);

      RinexObsHeader roh;
      RinexObsData rod;

      roffs >> roh;

      list<RinexMetData>::iterator mi=rml.begin();

      while (roffs >> rod)
      {
         double T, P, H;
         
         // Find a weather point.
         while ( (argc==4) &&
                 (!rml.empty()) &&
                 (mi!=rml.end()) &&
                 ((*mi).time < rod.time) )
         {
            mi++;
            ggTropModel.setWeather((*mi).data[RinexMetHeader::TD],
                                   (*mi).data[RinexMetHeader::PR],
                                   (*mi).data[RinexMetHeader::HR]);
         }
         

         // Apply editing criteria 
         if  (rod.epochFlag == 0 || rod.epochFlag == 1) // Begin usable data
	 {
	    vector<SatID> prnVec;
            vector<double> rangeVec;

	    RinexObsData::RinexSatMap::const_iterator it;
            for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
	    {
	       RinexObsData::RinexObsTypeMap otmap;
	       RinexObsData::RinexObsTypeMap::const_iterator itP1, itP2; 
               RinexObsData::RinexDatum meas;
               otmap = (*it).second;
               itP1 = otmap.find(RinexObsHeader::P1);
     
               if (itP1!=otmap.end())
	       {
                  double ionocorr = 0;
                  itP2 = otmap.find(RinexObsHeader::P2);
                  if (itP2!=otmap.end()) 
                     ionocorr = 1./(1.-gamma)*((*itP1).second.data-(*itP2).second.data);
                  prnVec.push_back((*it).first);
                  rangeVec.push_back((*itP1).second.data-ionocorr);
	       }           

            }

            raimSolver.RMSLimit = 3e6;
	    raimSolver.RAIMCompute(rod.time,prnVec,rangeVec, bcestore, \
			       tropModelPtr);

           if (raimSolver.isValid())
	   {
              cout << setprecision(12) << raimSolver.Solution[0] << " " ;
              cout << raimSolver.Solution[1] << " " ;
              cout << raimSolver.Solution[2];
              cout << endl ;
           }
            
 
 	 } // End usable data

      } // End loop through each epoch
   }
   catch(Exception& e)
   {
      cerr << e << endl;     
   }
   catch (...)
   {
      cerr << "Caught an unexpected exception." << endl;
   }


      exit(0);
      
}





