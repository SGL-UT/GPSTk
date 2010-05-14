#pragma ident "$Id: $"

/**
* @file EarthOceanTide.cpp
* 
*/


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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

#include "EarthOceanTide.hpp"
#include <fstream>
#include <math.h>
#include <string>
#include "UTCTime.hpp"
#include "ASConstant.hpp"
#include "ReferenceFrames.hpp"


namespace gpstk
{
   using namespace gpstk::StringUtils;

   // constants
   const double EarthOceanTide::G  = 6.67259e-11; // m^3/kg/s/s
   const double EarthOceanTide::GE = 9.780327;    // m/s/s 

      /*
      * load ocean data file see bern "OT_CSRC.TID"
      * reference bernese5 OTIDES.f
      */
   void EarthOceanTide::loadTideFile(std::string fileName,int NMAX,double XMIN)
   {
      if(isLoaded) return;

      // open the file
      std::ifstream fin(fileName.c_str());
      if(!fin.good())
      {
         Exception e("Can not Open the CSR Ocean Tide File:"+fileName);
         GPSTK_THROW(e);
      }

      // read the file
      std::string buf;

      // line 1 ,skip it
      getline(fin,buf);   

      // line 2
      getline(fin,buf);
      NWAV = asInt(buf.substr(0,4));
      NTOT = asInt(buf.substr(4,4));
      NMX = asInt(buf.substr(8,4));
      MMX = asInt(buf.substr(12,4));

      // line 3 ,skip it
      getline(fin,buf);

      // line 4
      getline(fin,buf);
      RRE = asDouble(buf.substr(0,21));
      RHOW = asDouble(buf.substr(21,21));
      XME = asDouble(buf.substr(42,21));
      PFCN = asDouble(buf.substr(63,21));
      XXX = asDouble(buf.substr(84,21));

      // line 5 ~ 8
      for(int i=0;i<4;i++)
      {
         getline(fin,buf);
         for(int j=0;j<6;j++   )      
         {
            if((i*6+j) > 19) break;
            tideData.KNMP[i*6+j] = asDouble(buf.substr(j*21,21));   
         }
      }

      //IGNORE  THE NWAV LINE
      for(int i=0;i<NWAV;i++) getline(fin,buf);

      // CEXTRACT REQUIRED INFORMATION FROM NEXT NTOT LINES
      int id = 0;
      for(int i=0;i<NTOT;i++)
      {
         getline(fin,buf);

         tideData.NDOD[id][0] = asInt(buf.substr(13,1)); 
         tideData.NDOD[id][1] = asInt(buf.substr(14,1)); 
         tideData.NDOD[id][2] = asInt(buf.substr(15,1)); 

         tideData.NDOD[id][3] = asInt(buf.substr(17,1)); 
         tideData.NDOD[id][4] = asInt(buf.substr(18,1)); 
         tideData.NDOD[id][5] = asInt(buf.substr(19,1));          

         tideData.NM[id][0] = asInt(buf.substr(24,2)); 
         tideData.NM[id][1] = asInt(buf.substr(26,2));       

         tideData.CSPM[id][0] = asDouble(buf.substr(30,22)); 
         tideData.CSPM[id][1] = asDouble(buf.substr(52,22));
         tideData.CSPM[id][2] = asDouble(buf.substr(74,22));
         tideData.CSPM[id][3] = asDouble(buf.substr(96,22));


         if((tideData.NM[id][0]<=NMAX) &&
            (
            (fabs(tideData.CSPM[id][0])>XMIN) ||
            (fabs(tideData.CSPM[id][1])>XMIN) ||
            (fabs(tideData.CSPM[id][2])>XMIN) ||
            (fabs(tideData.CSPM[id][3])>XMIN))
            )
         {
            for(int j=1;j<6;j++) tideData.NDOD[id][j]-=5;

            id ++;
            tideData.NTACT = id;
         }
         // check
         /*
         if(m_csr_otide.NTACT>=MAXTRM)
         {
         cerr << "ERROR: SR OTIDES: NOT ALL TERMS AVAILABLE IN FILE" << endl;
         exit(1);
         }*/
      }   

      // close the file
      fin.close();

      isLoaded = true;

      FAC[0] = 1.0;
      for(int i=1;i<=30;i++)
      {
         FAC[i]=FAC[i-1]*i;
      }

   }

      /* Ocean pole tide to normalized earth potential coefficients
       *
       * @param mjdUtc UTC in MJD
       * @param dC     Correction to normalized coefficients dC
       * @param dS     Correction to normalized coefficients dS
       *    C20 C21 C22 C30 C31 C32 C33 C40 C41 C42 C43 C44
       */
   void EarthOceanTide::getOceanTide(double mjdUtc, double dC[], double dS[] )
   {
      try
      {
         loadTideFile(fileName,maxN,minX);
      }
      catch (...)
      {
         // faild to get the ocean tide model
         // return zeros
         const int n = (maxN-1)*(maxN+4)/2; 
         for(int i=0;i<n;i++)
         {
            dC[i] = 0.0;
            dS[i] = 0.0;
         }

         return;
      }
      
      UTCTime utc;
      utc.setMJD(mjdUtc);

      //   CC PURPOSE    :  COMPUTE DOODSON'S FUNDAMENTAL ARGUMENTS (BETA)
      //  CC               AND FUNDAMENTAL ARGUMENTS FOR NUTATION (FNUT)
      double BETA[6]={0.0};
      double FNUT[5] ={0.0};
      ReferenceFrames::doodsonArguments(utc.asUT1(), utc.asTT(), BETA,FNUT);

      for(int i=0;i<tideData.NTACT;i++)
      {
         int N= tideData.NM[i][0];
         int M= tideData.NM[i][1];

         if(tideData.NM[i][0]>maxN) continue;

         double delta = M ? 0 : 1;

         double FNM = 4.0*ASConstant::PI*G*RHOW/GE* std::sqrt(FAC[N+M]/FAC[N-M]/(2.0*N+1.0)/(2.0-delta))
            *(1.0+tideData.KNMP[N-1])/(2.0*N+1)/100.0;

         double ARG =0;
         for(int j=0;j<6;j++)
         {
            ARG +=tideData.NDOD[i][j]*BETA[j];
         }

         double CARG = std::cos(ARG);
         double SARG = std::sin(ARG);

         int index=N*(N+1)/2-3+M;

         dC[index]=dC[index]
         +FNM*(
            (tideData.CSPM[i][0]+tideData.CSPM[i][2])*CARG
            +(tideData.CSPM[i][1]+tideData.CSPM[i][3])*SARG);

         dS[index]=dS[index]
         +FNM*(
            (tideData.CSPM[i][1]-tideData.CSPM[i][3])*CARG
            -(tideData.CSPM[i][0]-tideData.CSPM[i][2])*SARG);
      }
      
   }	// End of method 'EarthOceanTide::getOceanTide()'
   

   void EarthOceanTide::test()
   {
      std::cout<<"test Earth Ocean Tide"<<std::endl;
      // debuging
      double mjdUtc = 2454531 + 0.49983796296296296 - 2400000.5;
      double dc[12]={0.0},ds[12]={0.0};
      getOceanTide(mjdUtc,dc,ds);

      int a = 0;

   }

}  // End of namespace 'gpstk'
