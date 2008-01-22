/* $Id$
*  Xbegweek.cpp - Test scaffold for demonstrating correctness of the 
*  SVPCodeGen class.
*
*  February 2004
*  Applied Reserach Laboratories, The University of Texas at Austin
*/
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
 * @file Xbegweek.cpp
 *
 */
using namespace std;    // for map and pair access
using namespace gpstk;

   //  Convenience structure for grouping all the data objects associated
   //  with a single SV.  These structs are commonly placed in maps for
   //  processing.
struct SVData
{
   SVPCodeGen * PCodeGen;
   CodeBuffer * PCodeBuf;
   codeType   cType;
   SVData( SVPCodeGen * a1,
           CodeBuffer * a2,
           codeType c )
   { 
      PCodeGen = a1;
      PCodeBuf = a2;
      cType    = c; 
   };
};

const int NUM_SATS = 38;

int main(int argc, char* argv[])
{
   printf("Ryan's Xbegweek\n");
  
   if (argc < 2)
   {
     printf("Usage: >Xbegweek <outputfile>\n");
     exit(1);
   }
      // Open an output file.
   printf(" Opening output file.\n");
   FILE *outFile = fopen( argv[1], "wt" );
   if (outFile==NULL)
   {
      printf(" Cannot open xbegweek.out for output.\n");
      exit(-1);
   }
   fprintf(outFile," Xbegweek.out - Demonstrating P-Code Beginnging of Week Generation..\n");
   fprintf(outFile,"                Reproducing ICD-GPS-200, Table 3-I\n");

      // Set time to beginning of week
   printf(" Setting time to beginning of week.\n");
   //DayTime dt( 1233, 0.0, 2003 );   // Beginning of week 1233 
   DayTime dt( 1233, 0.0 );   // Beginning of week 1233 
   
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
   printf(" Instantiating the PRN arrays.\n");
   map<int, SVData>svdMap;
   typedef map<int, SVData>::iterator SVDataI;
   
   SVPCodeGen * svp[NUM_SATS];
   CodeBuffer * pcb[NUM_SATS];
   for (int n=0;n<NUM_SATS;++n)
   {
      svp[n] = 0;
      pcb[n] = 0;
   }
   
   for (int PRNndx=0;PRNndx<NUM_SATS;PRNndx++)
   {
      svp[PRNndx] = new SVPCodeGen( PRNndx, dt );
      pcb[PRNndx] = new CodeBuffer( PRNndx );
      SVData svd ( svp[PRNndx], pcb[PRNndx], P_CODE );
      pair<int,SVData> ps( PRNndx, svd );
      svdMap.insert( ps );
   }
   
   fprintf(outFile," PRN  12-bits of code (octal)\n");

   int PRNID;   
   unsigned long temp;
   for ( SVDataI p=svdMap.begin(); p!=svdMap.end(); ++p)
   {
      PRNID = p->first;
      SVData& rsvd = p->second;
      SVPCodeGen& rsvp = *(rsvd.PCodeGen);
      CodeBuffer& rcb = *(rsvd.PCodeBuf);
      rsvp.getCurrentSixSeconds( rcb );
      temp = rcb[0] >> 20;
      fprintf( outFile,"  %02d             %04o\n",PRNID,temp);
   }
   
      // Close the output files and exit gracefully.
   printf(" Closing files.\n");
   fflush( outFile );
   fclose( outFile );
   return(0);
}
