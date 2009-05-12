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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "rinexpvt.hpp"
#include "icd_200_constants.hpp"

using namespace std;
using namespace gpstk;

double carrierPhaseSmooth(SatID sat, double range, double phase,
			  DayTime t, double maxAge=86400,
			  double datarate=30)
{
    static map<SatID,double> smoothedRange;
    static map<SatID,DayTime> lastEpoch;
    static map<SatID,double> lastPhase;
    static map<SatID,DayTime> firstEpoch;

    bool debug = true;
    const double teps = .1; // fudge factor for missed epochs 
    const int k = 10;
               
    // Should this smoother be (re)started?
    bool initialize = false; 

    // Record why the smoother is restarted, if necessary
    enum smootherState { NORMAL=0, NODATA, SKIPPEDEPOCH, AGE, SLIP};
    smootherState thisState = NORMAL;

    // Yes, if we are just beginning to see data for this PRN
    map<SatID,double>::iterator i, j;
    i = smoothedRange.find(sat);
    j = lastPhase.find(sat);

    initialize = ((i==smoothedRange.end())||(j==lastPhase.end())); 
    if (initialize) thisState = NODATA;
    
    // Yes, if we skipped an epoch
    if (!initialize)
    {
	if (initialize = ( fabs(t - lastEpoch[sat] - datarate) > teps ))
           thisState = SKIPPEDEPOCH;
    }

    // Yes, if the filter is too old
    if (!initialize)
    {
        if (initialize = ( fabs( t - firstEpoch[sat]) > maxAge))
           thisState = AGE;
    }

    if (initialize)
    {
	smoothedRange[sat]=range;
        firstEpoch[sat]=t;
    }
    else
	smoothedRange[sat] = 
	    (range + (k-1)*(smoothedRange[sat] + phase - lastPhase[sat]))/k;

    if (fabs((smoothedRange[sat] - range)) > 20)
    { 
       initialize = true;
       smoothedRange[sat]=range;
       firstEpoch[sat]=t; 
       thisState = SLIP;
    }

    lastPhase[sat] = phase;
    lastEpoch[sat] = t;

    if (debug)
    {
       static ofstream debugStream("smootherdebug.txt");
       debugStream << t.printf("%F %g ");
       //debugStream << t.printf("%Y %m %d %02H %02M %f ");
       debugStream << sat.id << " "; 
       debugStream << setprecision(12) << smoothedRange[sat] << " ";
       debugStream << range << " " << phase << " ";
       debugStream << thisState << " ";
//       debugStream << lastPhase[sat];
       debugStream << endl;
    }

    return smoothedRange[sat];

}

RINEXPVTSolution::RINEXPVTSolution(char *arg0)
    : BasicFramework(arg0, "GPSTk PVT Generator\n\nThis application generates user positions based on RINEX observations.\n\nNOTE: Although the -n and -p arguments appear as optional below, one of the two must be used. An ephemeris source must be specified."),
      obsOption('o', "obs-file", "RINEX Obs File.", true),
      navOption('n', "nav-file", "RINEX Nav File. Required for single frequency ionosphere correction.", false),
      peOption('p', "pe-file",  "SP3 Precise Ephemeris File. Repeat this for each input file.", false),
      metOption('m', "met-file", "RINEX Met File.", false),
      spsOption('s', "single-frequency", "Use only C1 (SPS)"),
      ppsOption('f', "dual-frequency", "Use only P1 and P2 (PPS)"),
      timeFormatOption('t',"time-format","Alternate time format string."),
      ionoOption('i',"no-ionosphere", "Do NOT correct for ionosphere delay."),
      enuOption('e', "enu", "Use the following as origin to solve for East/North/Up coordinates, formatted as a string: \"X Y Z\" "),
      elevationMaskOption('l',"elevation-mask","Elevation mask (degrees)."),
      searchNearOption('x',"no-closest-ephemeris","Allow ephemeris use outside of fit interval."),
      smootherOption('c',"no-carrier-smoothing","Do NOT use carrier phase smoothing."),
      logfileOption('g',"logfile","Write logfile to this file." ),
      rateOption('r',"rate","Observation interval (default=30s or Rinex Header specification",false),
      hasBCEstore(false)
  {
    obsOption.setMaxCount(1);
    navOption.setMaxCount(1);
    metOption.setMaxCount(1);
    spsOption.setMaxCount(1);
    ppsOption.setMaxCount(1);
    searchNearOption.setMaxCount(1);
    elevationMaskOption.setMaxCount(1);
    rateOption.setMaxCount(1);

    enuOption.setMaxCount(1);
    timeFormatOption.setMaxCount(1);
    ionoOption.setMaxCount(1);
    smootherOption.setMaxCount(1);
    logfileOption.setMaxCount(1);
    
    gotMet = false;
    spsSolution = false;
    ppsSolution = false;
    aprioriPositionDefined = false;
    transformENU = false;
    removeIonosphere = true;
    searchNear = false;
    useSmoother = true;
    logfileOn = false;

    elevationMask = 0;
    
    logFileName = obsFileName = metFileName = "";
  }


bool RINEXPVTSolution::initialize(int argc, char *argv[])
    throw()
  {
    if(!BasicFramework::initialize(argc, argv))
      return false;
    
    obsFileName = obsOption.getValue().front();

    if(metOption.getCount())
    {
       gotMet = true;
       metFileName = metOption.getValue().front();
    }

    spsSolution = (spsOption.getCount()>0);
    ppsSolution = (ppsOption.getCount()>0);

    if (enuOption.getCount())
    {
       double x, y, z;
       sscanf(enuOption.getValue().front().c_str(),"%lf %lf %lf",
              &x, &y, &z);
       enuOrigin=Triple(x,y,z);
       double mag = enuOrigin.mag();
       upVector = Triple( enuOrigin[0] / mag, enuOrigin[1]/mag, enuOrigin[2]/mag);
       Triple poleVector(0,0,1.);
       if (enuOrigin[2]<0) poleVector[2]=-1.;
       eastVector = upVector.cross(poleVector);
       northVector = upVector.cross(eastVector);

       transformENU = true;
       
    }
    
    if (timeFormatOption.getCount()>0)
    {
       epochFormat = timeFormatOption.getValue().front();
    } 
    else 
       epochFormat = string("%Y %m %d %02H %02M %02S");

    if (elevationMaskOption.getCount()>0)
    {
          elevationMask =
             StringUtils::asFloat(elevationMaskOption.getValue().front());
    }
    
    searchNear       = (searchNearOption.getCount()>0);
    removeIonosphere = (ionoOption.getCount()==0);
    useSmoother      = (smootherOption.getCount()==0);
    
    if (logfileOption.getCount()>0)
    {
       logFileName = StringUtils::asString(logfileOption.getValue().front());
       logStream.open( logFileName.c_str() );
       if (logStream.is_open())
       {
          logfileOn = true;
          logStream << "! rinexpvt log file" << endl;
          DayTime nowTime;
          logStream << "! Executed at: " << nowTime.printf(epochFormat) << endl;
          logStream << "! Obs file name: " << obsFileName << endl;
          logStream << "! Met file name: ";
          if (gotMet) logStream << metFileName << endl;
           else logStream << "none" << endl;
          logStream << "! ENU Transform :";
          if (!transformENU) logStream << " No " << endl;
           else
           {
              logStream << "Yes.  Reference Position (m) = ";
              logStream.setf(ios_base::fixed, ios_base::floatfield);
              logStream.width(12);
              logStream.precision(3);
              logStream << enuOrigin.theArray[0] << ", " 
                        << enuOrigin.theArray[1] << ", "
                        << enuOrigin.theArray[2] << endl;
              logStream.setf(ios_base::fmtflags(0), ios_base::floatfield);  // reset to default
           }
          logStream << "!  Search near? ";
          if (searchNear) logStream << "yes" << endl;
           else logStream << "no" << endl;
          logStream << "!  Remove iono? ";
          if (removeIonosphere) logStream << "yes" << endl;
           else logStream << "no" << endl;
          logStream << "! Use smoother? ";
          if (useSmoother) logStream << "yes" << endl;
           else logStream << "no" << endl;
           
          logStream << "! " << endl;
       }
    }
    
    return true;      
  }


void RINEXPVTSolution::process()
{
    IonoModel spsIonoCorr;
    
    // Read nav file and store unique list of ephemeredes
    if (navOption.getCount()>0)
    {       
       RinexNavStream rnffs(navOption.getValue().front().c_str());
       RinexNavData rne;
       RinexNavHeader hdr;
       rnffs >> hdr;
       spsIonoCorr = IonoModel(hdr.ionAlpha, hdr.ionBeta);
       while (rnffs >> rne) bcestore.addEphemeris(rne);
       if (searchNear) bcestore.SearchNear();
       virtualEphStore = &bcestore;
       hasBCEstore = true;
       if (logfileOn)
       {
          logStream << "! Rinex nav file : " << navOption.getValue().front() << endl;
       }
    }

    //
    if (peOption.getCount()>0)
    {
       for (int i=0; i<peOption.getCount(); i++)
       {
          sp3store.loadFile(peOption.getValue()[i].c_str());
          if (logfileOn)
          {
             logStream << "! Precise ephemeris file : " << peOption.getValue()[i] << endl;
          }
       }
       virtualEphStore = &sp3store;
    }
    

    // If provided, open and store met file into a linked list.
    list<RinexMetData> rml;
    if (metFileName != "")
    {
	    RinexMetStream rms(metFileName.c_str());
	    RinexMetHeader rmh;
	    rms >> rmh;
	
	    RinexMetData rmd;
	    while (rms >> rmd) rml.push_back(rmd);
    }
    
    // Open and read the observation file one epoch at a time.
    // For each epoch, compute and print a position solution
    RinexObsStream roffs(obsFileName.c_str());
    roffs.exceptions(ios::failbit);
    
    RinexObsHeader roh;
    RinexObsData rod;
    
    roffs >> roh;

    if ((!aprioriPositionDefined) && (roh.valid & RinexObsHeader::antennaPositionValid) )
    {
       aprioriPosition = roh.antennaPosition;
       aprioriPositionDefined = true;
    }

    double obsInterval = 30.0;
    bool intervalDefined = false;
    if (roh.valid & RinexObsHeader::intervalValid)
    {
	    obsInterval = roh.interval;
	    intervalDefined = true;
    }
    
    if (rateOption.getCount()>0)
    {
       obsInterval =
             StringUtils::asFloat(rateOption.getValue().front());
       if (logfileOn)
       {
          logStream << "! Observation Interval set to " << obsInterval << "s" << endl;
       }
    }

       // Determine if we can have access to dual frequency measurements.
    bool p1Found(false), p2Found(false), caFound(false);
    std::vector<RinexObsHeader::RinexObsType>::iterator i;
    for (i = roh.obsTypeList.begin(); i != roh.obsTypeList.end(); i++)
    {
       if ((*i)== RinexObsHeader::P1) p1Found = true;
       if ((*i)== RinexObsHeader::P2) p2Found = true;
       if ((*i)== RinexObsHeader::C1) caFound = true;
    }

    if ( !p1Found || !p2Found )
       spsSolution = true;

    if ((spsOption) && !caFound)
    {
          // Should probably use a throw here
       cerr << "RINEX header claims CA observations cannot be found." << endl;
       exit(-1);
    }

        // Tweaking the PRSolution object
    prSolver.RMSLimit = 400;
    //prSolver.Debug    = true;

       // Engage default weather model. Start weather data list at the beginning.
    ggTropModel.setWeather(20., 1000., 50.);    
    list<RinexMetData>::iterator mi=rml.begin();
    
    while (roffs >> rod)
    {
	    double T, P, H;
	
	       // Find a weather point.
	    while ( (gotMet) &&
		         (!rml.empty()) &&
               (mi != rml.end()) &&
		         ((*mi).time < rod.time) )
	    {               
          ggTropModel.setWeather((*mi).data[RinexMetHeader::TD],
                                 (*mi).data[RinexMetHeader::PR],
                                 (*mi).data[RinexMetHeader::HR]);
          mi++;
	    }
	
	
	       // Apply editing criteria 
	    if  (rod.epochFlag == 0 || rod.epochFlag == 1) // Begin usable data
       {
	       vector<SatID> satVec;
          vector<double> rangeVec;
          Xvt svpos;
          double ionocorr;

	       try 
          {	
	          RinexObsData::RinexSatMap::const_iterator it;
             for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
             {
		          RinexObsData::RinexObsTypeMap otmap = (*it).second;

                svpos = virtualEphStore->getXvt((*it).first,rod.time);
                double elevation = aprioriPosition.elvAngle(svpos.x);
                double azimuth =  aprioriPosition.azAngle(svpos.x);
                
                bool healthy=true;
                if (hasBCEstore)
                {
                   if (bcestore.getSatHealth((*it).first,rod.time)!=0)
                      healthy=false;
                }
                
                   // Editing criteria
                if ( (elevation >= elevationMask)  &&
                     (healthy) )
                {
                
                   if ((spsSolution) && (!ppsSolution))
                   {
                      RinexObsData::RinexObsTypeMap::const_iterator itCA = 
                         otmap.find(RinexObsHeader::C1);
                      RinexObsData::RinexObsTypeMap::const_iterator itL1 = 
                         otmap.find(RinexObsHeader::L1);

                      ionocorr = 0;
                   
                      if ((aprioriPositionDefined) && (removeIonosphere))
                         ionocorr = spsIonoCorr.getCorrection(rod.time, 
                                                           aprioriPosition,
                                                           elevation, azimuth);
                      satVec.push_back((*it).first);
                      double range  = (*itCA).second.data-ionocorr;

                      if ((useSmoother) && (itL1 != otmap.end()))
		                {
                         double phase = ((*itL1).second.data)*C_GPS_M / L1_FREQ
                                    + ionocorr;
                         range = carrierPhaseSmooth( (*it).first, range, phase, 
                                                 rod.time, 300.0, obsInterval);
		                }

                      rangeVec.push_back(range);
                   }
                   else
                   {
                      ionocorr = 0;

                      RinexObsData::RinexObsTypeMap::const_iterator itP1, itP2, itL1, itL2;
                      itP1 = otmap.find(RinexObsHeader::P1);
		                itP2 = otmap.find(RinexObsHeader::P2);
                      itL1 = otmap.find(RinexObsHeader::L1);
                      itL2 = otmap.find(RinexObsHeader::L2);
		
                      if ((removeIonosphere) &&
                          (itP1!=otmap.end()) && (itP2!=otmap.end()))
                         ionocorr = 1./(1.-gamma)*((*itP1).second.data-(*itP2).second.data);                
                      
                      if (fabs(ionocorr) < maxIonoDelay)
                      {
                         satVec.push_back((*it).first);
                         double range = (*itP1).second.data-ionocorr;
                       
                         if ( (useSmoother) && (itL1!=otmap.end()) && (itL2!=otmap.end()) )
                         {
                             double ionocorrPhase = -1./(1.-gamma)*((*itL1).second.data*C_GPS_M / L1_FREQ-(*itL2).second.data*C_GPS_M / L2_FREQ);
                             double phase = (*itL1).second.data * C_GPS_M / L1_FREQ - ionocorrPhase;
			                    range = carrierPhaseSmooth( (*it).first, range, phase, 
                                                          rod.time, 86400.0, obsInterval);
		                   }
                         rangeVec.push_back(range);
                      }

                   }
                } // If above elevation mask
                
             }   
       
                // Log file output
                //  epoch time #Obs : <list of PRN IDs> : #GoodSVs [V}NV]
             if (logfileOn)
             {
                logStream << rod.time.printf(epochFormat) << " ";
                logStream << rod.obs.size() << " ! ";
                RinexObsData::RinexSatMap::const_iterator it;
                vector<SatID>::const_iterator itSol;
                itSol = satVec.begin();
                SatID solID = (SatID) (*itSol);
                for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
                {
                   const SatID& satID = (SatID) (*it).first;
                   // cerr << "satID, solID : " << satID << ", " << solID << endl;
                   if (satID.id==solID.id)
                   {
                      logStream << satID.id << " ";
                      itSol++;
                      solID = (SatID) (*itSol);
                      // cerr << "incrementing itSol." << endl;
                   }
                   else
                   {
                      logStream << "(" << satID.id << ") ";
                   }
                }
                logStream << "! ";
                if (prSolver.isValid()) 
                   logStream << prSolver.Nsvs << " " << "V" << endl;
                 else 
                   logStream << "0 NV" << endl;
             }         
 
             prSolver.RAIMCompute(rod.time,satVec,rangeVec, *virtualEphStore, \
	   		            &ggTropModel);
	       }
          catch (Exception e) {
             cerr << e << endl;
	       }
          catch (...) {
             cerr << "Unknown exception occured." << endl;
          }
            
	       if (prSolver.isValid())
          { 
                // Output epoch tag
             cout << rod.time.printf(epochFormat) << " ";
               
             if (!transformENU)
             {
                cout << setprecision(12) << prSolver.Solution[0] << " " ;
                cout << prSolver.Solution[1] << " " ;  
		          cout << prSolver.Solution[2] << " " ;
                cout << prSolver.Solution[3];
		          cout << endl ;
             }
             else
             {
                Triple x(prSolver.Solution[0] - enuOrigin[0],
                         prSolver.Solution[1] - enuOrigin[1],
                         prSolver.Solution[2] - enuOrigin[2]);
                  
                cout << setprecision(12) << x.dot(eastVector) << " " ;
                cout << x.dot(northVector) << " " ;
		          cout << x.dot(upVector) << " ";
                cout << prSolver.Solution[3];
                  
		          cout << endl;
             }

             if (!aprioriPositionDefined)
                aprioriPosition = Triple(prSolver.Solution[0], 
                                         prSolver.Solution[1],
                                         prSolver.Solution[2]);
          }
	    
	    } // End usable data
	
    } // End loop through each epoch
}

const double RINEXPVTSolution::gamma = (L1_FREQ / L2_FREQ)*(L1_FREQ / L2_FREQ);
const double RINEXPVTSolution::maxIonoDelay = 1000;    

int main(int argc, char *argv[])
{
  try
    {
      RINEXPVTSolution rinexpvt(argv[0]);
      
      if(!rinexpvt.initialize(argc, argv))
	return 0;
      
      if(!rinexpvt.run())
	return 1;
      
      return 0;
    }
  catch(Exception& exc)
    {
      cout << exc << endl;
    }
  catch(...)
    {
      cout << "Caught an unknown exception." << endl;
    }
  return 1;
}






