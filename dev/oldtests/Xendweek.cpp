/* $Id$
*  Xendweek.c - Test scaffold for demonstrating correctness of the 
*  SVPCodeGen class.  In this case, the test is to correctly generate
*  the P-code for the last 6 seconds of the week for each possible
*  GPS PRN.
*
*  August 2003
*  Applied Reserach Laboratories, The University of Texas at Austin
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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

   // Language headers
#include <stdlib.h>
#include <stdio.h>
#include <map>

   // Project headers
#include "PCodeConst.hpp"
#include "SVPCodeGen.hpp"
#include "CodeBuffer.hpp"
#include "X1Sequence.hpp"
#include "X2Sequence.hpp"

/**
 * @file Xendweek.cpp
 *
 */

using namespace std;    // for map and pair access
using namespace gpstk;

   //  Convenience structure for grouping all the data objects associated
   //  with a single SV.  These structs are commonly placed in maps for
   //  processing.
struct SVData
{
   SVPCodeGen *           PCodeGen;
   CodeBuffer *           PCodeBuf;
   codeType               cType;
   SVData( SVPCodeGen *         a1,
           CodeBuffer *         a2,
           codeType             c )
   { 
      PCodeGen = a1;
      PCodeBuf = a2;
      cType    = c; 
   };
};

const int NUM_SATS = 38;

int main(int argc, char* argv[] )
{
  printf("Ryan's Xendweek\n");

  if (argc < 2)
    {
      printf("Usage: >>Xendweek <outputfile>\n");
      exit(1);
    }  
      // Open an output file.
   printf(" Opening output file.\n");
   FILE *outFile = fopen( argv[1], "wt" );
   if (outFile==NULL)
   {
      printf(" Cannot open Xendweek.out for output.\n");
      exit(-1);
   }
   fprintf(outFile," XENDWEEK.OUT - Demonstrating P-Code End of Week Generation..\n");
   fprintf(outFile,"                Reproducing ICD-GPS-200, Table 3-IV\n");

      // Set time to end of week minus six seconds
   printf(" Setting time to end of week.\n");
   //DayTime dt( 1233, 604794.0, 2003 );
   DayTime dt( 1233, 604794.0 );
   
      // NOTE: The P-code generator works in 6-second "chunks".  This implies
      // that there are 6 seconds of X1 bitstream and 6 seconds of X2 bitstreams
      // held in memory.  To mimimize the memory footprint, these bitstreams
      // are shared between all coders and are located in dynamically allocated
      // buffers referenced through static pointers.  THEREFORE, before any 
      // SVPCodeGen objects are instantiated, these buffers must be allocated
      // and initialized via the following two calls.  Failure to do so will
      // resort in abnormal program termination (unless the exceptions are
      // trapped). 
   try
   {
      X1Sequence::allocateMemory();
      X2Sequence::allocateMemory();
   }
   catch (gpstk::Exception e)
   {
      printf(" Memory allocation failure.\n");
      printf(" Xbegweek will terminate.\n");
      return(1);
   }
   
      // Instantiate a map to hold SVPCodeGen and CodeBuffer objects
   printf(" Instantiating the PRN map.\n");
   map<int, SVData>svdMap;
   typedef map<int, SVData>::iterator SVDataI;
   
      // Instantiate and initialize the P-coder objects
   SVPCodeGen * svp[NUM_SATS];
   CodeBuffer * pcb[NUM_SATS]; 
   for (int n=0;n<NUM_SATS;++n) { svp[n]=0; pcb[n]=0; };
   
   for (int PRNndx=0; PRNndx<NUM_SATS; ++PRNndx)
   {
      svp[PRNndx] = new SVPCodeGen( PRNndx, dt );
      pcb[PRNndx] = new CodeBuffer( PRNndx );

      SVData svd( svp[PRNndx], pcb[PRNndx], P_CODE );
      pair<int,SVData> ps( PRNndx, svd );
      svdMap.insert( ps );
   }
   
   fprintf(outFile," PRN  Last word ");

      // Calculate X1count where X2 starts to transition to end of week "hold"
   long EndOfWeekTestCount =
      3 * (XA_COUNT * XA_MAX_EPOCH) + (XA_COUNT * (XA_MAX_EPOCH-1)) + 3023;
   long EndOfWeekTestWord = (EndOfWeekTestCount / MAX_BIT);
   long EndOfWeekTestEnd = EndOfWeekTestWord+(XA_EPOCH_DELAY+104)/MAX_BIT+3; 
   
   long chip = EndOfWeekTestWord * MAX_BIT -
               3 * (XA_COUNT * XA_MAX_EPOCH) - (XA_COUNT * (XA_MAX_EPOCH-1));
   for (long j=EndOfWeekTestWord;j<EndOfWeekTestEnd;j++)
   {
      fprintf(outFile,"%4d      ",chip);
      chip+=32;
   }
   fprintf(outFile,"\n");
   
   int PRNID;
   unsigned long temp;
   for ( SVDataI p=svdMap.begin(); p!=svdMap.end(); ++p )
   {
      PRNID = p->first;
      SVData& rsvd = p->second;
      SVPCodeGen& rsvp = *(rsvd.PCodeGen);
      CodeBuffer& rcb = *(rsvd.PCodeBuf);
      rsvp.getCurrentSixSeconds( rcb );
      temp = rcb[NUM_6SEC_WORDS-1];
      fprintf( outFile,"  %02d  x%08X",PRNID,temp);
      for (long n=EndOfWeekTestWord;n<EndOfWeekTestEnd;n++)
      {
         fprintf(outFile," x%08X",rcb[n]);
      }
      fprintf(outFile,"\n");
   }

      /*
         Close the output files and exit gracefully.
      */
   printf(" Closing files.\n");
   fflush( outFile );
   fclose( outFile );
   return(0);
}
