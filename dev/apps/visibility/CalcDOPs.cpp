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
//  but WITHOUT ANY WARRANTY; without even the implied warranty
//  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/** ----------------------------------------------------------------------------------
* @file CalcDOPs
*
* Version 1.0 for GPS-Tk
*
* Compute and display satellite visibility information.
* Inputs SV almanac data (one file per day of observation) and calculates
*   DOP values from that data using standard methods.  See for example:
*   1. AIAA GPS Theory and Applications vol. 1, Ed. Parkinson & Spilker, p414.
*   2. GPS Signals, Measurements, and Performance, 2ed., Misra & Enge, p203.
* Input almanac may be FIC, FICA or RINEX file type -- program auto-detects.
*
* Original version written by Brian Tolman <btolman@arlut.utexas.edu> Jun 2006.
* Addition of new features by Dave Rainwater  <rain@arlut.utexas.edu> Oct 2008.
*
* Command line options:
*   -i <inputfile>      input file for day to be calculated [required]
*   -p <inputfile>      input file for previous day [optional for ephemeris mode]
*   -o <outputfile>     grid  output file [DOPs.out]
*   -tf <timesfile>     times output file [DOPs.times]
*   -sf <statsfile>     stats output file [DOPs.stat]
*   -l <logfile>        log   output file [DOPs.log]
*   -rs                 read from stats file
*   -a                  run in almanac mode (ephemeris mode is default)
*   -w <week> -s <sow>  starting time tag
*   -x <prn>            exclude satellite PRN (may be used multiple times)
*   -t <dt>             time spacing
*   -na                 do North America only [world default]
*   -d                  dump grid results to file for each time step (time-intensive)
*   -h, --help          output options info and exit
*   -v                  output version info and exit
*
* Abort/failure error codes given on return:
*   -1  could not open input data file
*   -2  could not identify input data file type
*   -3  fewer than 4 satellite almanacs available
*   -4  could not allocate GridStats data types
*   -5  could not open input  stats file
*   -6  could not open output grid  file
*   -7  could not open output stats file
*   -8  could not open output log   file
*
* Essential variables not documented below at declaration:
*
*   NtrofN   # of cells/times with < 5 SVs visible during the time period
*   NpeakH   # cells/times w/ HDOP > 10
*   NpeakP   # cells/times w/ PDOP > 10
*   IworstN  index in Grid[] of cell with worst nsvs (number of satellites)
*   IworstH  index in Grid[] of cell with worst HDOP
*   IworstP  index in Grid[] of cell with worst PDOP
*   WorstN   value of nsvs at IworstN
*   WorstH   value of HDOP at IworstH
*   WorstP   value of PDOP at IworstP
*   TworstN  time tag (DayTime class) of WorstN
*   TworstH  time tag (DayTime class) of WorstH
*   TworstP  time tag (DayTime class) of WorstP
*
* Notes:
*
*   1. GPS only, using PRNs hard-wired to SV #s 1-32
*   2. Elevation limit is hard-wired to 5 degrees above horizion.
*   3. "North America" means the northern half-hemisphere: -180 to 0 deg long.,
*      0 to 90N latitude.
*   4. Ephemeris mode is default, almanac mode is optional.  Ephemeris mode is
*      preferred, because it excludes unhealthy satellites for any time when they
*      transmitted an unhealthy flag.  Almanac mode will generally not exclude SVs
*      when they were unhealthy (typical), or may erroneously exclude them for an
*      entire day (rarely).
*   5. If 2 input files are given, the default start time is midnight on the day
*      to be calculated.  A previous-day input file can be given only in ephemeris
*      mode, not almanac.
*   6. The code uses geodetic coordinates for all calculations.
*   7. The -d option is useful for e.g. making movies of DOPs throughout a day.
*
*-----------------------------------------------------------------------------------*/

// C++ libraries
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <iostream>
#include <fstream>

// GPS-Tk modules
#include "FICStream.hpp"
#include "FICAStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "FICFilterOperators.hpp"
#include "RinexNavFilterOperators.hpp"
#include "FileFilterFrame.hpp"
#include "DayTime.hpp"
#include "StringUtils.hpp"
#include "Matrix.hpp"
#include "Position.hpp"
#include "Stats.hpp"
#include "WGS84Geoid.hpp"
#include "icd_200_constants.hpp"
#include "geometry.hpp"
#include "GPSEphemerisStore.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------

bool debug = false;   // used only during development
double version = 1.0; // GPS-Tk version number

// input

bool readStats = false; // if true, read initial statistics from a file
                        // (which was created in an earlier run of this program)
bool NAonly    = false; // if true, limit consideration to North America; else world
bool dumpeach  = false; // if true, dump grid results to a file at each time step
int week = -1;          // input initial timetag -- if not present in input, use the
double sow = 0;         // TOA of the first almanac
vector<int> ExPRN;      // PRNs to exclude from processing
string inputfile;               // input file name (FIC,FICA or RINEX nav)
string previnputfile;           // input file name for previous day (")
string logfile("DOPs.log");     // output file name for log
string outputfile("DOPs.out");  // output file name for grid
string timesfile("DOPs.times"); // output file name for time steps
string statsfile("DOPs.stat");  // output file name for statistics

// mode

bool ephmode = true; // use nearest ephemeris info, don't build an almanac

// these data objects store the almanac data per satellite PRN

list<AlmOrbit> aolist;
map<int,AlmOrbit> aomap;   // map <PRN, almanac orbit> (but it's not yet built)
GPSEphemerisStore ges;     // for mode using ephemeris info, not building an almanac

// structure used to hold grid information, including some temporary space for DOPs

typedef struct
{
   double lat,lon;
   double gdop,pdop;
   double hdop,vdop;
   double tdop,nsvs;
   double wgdop,wpdop;
   double whdop,wvdop;
   double wtdop,wnsvs;
   double bdop;
} GridData;

// grid specifications

const double GridSpace = 111111.1; // grid spacing in km
int MaxNLon;                       // this will be the width (# of lons) in grid
vector<GridData> Grid;             // Grid is a vector of GridData structures;
                                   // each is a (lon,lat,data) pt. for one timept.
WGS84Geoid WGS84;

// computing on the grid

double dlon;                  // dlon is the spacing in lon on the equator
vector<int> Sats;             // satellite PRNs available in the AlmOrbit map aomap
Xvt SVPVT;                    // satellite info used to calculate SV position
Position SV;                  // dummy variable for satellite position
vector<Position> SVs;         // satellite position array used for each time step;
                              // a SV is added to SVs if it's valid
class M4 : public Matrix<double> // trick to declare a 4x4 double array of Matrix
{
public:
  M4() : Matrix<double>(4,4) {};
};
vector<M4> Rmat; // vector of XYZ->UENT coordinate transform 4x4 matrices

// time averaging

double dt = 300.0;  // timestep in seconds
int nt;             // number of timesteps
GridData AvgWorstSiteDOP;  // worst-site DOPs (on grid) averaged over the day

// results on the grid

string dumpfile;                   // filename for dumping the grid every time step
Stats<double> *GGridStats,*PGridStats,*HGridStats,*VGridStats,*TGridStats,*NGridStats;  // stats on the grid (see Stats.hpp)
vector<double> WGTimeStats,WPTimeStats,WHTimeStats,WVTimeStats,WTTimeStats,WNTimeStats; // stats @ timesteps
double *BadPDOP;
int IworstN, IworstG, IworstP, IworstH, IworstV, IworstT; // index in Grid[] of cell/time with worst #SVs,DOPs
int NtrofN , NpeakG , NpeakP , NpeakH , NpeakV , NpeakT ; // number of cells/times with < 5 SVs vis; DOP > 10
double  WorstN , WorstG , WorstP , WorstH , WorstV , WorstT ; // value of nsvs, DOPs at Grid[Iworst..]
DayTime TworstN, TworstG, TworstP, TworstH, TworstV, TworstT; // timetags of IworstN, etc.
double  StepWorstN, StepWorstG, StepWorstP, StepWorstH, StepWorstV, StepWorstT ; // worst values @ each timept.

// logfile and timesfile output streams
ofstream lofs;
ofstream tofs;

//------------------------------------------------------------------------------------
// prototypes in this module

bool isFICFile(const string& file);
bool isFICAFile(const string& file);
bool isRinexNavFile(const string& file);
int ReadDataFile(string filename);
int ReadFICFile(string filename);
int ReadFICAFile(string filename);
int ReadRinexFile(string filename);
int ReadStatsFile(string filename);
int WriteStatsFile(string filename);
int OutputGrid(string filename);
int DumpGrid(DayTime& tt, string filename);
void BuildGrid(void);
void ComputeDOPs(DayTime& tt, GridData& gd, vector<Position>& SVs, M4& R);

//------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
try
{
   // if no options are given on command line, print syntax and quit
  if (argc < 2 || string(argv[1]) == "-h" || string(argv[1]) == "--help")
   {
      cout << "Program CalcDOPs reads an FIC, FICA or a Rinex Nav file" << endl
           << "Usage:  CalcDOPs -i  <inputfile>     input file for day to be calculated (required)" << endl
           << "                 -p  <inputfile>     input file for previous day (optional, ephemeris mode only)" << endl
           << "                 -o  <outputfile>    grid  output file [DOPs.out]" << endl
           << "                 -sf <outputfile>    stats output file [DOPs.stat]" << endl
           << "                 -tf <outputfile>    time steps output file [DOPs.times]" << endl
           << "                 -l  <outputfile>    log   output file [DOPs.log]" << endl
           << "                 -rs                 read from stats file" << endl
           << "                 -a                  work in almanac mode [ephemeris mode is default]" << endl
           << "                 -w <week> -s <sow>  starting time tag" << endl
           << "                 -x <prn>            exclude satellite PRN" << endl
           << "                 -t <dt>             time spacing" << endl
           << "                 -na                 do North America only" << endl
           << "                 -d                  dump grid results at each time step (time-intensive)" << endl
           << "                 -h, --help          output options info and exit" << endl
           << "                 -v                  print version info and exit" << endl
           << endl;
      return 0;
   }

  int i, j, ii;

   // parse command line
   i = 1;
   while (i < argc)
   {
          if (string(argv[i]) == "-i" ) inputfile = string(argv[++i]);
     else if (string(argv[i]) == "-p" ) previnputfile = string(argv[++i]);
     else if (string(argv[i]) == "-o" ) outputfile = string(argv[++i]);
     else if (string(argv[i]) == "-l" ) logfile = string(argv[++i]);
     else if (string(argv[i]) == "-tf") timesfile = string(argv[++i]);
     else if (string(argv[i]) == "-a" ) ephmode = false;
     else if (string(argv[i]) == "-w" ) week = atoi(argv[++i]);
     else if (string(argv[i]) == "-s" ) sow = atof(argv[++i]);
     else if (string(argv[i]) == "-x" ) ExPRN.push_back(atoi(argv[++i]));
     else if (string(argv[i]) == "-t" ) dt = atof(argv[++i]);
     else if (string(argv[i]) == "-sf") statsfile = string(argv[++i]);
     else if (string(argv[i]) == "-rs") readStats = true;
     else if (string(argv[i]) == "-na") NAonly = true;
     else if (string(argv[i]) == "-d" ) dumpeach = true;
     else if (string(argv[i]) == "-v" )
     {
       cout << "CalcDOPs version " << fixed << setprecision(1) << setw(3) << version << endl;
       return 0;
     }
     else cout << "Unrecognized option: " << argv[i] << endl;
     i++;
   }

   lofs.open(logfile.c_str(),ios_base::out);
   if (!lofs) return -8;

   tofs.open(timesfile.c_str(),ios_base::out);
   if (!tofs) return -8;

   // reassurance print
   lofs << "Program visible with:" << endl << "current-day input file " << inputfile << endl;
   if ( previnputfile != "" ) lofs << "and previous-day input file " << previnputfile << endl;
   lofs << "and output file " << outputfile << endl;
   if (week > 0) lofs << " Input time tag: " << week << " " << sow << endl;
   if (ExPRN.size() > 0)
   {
      lofs << " Exclude satellite PRNs";
      for(i=0; i<ExPRN.size(); i++) lofs << " " << ExPRN[i];
      lofs << "." << endl;
   }

   // compute the number of time steps from the time spacing
   nt = int(86400.0/dt); // 86400 = 60*60*24 = sec/day

   // open and read the previous day's input data file first, if specified and in Eph mode
   if ( previnputfile != "" && ephmode )
   {
     lofs << "Reading in previous-day input file..." << endl;
     i = ReadDataFile(previnputfile);
     if (i)
     {
       if (i == -1) lofs << "Previous-day input file does not exist. Abort." << endl;
       if (i == -2) lofs << "Cannot identify previous-day file type. Abort." << endl;
       return i;
     }
   }

   // open and read the current day's input data file
   lofs << "Reading in current-day input file..." << endl;
   i = ReadDataFile(inputfile);
   if (i)
   {
     if (i == -1) lofs << "Current-day input file does not exist. Abort." << endl;
     if (i == -2) lofs << "Cannot identify current-day file type. Abort." << endl;
     return i;
   }

   // build the spatial grid, and store it in vector<GridData> Grid;
   BuildGrid();

   // get a list of the available satellite PRNs and the initial timetag
   bool ok;
   DayTime tt, starttime;
   bool initialTimeSet = false;
   if ( ephmode ) // ephemeris mode
   {
     try
     {
       DayTime earliest = ges.getInitialTime();
       DayTime latest = ges.getFinalTime();
//       DayTime start = latest - gpstk::DayTime::SEC_DAY/2; /* make sure you're in the right day */
       DayTime start = latest - 6*3600.0; /* go back 6 h: covers any 4 h ephemeris going into the next day */
       tt = DayTime( start.year(), start.month(), start.day(), 0, 0, 0.0 );
       starttime = tt;
       lofs << " Initial time tag is " << tt.printf("%4F %8.1g") << endl;
       initialTimeSet = true;
     }
     catch(InvalidRequest)
     {
       i = -100;
       lofs << "Initial Time or Final Time missing. Abort." << endl;
       return i;
     }
     for (i=1; i<33; i++)
     {
       for (ok=true,j=0; j<ExPRN.size(); j++)
       {
         if (ExPRN[j] == i) { ok=false; break; }
       }
       if (!ok) continue;                     // skip this satellite
       Sats.push_back(i);                     // save this satellite
     }
   }
   else           // almanac mode (original version)
   {
     DayTime start(DayTime::BEGINNING_OF_TIME); // Declare and initialize to something guaranteed to be early.
     for (i=1; i<33; i++) // # of SVs hard-wired to 32
     {
       if (aomap.find(i) == aomap.end()) continue; // satellite not found in almanac
       for (ok=true,j=0; j<ExPRN.size(); j++)
       {
         if (ExPRN[j] == i) { ok=false; break; }
       }
       if (!ok) continue;                     // skip this satellite if not ok

       Sats.push_back(i);                     // save this satellite otherwise

       // store latest transmit time tag of the set.
       if (aomap[i].getTransmitTime()>start) start = aomap[i].getTransmitTime();
     }
     // Set starting time to beginning of day in which majority of almanac was collected.
     tt = DayTime( start.year(), start.month(), start.day(), 0, 0, 0.0 );
     starttime = tt;
     lofs << " Initial time tag is " << tt.printf("%4F %8.1g") << endl;
     initialTimeSet = true;
   }
   if (Sats.size() < 4)
   {
     lofs << "Fewer than 4 satellite almanacs are available - abort." << endl;
     return -3;
   }

   // allocate Stats objects for each grid point's DOPs
   GGridStats = new Stats<double>[Grid.size()];    
   PGridStats = new Stats<double>[Grid.size()];
   HGridStats = new Stats<double>[Grid.size()];
   VGridStats = new Stats<double>[Grid.size()];
   TGridStats = new Stats<double>[Grid.size()];
   NGridStats = new Stats<double>[Grid.size()];
   BadPDOP = (double*)calloc(Grid.size(),sizeof(double));
   if (!GGridStats || !PGridStats || !HGridStats ||
       !VGridStats || !TGridStats || !NGridStats   )
   {
      lofs << "Failed to allocate GridStats" << endl;
      return -4;
   }

   // initialize storage of 'worsts' and 'peaks'
   IworstN = IworstG = IworstP = IworstH = IworstV = IworstT = -1; // indexes of worst Number and worst DOPs
   NtrofN  = NpeakG  = NpeakP  = NpeakH  = NpeakV  = NpeakT  =  0; // number of cells with DOP > 10, # with < 5 sats

   // if reading a stats file (-rs), initialize stats using data from a file
   if (readStats)
   {
      i = ReadStatsFile(statsfile);
      if (i)
      {
         lofs << "Could not open stats file for input. Abort." << endl;
         return i;
      }
   }

   // compute away
   dlon = 360.0/double(MaxNLon);

   for (j=0; j<nt; j++)             // LOOP OVER TIMES
   {
      SVs.clear();                  // clear SV position array

      for (i=0; i<Sats.size(); i++) // LOOP OVER SVs -- get positions at each time step
      {
        if (ephmode)  // ephemeris mode
        {
          SatID sid(Sats[i],SatID::systemGPS);
          try
          {
            if (ges.getSatHealth(sid,tt)!=0) continue;
            SVPVT = ges.getXvt(sid,tt);
          }
          catch(InvalidRequest& e)
          {
            continue;
          }
          SV.setECEF(SVPVT.x[0],SVPVT.x[1],SVPVT.x[2]); // get SV position
          SVs.push_back(SV);                            // add to the vector
        }
        else          // almanac mode
        {
          SVPVT = aomap[Sats[i]].svXvt(tt);
          SV.setECEF(SVPVT.x[0],SVPVT.x[1],SVPVT.x[2]); // get SV position
          SVs.push_back(SV);                            // add to the vector
        }
      }

      // zero worst-site DOPs (worst #SVs to large #) for this time step

      StepWorstG = StepWorstP = StepWorstH = StepWorstV = StepWorstT = 0.;
      StepWorstN = 10000.;

      for (i=0; i<Grid.size(); i++) // LOOP OVER GRID POSITIONS
      {
        if ( j == 0 ) // set up grid position vector only on first time step
        {
          // transform XYZT to UENT: R*Vector(XYZT) = Vector(UENT)
          double ca,sa,co,so;
          Position Rx(Grid[i].lat,Grid[i].lon,0.0,Position::Geodetic); // grid position
          ca = cos(Rx.geodeticLatitude()*DEG_TO_RAD);
          sa = sin(Rx.geodeticLatitude()*DEG_TO_RAD);
          co = cos(Rx.longitude()*DEG_TO_RAD);
          so = sin(Rx.longitude()*DEG_TO_RAD);
          M4 Rtemp;
          Rtemp(0,0) = ca*co ; Rtemp(0,1) = ca*so ; Rtemp(0,2) = sa ; Rtemp(0,3) = 0.0;
          Rtemp(1,0) = -so   ; Rtemp(1,1) = co    ; Rtemp(1,2) = 0.0; Rtemp(1,3) = 0.0;
          Rtemp(2,0) = -sa*co; Rtemp(2,1) = -sa*so; Rtemp(2,2) = ca ; Rtemp(2,3) = 0.0;
          Rtemp(3,0) = 0.0   ; Rtemp(3,1) = 0.0   ; Rtemp(3,2) = 0.0; Rtemp(3,3) = 1.0;
          Rmat.push_back(Rtemp); // add this grid point's R matrix to the stack
        }

        ComputeDOPs(tt,Grid[i],SVs,Rmat[i]); // compute DOPs

        BadPDOP[i] = BadPDOP[i] + Grid[i].bdop; // adds up each grid pt.'s BDOP over all times
                                                // BDOP for a single pt. is 0 or 1 for PDOP <= v. > 6

        // add to stats -- each GridStats object ends up holding all times for a grid point

        GGridStats[i].Add(Grid[i].gdop);
        PGridStats[i].Add(Grid[i].pdop);
        HGridStats[i].Add(Grid[i].hdop);
        VGridStats[i].Add(Grid[i].vdop);
        TGridStats[i].Add(Grid[i].tdop);
        NGridStats[i].Add(Grid[i].nsvs);

        // save the worst and the peaks

        if (Grid[i].gdop > StepWorstG)
        {
          StepWorstG = Grid[i].gdop;
        }

        if (Grid[i].pdop > StepWorstP)
        {
          StepWorstP = Grid[i].pdop;
        }

        if (Grid[i].hdop > StepWorstH)
        {
          StepWorstH = Grid[i].hdop;
        }

        if (Grid[i].vdop > StepWorstV)
        {
          StepWorstV = Grid[i].vdop;
        }

        if (Grid[i].tdop > StepWorstT)
        {
          StepWorstT = Grid[i].tdop;
        }

        if (Grid[i].nsvs < StepWorstN)
        {
          StepWorstN = Grid[i].nsvs;
        }

        if (IworstG == -1 || Grid[i].gdop > WorstG)
        {
          IworstG = i;
          WorstG  = Grid[i].gdop;
          TworstG = tt;
        }

        if (IworstP == -1 || Grid[i].pdop > WorstP)
        {
          IworstP = i;
          WorstP  = Grid[i].pdop;
          TworstP = tt;
        }

        if (IworstH == -1 || Grid[i].hdop > WorstH)
        {
          IworstH = i;
          WorstH  = Grid[i].hdop;
          TworstH = tt;
        }

        if (IworstV == -1 || Grid[i].vdop > WorstV)
        {
          IworstV = i;
          WorstV  = Grid[i].vdop;
          TworstV = tt;
        }

        if (IworstT == -1 || Grid[i].tdop > WorstT)
        {
          IworstT = i;
          WorstT  = Grid[i].tdop;
          TworstT = tt;
        }

        if (IworstN == -1 || Grid[i].nsvs < WorstN)
        {
          IworstN = i;
          WorstN  = Grid[i].nsvs;
          TworstN = tt;
        }

        if (Grid[i].nsvs < 5) NtrofN++;

        if (Grid[i].pdop > 10.)
        {
          NpeakP++;
          lofs << "PDS " << NpeakP
               << " " << setw(4) << j+1              // time
               << fixed << setprecision(2)
               << " " << setw(7) << Grid[i].lon
               << " " << setw(7) << Grid[i].lat
               << endl;
        }

        if (Grid[i].hdop > 10.)
        {
          NpeakH++;
          lofs << "HDS " << NpeakH
               << " " << setw(4) << j+1              // time
               << fixed << setprecision(2)
               << " " << setw(7) << Grid[i].lon
               << " " << setw(7) << Grid[i].lat
               << endl;
        }

      } // end loop over grid

      // write timestep results to timesfile

      tofs << " "
           << tt.printf("%4F %8.1g") << "  "
           << " " << setw(7) << StepWorstG
           << " " << setw(7) << StepWorstP
           << " " << setw(7) << StepWorstH
           << " " << setw(7) << StepWorstV
           << " " << setw(7) << StepWorstT
           << " " << setw(6) << StepWorstN
           << endl;

      // dump grid results to file for each time step if enabled

      if (dumpeach)
      {
        stringstream ss;
        ss << j;
        dumpfile = outputfile + ".t-" + ss.str();
        cout << dumpfile << endl;
        ii = DumpGrid(tt, dumpfile);
        if (ii)
        {
          lofs << "Could not dump grid file for writing. Abort." << endl;
          return i;
        }
      }

      // record worst-site DOPs at each time step

      WGTimeStats.push_back(StepWorstG);
      WPTimeStats.push_back(StepWorstP);
      WHTimeStats.push_back(StepWorstH);
      WVTimeStats.push_back(StepWorstV);
      WTTimeStats.push_back(StepWorstT);
      WNTimeStats.push_back(StepWorstN);

      tt += dt;      // increment time tag
   } // end loop over times

   // get day's average of worst-site (grid) DOPs

   for (i=0; i<nt; i++)
   {
     AvgWorstSiteDOP.wgdop += WGTimeStats[i];
     AvgWorstSiteDOP.wpdop += WPTimeStats[i];
     AvgWorstSiteDOP.whdop += WHTimeStats[i];
     AvgWorstSiteDOP.wvdop += WVTimeStats[i];
     AvgWorstSiteDOP.wtdop += WTTimeStats[i];
     AvgWorstSiteDOP.wnsvs += WNTimeStats[i];
   }
   AvgWorstSiteDOP.wgdop /= nt;
   AvgWorstSiteDOP.wpdop /= nt;
   AvgWorstSiteDOP.whdop /= nt;
   AvgWorstSiteDOP.wvdop /= nt;
   AvgWorstSiteDOP.wtdop /= nt;
   AvgWorstSiteDOP.wnsvs /= nt;

   // output the grid itself and the stats - for use later

   i = OutputGrid(outputfile); // output average and worst-site DOP results over the grid
   if (i)
   {
      lofs << "Could not output file for writing. Abort." << endl;
      return i;
   }

   i = WriteStatsFile(statsfile);
   if (i)
   {
      lofs << "Could not open stats file for output. Abort." << endl;
      return i;
   }

   // clean up

   delete[] GGridStats;
   delete[] PGridStats;
   delete[] HGridStats;
   delete[] VGridStats;
   delete[] TGridStats;
   delete[] NGridStats;

   lofs.close();
}
catch(Exception& e) {
   lofs << "Caught an exception" << endl << e << endl;
}
   return 0;
}

//------------------------------------------------------------------------------------

AlmOrbit ConvertEEtoAO(EngEphemeris& ee)
{
   AlmOrbit ao(
      ee.getPRNID()   , ee.getEcc()   , ee.getI0()-0.3*PI, ee.getOmegaDot()      ,
      ee.getAhalf()   , ee.getOmega0(), ee.getW()        , ee.getM0()            ,
      ee.getAf0()     , ee.getAf1()   , long(ee.getToe()), long(ee.getHOWTime(1)),
      ee.getFullWeek(), ee.getHealth()
      );

   return ao;
}

//------------------------------------------------------------------------------------

int ReadDataFile(string infile)
{
try
{
   int iret;

   // does input file exist?
   ifstream ifs;
   ifs.open(infile.c_str(),ios_base::in);
   if (!ifs) return -1;
   ifs.close();

   if (isRinexNavFile(infile))
      iret = ReadRinexFile(infile);
   else if (isFICAFile(infile))
      iret = ReadFICAFile(infile);
   else if (isFICFile(infile))
      iret = ReadFICFile(infile);
   else
      return -2;                      // not a known file type

   if (iret)
   {
      lofs << "Reading failed (" << iret << ")" << endl;
      return iret;  // appears to be able to return only 0 right now
   }

   lofs << "Here is the AO list (" << aomap.size() << ") :";
   map<int,AlmOrbit>::iterator itao = aomap.begin();
   while (itao != aomap.end())
   {
      //itao->second.dump(lofs);
      lofs << " " << itao->first;
      itao++;
   }
   lofs << endl << endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

bool isFICFile(const string& file)
{
try
{
   FileFilterFrame<FICStream, FICData> data(file.c_str());
   return true;
}
catch(FFStreamError& ffse) { return false; }
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

bool isFICAFile(const string& file)
{
try
{
   FICAStream fics(file.c_str());
   FICHeader header;
   header.getRecord(fics);
   fics.close();
   FileFilterFrame<FICAStream, FICData> input(file.c_str());
   return true;
}
catch(FFStreamError& ffse) { return false; }
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

bool isRinexNavFile(const string& file)
{
try
{
   RinexNavHeader header;
   RinexNavStream rnstream(file.c_str());
   rnstream.exceptions(fstream::failbit);
   try { rnstream >> header; } catch(Exception& e) { return false; }
   rnstream.close();
   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

int ReadFICFile(string infile)
{
try
{
   lofs << "Its an FIC file" << endl;
   FileFilterFrame<FICStream, FICData> input(infile);

   list<long> blockList;
   blockList.push_back(9);
   blockList.push_back(62);
   input.filter(FICDataFilterBlock(blockList));

   list<FICData> ficList = input.getData();
   list<FICData>::iterator itr = ficList.begin();
   while (itr != ficList.end())
   {
      if (itr->blockNum == 62)
      {
         AlmOrbit ao(*itr);
         if (debug) lofs << "Its a 62 for PRN " << ao.getPRNID();
         if (ao.getSVHealth() != 0) { itr++; continue; } // don't use if flag is unhealthy
         aolist.push_back(ao);
         if (aomap.find(ao.getPRNID()) == aomap.end())
         {
            if (debug) lofs << " -- add it";
            aomap[ao.getPRNID()] = ao;
         }
         if (debug) lofs << endl;
      }
      if (itr->blockNum == 9)
      {
         FICData& r = *itr;
         EngEphemeris ee(r);
         ges.addEphemeris(ee);
      }
      itr++;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

int ReadFICAFile(string infile)
{
try
{
   lofs << "Its an FICA file" << endl;
   FICAStream fics(infile.c_str());
   FICHeader header;
   header.getRecord(fics);
   fics.close();

   FileFilterFrame<FICAStream, FICData> input(infile.c_str());

   list<long> blockList;
   blockList.push_back(9);
   blockList.push_back(62);
   input.filter(FICDataFilterBlock(blockList));

   list<FICData> ficList = input.getData();
   list<FICData>::iterator itr = ficList.begin();
   while (itr != ficList.end())
   {
      if (itr->blockNum == 62)
      {
         AlmOrbit ao(*itr);
         if (debug) lofs << "Its a 62 for PRN " << ao.getPRNID();
         if (ao.getSVHealth() != 0) { itr++; continue; } // don't use if flag is unhealthy
         aolist.push_back(ao);
         if (aomap.find(ao.getPRNID()) == aomap.end())
         {
            if (debug) lofs << " -- add it";
            aomap[ao.getPRNID()] = ao;
         }
         if (debug) lofs << endl;
      }
      if (itr->blockNum == 9)
      {
         FICData& r = *itr;
         EngEphemeris ee(r);
         ges.addEphemeris(ee);
      }
      itr++;
   }
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

int ReadRinexFile(string infile)
{
try
{
   lofs << "Its a Rinex Nav file" << endl;

   RinexNavHeader rnh;
   RinexNavData rne;
   RinexNavStream RNFileIn(infile.c_str());
   RNFileIn.exceptions(fstream::failbit);
   
   RNFileIn >> rnh;
   while (RNFileIn >> rne)
   {
      EngEphemeris ee(rne);
      ges.addEphemeris(ee);            // add to stack for ephemeris mode
                                       // health bit checked in ComputeDOPs

      AlmOrbit ao = ConvertEEtoAO(ee); // convert to almanac for almanac mode

      if (debug) lofs << "Its a Rinex ephemeris for PRN " << ao.getPRNID();
      if (ao.getSVHealth() != 0) continue; // don't use if flag is unhealthy
      aolist.push_back(ao);
      if (aomap.find(ao.getPRNID()) == aomap.end())
      {
         if (debug) lofs << " -- add it";
         aomap[ao.getPRNID()] = ao;
      }
      if (debug) lofs << endl;
   }
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

int ReadStatsFile(string statsfile)
{
try
{
   ifstream sifs;

   sifs.open(statsfile.c_str());
   if (!sifs) return -5;
   else
   {
      lofs << "Opened stats file for input " << statsfile << endl;
      bool wtd;
      const int BUFF_SIZE=1024;
      char buffer[BUFF_SIZE];
      string line;
      vector<string> fields;
      int i;
      while (sifs.getline(buffer,BUFF_SIZE))
      {
         line = buffer;
         StringUtils::stripTrailing(line,'\r');
         fields.clear();
         for (i=0; i<StringUtils::numWords(line); i++)
         {
            fields.push_back(StringUtils::word(line,i));
         }

         if (fields[0] == string("STAT"))
         {
            i = StringUtils::asInt(fields[1]);
            if (fields[9] == string("Y")) wtd = true; else wtd = false;
            HGridStats[i].Load(
                               (unsigned int)(StringUtils::asInt(fields[4])),
                               StringUtils::asDouble(fields[5]),                // min
                               StringUtils::asDouble(fields[6]),                // max
                               StringUtils::asDouble(fields[7]),                // ave
                               StringUtils::asDouble(fields[8]),                // var
                               wtd,
                               StringUtils::asDouble(fields[10])                // norm
                              );
            if (fields[16] == string("Y")) wtd = true; else wtd = false;
            NGridStats[i].Load(
                               (unsigned int)(StringUtils::asInt(fields[11])),
                               StringUtils::asDouble(fields[12]),               // min
                               StringUtils::asDouble(fields[13]),               // max
                               StringUtils::asDouble(fields[14]),               // ave
                               StringUtils::asDouble(fields[15]),               // var
                               wtd,
                               StringUtils::asDouble(fields[17])                // norm
                              );
         }
         else if (fields[0] == string("WORSTN"))
         {
            IworstN = StringUtils::asInt(fields[1]);
            TworstN.setGPSfullweek(StringUtils::asInt(fields[2]),
        			   StringUtils::asDouble(fields[3]));
            WorstN = StringUtils::asDouble(fields[6]);
            NtrofN = StringUtils::asInt(fields[7]);
         }
         else if (fields[0] == string("WORSTH"))
         {
            IworstH = StringUtils::asInt(fields[1]);
            TworstH.setGPSfullweek(StringUtils::asInt(fields[2]),
        			   StringUtils::asDouble(fields[3]));
            WorstH = StringUtils::asDouble(fields[6]);
            NpeakH = StringUtils::asInt(fields[7]);
         }
         else if (fields[0] == string("WORSTP"))
         {
            IworstP = StringUtils::asInt(fields[1]);
            TworstP.setGPSfullweek(StringUtils::asInt(fields[2]),
        			   StringUtils::asDouble(fields[3]));
            WorstP = StringUtils::asDouble(fields[6]);
            NpeakP = StringUtils::asInt(fields[7]);
         }
      } // end loop over lines in file
   }

   sifs.close();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

int WriteStatsFile(string statsfile)
{
try
{
   ofstream sofs;

   sofs.open(statsfile.c_str(),ios_base::out);
   if (!sofs) return -7;
   else
   {
      bool wtd;
      double W,f;
      int i;
      for (i=0; i<Grid.size(); i++)
      {
         W = 1.0;

         // for completeness, if stats are weighted (they are not in this program's output),
         // they have to be re-normalized....
         wtd = HGridStats[i].Weighted();
         if (wtd)
         {
            W = HGridStats[i].Normalization();
            f = W / double(HGridStats[i].N());
         }
         // write the unweighted stats to the file
         sofs << " " << setw(5) << i
            << fixed << setprecision(2)
            << " " << setw(7) << Grid[i].lon << " " << setw(6) << Grid[i].lat
            << setprecision(10)
            << " " << setw( 4) << HGridStats[i].N()
            << " " << setw(12) << HGridStats[i].Minimum()
            << " " << setw(12) << HGridStats[i].Maximum();
         if (wtd)
         {
            sofs << " " << setw(12) << HGridStats[i].Average()*f
                 << " " << setw(12) << HGridStats[i].Variance()*f*f*f*f
                 << " Y";           // this says 'are they weighted?' (see Stats.hpp in GPS-Tk)
         }
         else
         {
            sofs << " " << setw(12) << HGridStats[i].Average()
                 << " " << setw(12) << HGridStats[i].Variance()
                 << " N";
         }
         sofs << " " << setw(12) << W;
   
         wtd = NGridStats[i].Weighted();
         if (wtd)
         {
            W = NGridStats[i].Normalization();
            f = W / double(NGridStats[i].N());
         }
         sofs << setprecision(10)
            << " " << setw( 4) << NGridStats[i].N()
            << " " << setw(12) << NGridStats[i].Minimum()
            << " " << setw(12) << NGridStats[i].Maximum();
         if (wtd)
         {
            sofs << " " << setw(12) << NGridStats[i].Average()*f
                 << " " << setw(12) << NGridStats[i].Variance()*f*f*f*f
                 << " Y";
         }
         else
         {
            sofs << " " << setw(12) << NGridStats[i].Average()
                 << " " << setw(12) << NGridStats[i].Variance()
                 << " N";
         }
         sofs << " " << setw(12) << W
              << endl;
      }

      // output absolute worst-site DOPs

      sofs << "Abs WORSTN"
           << " " << setw(5) << IworstN
           << TworstN.printf(" %4F %8.1g")
           << fixed << setprecision(2)
           << " " << setw(7) << Grid[IworstN].lon
           << " " << setw(7) << Grid[IworstN].lat
           << " " << setw(5) << WorstN
           << " " << setw(5) << NtrofN
           << endl;
      sofs << "Abs WORSTG"
           << " " << setw(5) << IworstG
           << TworstG.printf(" %4F %8.1g")
           << fixed << setprecision(2)
           << " " << setw(7) << Grid[IworstG].lon
           << " " << setw(7) << Grid[IworstG].lat
           << " " << setw(5) << WorstG
           << " " << setw(5) << NpeakG
           << endl;
      sofs << "Abs WORSTP"
           << " " << setw(5) << IworstP
           << TworstP.printf(" %4F %8.1g")
           << fixed << setprecision(2)
           << " " << setw(7) << Grid[IworstP].lon
           << " " << setw(7) << Grid[IworstP].lat
           << " " << setw(5) << WorstP
           << " " << setw(5) << NpeakP
           << endl;
      sofs << "Abs WORSTH"
           << " " << setw(5) << IworstH
           << TworstH.printf(" %4F %8.1g")
           << fixed << setprecision(2)
           << " " << setw(7) << Grid[IworstH].lon
           << " " << setw(7) << Grid[IworstH].lat
           << " " << setw(5) << WorstH
           << " " << setw(5) << NpeakH
           << endl;
      sofs << "Abs WORSTV"
           << " " << setw(5) << IworstV
           << TworstV.printf(" %4F %8.1g")
           << fixed << setprecision(2)
           << " " << setw(7) << Grid[IworstV].lon
           << " " << setw(7) << Grid[IworstV].lat
           << " " << setw(5) << WorstV
           << " " << setw(5) << NpeakV
           << endl;
      sofs << "Abs WORSTT"
           << " " << setw(5) << IworstT
           << TworstT.printf(" %4F %8.1g")
           << fixed << setprecision(2)
           << " " << setw(7) << Grid[IworstT].lon
           << " " << setw(7) << Grid[IworstT].lat
           << " " << setw(5) << WorstT
           << " " << setw(5) << NpeakT
           << endl;

      // output worst-site DOPs averaged over the day (avg. of worst-site values @ each timept.)

      sofs << "Avg WORSTN "
           << fixed << setprecision(2) << setw(5) << AvgWorstSiteDOP.wnsvs << endl;
      sofs << "Avg WORSTG "
           << fixed << setprecision(2) << setw(5) << AvgWorstSiteDOP.wgdop << endl;
      sofs << "Avg WORSTP "
           << fixed << setprecision(2) << setw(5) << AvgWorstSiteDOP.wpdop << endl;
      sofs << "Avg WORSTH "
           << fixed << setprecision(2) << setw(5) << AvgWorstSiteDOP.whdop << endl;
      sofs << "Avg WORSTV "
           << fixed << setprecision(2) << setw(5) << AvgWorstSiteDOP.wvdop << endl;
      sofs << "Avg WORSTT "
           << fixed << setprecision(2) << setw(5) << AvgWorstSiteDOP.wtdop << endl;

   }
   sofs.close();
   
return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// output grid results to file

int OutputGrid(string outfile)
{
try
{
   ofstream ofs;

   // open output file
   ofs.open(outfile.c_str(),ios_base::out);
   if (!ofs) return -6;
   else lofs << "Opened output file " << outfile << endl;

   if (Grid.size() == 0) return 0;

   int i;

   for (i=0; i<Grid.size(); i++)
   {
      // average values at a grid point over the time periods compiled
      Grid[i].gdop = GGridStats[i].Average();
      Grid[i].pdop = PGridStats[i].Average();
      Grid[i].hdop = HGridStats[i].Average();
      Grid[i].vdop = VGridStats[i].Average();
      Grid[i].tdop = TGridStats[i].Average();
      Grid[i].nsvs = NGridStats[i].Average();
      // worst values at a grid point during all time periods compiled
      Grid[i].wgdop = GGridStats[i].Maximum();
      Grid[i].wpdop = PGridStats[i].Maximum();
      Grid[i].whdop = HGridStats[i].Maximum();
      Grid[i].wvdop = VGridStats[i].Maximum();
      Grid[i].wtdop = TGridStats[i].Maximum();
      Grid[i].wnsvs = NGridStats[i].Minimum();
      // BDOP
      Grid[i].bdop  = BadPDOP[i] / HGridStats[i].N(); // average over day's timepts.

      ofs << " "
        //<< tt.printf("%4F %8.1g")
          << fixed << setprecision(3)
          << " " << setw(7) << Grid[i].lon
          << " " << setw(6) << Grid[i].lat
          << " " << setw(7) << Grid[i].gdop
          << " " << setw(7) << Grid[i].pdop
          << " " << setw(7) << Grid[i].hdop
          << " " << setw(7) << Grid[i].vdop
          << " " << setw(7) << Grid[i].tdop
          << " " << setw(6) << Grid[i].nsvs
          << " " << setw(7) << Grid[i].wgdop
          << " " << setw(7) << Grid[i].wpdop
          << " " << setw(7) << Grid[i].whdop
          << " " << setw(7) << Grid[i].wvdop
          << " " << setw(7) << Grid[i].wtdop
          << " " << setw(6) << Grid[i].wnsvs
          << fixed << setprecision(7)
          << " " << setw(9) << Grid[i].bdop
          << " "
          << endl;
   }

   ofs.close();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// output grid results to file

int DumpGrid(DayTime& time, string dumpfile)
{
try
{
   ofstream ofs;

   // open output file
   ofs.open(dumpfile.c_str(),ios_base::out);
   if (!ofs) return -6;
   else lofs << "Opened output file " << dumpfile << endl;

   if (Grid.size() == 0) return 0;

   int i;

   for (i=0; i<Grid.size(); i++)
   {
      ofs << " "
          << time.printf("%4F %8.1g")
          << fixed << setprecision(3)
          << " " << setw(7) << Grid[i].lon
          << " " << setw(6) << Grid[i].lat
          << " " << setw(7) << Grid[i].gdop
          << " " << setw(7) << Grid[i].pdop
          << " " << setw(7) << Grid[i].hdop
          << " " << setw(7) << Grid[i].vdop
          << " " << setw(7) << Grid[i].tdop
          << " " << setw(6) << Grid[i].nsvs
          << fixed << setprecision(7)
          << " " << setw(9) << Grid[i].bdop
          << " "
          << endl;
   }

   ofs.close();

   return 0;
 }
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

void BuildGrid(void)
{
try
{
   int i,j,n;
   // generate grid uniform spacing
   // circumference of an ellipse is approx PI*a*sqrt(2*(2-e^2))
   Grid.clear();
   int nlat,nlon;
   double lat,lon,radius,dlat,dlon,dist,d;
   Position p;
   // nlat = half circumference (NS) / spacing
   nlat = int(PI*WGS84.a()*SQRT(2*(2-WGS84.eccSquared()))/(2.0*GridSpace));
   dlat = 180.0/double(nlat);  // step in lat
//   cout << "nlat = " << nlat << endl;
//   cout << "dlat = " << dlat << endl;
//   cout << endl;
   n = (nlat-1)/2;
   MaxNLon = -1;
   for (i=0; i<nlat; i++) // latitude is outer loop - this is important
   {
      lat = (i-n)*dlat;        // assume this is geodetic
//      cout << endl;
//      cout << "lat = " << lat << endl;
      if (NAonly && lat < 0.0) continue;    // limit to North
      p.setGeodetic(lat,0.0,0);
         // nlon = circumference (EW) at lat / spacing
      radius = p.radiusEarth(lat,WGS84.a(),WGS84.eccSquared());
      dlon = 2 * PI * cos(p.getGeodeticLatitude()*DEG_TO_RAD) * radius;
      nlon = int(dlon/GridSpace);
      if (nlon == 0) nlon = 1;
      if (nlon > MaxNLon) MaxNLon=nlon;
      dlon = 360.0/nlon;
      for (j=0; j<nlon; j++)
      {
         lon = j * dlon - 180; // range from -180 to 180 for contour plot
//         cout << "lon = " << lon << endl;
         if (NAonly && lon > 0.0) continue; // limit to West
         GridData gd;  // temp. grid
         gd.lat = lat;
         gd.lon = lon;
         gd.gdop = gd.pdop = gd.hdop = gd.vdop = gd.tdop = 0.0;
         gd.bdop = gd.nsvs = 0.0;

         Grid.push_back(gd);

      } // end loop over lon
   } // end loop over lat
   lofs << "Grid size is " << Grid.size() << endl;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------

void ComputeDOPs(DayTime& tt, GridData& gd, vector<Position>& SVs, M4& R)
{
try
{
   int j,k,n,Nsvs;
   double elev,rawrange;

   Position Rx(gd.lat,gd.lon,0.0,Position::Geodetic); // grid position
   gd.bdop = 0.;

   Nsvs = SVs.size();

   // construct direction cosine matrix and solution covariance
   // BlueBook vol 1 p 414  or  GPS 2ed (Misra & Enge) p 203

   Matrix<double> DC(Nsvs,4), COV(4,4);
   n = 0;                                     // number of visible SVs
   for (j=0; j<Nsvs; j++)
   {
      elev = Rx.elevationGeodetic(SVs[j]);
      if (elev <= 5.0) continue;              // TD Elevation limit input

      rawrange = range(Rx,SVs[j]);            // geometric range

      DC(n,0) = (Rx.X()-SVs[j].X())/rawrange; // direction cosines
      DC(n,1) = (Rx.Y()-SVs[j].Y())/rawrange; // (G matrix of Misra & Enge)
      DC(n,2) = (Rx.Z()-SVs[j].Z())/rawrange;
      DC(n,3) = 1;

      n++;                                    // increase counter for # visible SVs
   }

   // if there aren't 4 satellites, we can't go on
   if (n < 4)
   {
      lofs << "Inadequate visibility: grid " << Rx.printf("%5.1AN %5.1LE")
           << " time " << tt << endl;
      return;
   }
   DC = Matrix<double>(DC,0,0,n,4);       // trim the unnecessary zeros

   DC = DC * transpose(R);                // transform to UENT (G~ matrix)

   COV = transpose(DC) * DC;              // (G~^T * G~)
   COV = inverseSVD(COV);                 // (G~^T * G~)^-1

   gd.vdop = ::sqrt(COV(0,0));            // pick off the various DOPs
   gd.hdop = ::sqrt(COV(1,1) + COV(2,2));
   gd.tdop = ::sqrt(COV(3,3));
   gd.pdop = ::sqrt(COV(0,0) + COV(1,1) + COV(2,2));
   gd.gdop = ::sqrt(COV(0,0) + COV(1,1) + COV(2,2) + COV(3,3));
   gd.nsvs = n;

   if (gd.pdop > 6) { gd.bdop = gd.bdop + 1. ; } // def'n of BDOP
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
