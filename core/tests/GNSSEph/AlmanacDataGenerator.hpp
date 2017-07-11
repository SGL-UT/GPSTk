//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#ifdef _MSC_VER
static inline double round(double val)
{    
    return floor(val + 0.5);
}
#endif

//Reads in the almanac file and stores it in data types
class AlmanacData
{
public:
   int id[31];
   int health[31];
   float ecc[31];
   float toa[31];
   float oi[31];
   float rora[31];
   float sqrta[31];
   float raaw[31];
   float aop[31];
   float ma[31];
   float af0[31];
   float af1[31];
   float week[31];
   std::string line;
        
   AlmanacData(void)
   {

   }

   AlmanacData(std::ifstream &file)
   {
      for (int i=0; i<31; i++)
      {
         getline(file, line); //skip
         id[i] = readData(file); // sat id
         health[i] = readData(file);
         ecc[i] = readData(file); // eccentricity (e)
         toa[i] = readData(file); // Time of Applicability (toa)
         oi[i] = readData(file); // Orbital Inclination (detai)
         rora[i] = readData(file); // Rate of Right Ascension (OMEGADOT)
         sqrta[i] = readData(file);
         raaw[i] = readData(file); // Rate of Ascension at Week (OMEGA0)
         aop[i] = readData(file); // Argument of Perigee (omega)
         ma[i] = readData(file); // Mean Anomaly (M0)
         af0[i] = readData(file);
         af1[i] = readData(file);
         week[i] = readData(file);
         getline(file, line); //skip
      }

   }

   float readData(std::ifstream &file)
   {
      getline(file, line);
      line=cleanLine(line);
      return atof(line.c_str());
   }

   std::string cleanLine(std::string line)
   {
      line = line.substr(25, std::string::npos);
      line.erase( std::remove(line.begin(), line.end(), ' '), line.end());
      return line;
   }
};

//Converts almanac data to subframes
//No parity! EngAlmanac doesn't test for parity
class AlmanacSubframes
{
public:
   struct SVsubframes
   {
      uint32_t word1;
      uint32_t word2;
      uint32_t word3;
      uint32_t word4;
      uint32_t word5;
      uint32_t word6;
      uint32_t word7;
      uint32_t word8;
      uint32_t word9;
      uint32_t word10;
   } allSVs[31];

   long totalSf[31][10];

   AlmanacSubframes(void)
   {

   }

   AlmanacSubframes(AlmanacData aData) //Shouldn't be a reference, need the original data for comparison
   {
      scaleData(aData);

      for (int i=0; i<31; i++)
      {
         allSVs[i].word1 = 0x22c000e4;
         allSVs[i].word2 = 0x00000598;
         if(aData.id[i] > 25) allSVs[i].word2 = 0x0000042c;
         allSVs[i].word3 = ( (( (1 << 6) + ((uint32_t) aData.id[i])) << 16) + ((uint32_t) aData.ecc[i]) ) << 6;
         allSVs[i].word4 = ( (((uint32_t) aData.toa[i]) << 16) + ((uint32_t) aData.oi[i]) ) << 6;
         allSVs[i].word5 = ( (((uint32_t) aData.rora[i]) << 8) + ((uint32_t) aData.health[i]) ) << 6;
         allSVs[i].word6 = ((uint32_t) aData.sqrta[i]) << 6;
         allSVs[i].word7 = ((uint32_t) aData.raaw[i]) << 6;
         allSVs[i].word8 = ((uint32_t) aData.aop[i]) << 6;
         allSVs[i].word9 = ((uint32_t) aData.ma[i]) << 6;
         startaf0 = ( (uint32_t) aData.af0[i] ) >> 3;
         endaf0 = (((uint32_t) aData.af0[i]) << (32-3)) >> (32-3);
         allSVs[i].word10 = ( (((startaf0 << 11) + (uint32_t) aData.af1[i]) << 3) + endaf0 ) << 8;
                
         totalSf[i][0] = allSVs[i].word1;
         totalSf[i][1] = allSVs[i].word2;
         totalSf[i][2] = allSVs[i].word3;
         totalSf[i][3] = allSVs[i].word4;
         totalSf[i][4] = allSVs[i].word5;
         totalSf[i][5] = allSVs[i].word6;
         totalSf[i][6] = allSVs[i].word7;
         totalSf[i][7] = allSVs[i].word8;
         totalSf[i][8] = allSVs[i].word9;
         totalSf[i][9] = allSVs[i].word10;
      }

   }

   AlmanacData scaleData(AlmanacData &aData)
   {
      for(int i=0; i<31; i++)
      {
            //pow is used to scale, round returns an signed int, uint32_t recasts to unsigned
            //resigned in 32, so 
         aData.ecc[i] = twosCompliment((uint32_t) round(aData.ecc[i] * pow(2.,21)), 16);
         aData.toa[i] = twosCompliment((uint32_t) round(aData.toa[i] * pow(2.,-12)), 8);
         aData.oi[i] = twosCompliment((uint32_t) round((aData.oi[i] - .3*M_PI) * pow(2., 19) / M_PI), 16);
         aData.rora[i] = twosCompliment((uint32_t) round(aData.rora[i] * pow(2.,38) / M_PI), 16);
         aData.sqrta[i] = twosCompliment((uint32_t) round(aData.sqrta[i] * pow(2.,11)), 24);
         aData.raaw[i] = twosCompliment((uint32_t) round(aData.raaw[i] * pow(2.,23) / M_PI), 24);
         aData.aop[i] = twosCompliment((uint32_t) round(aData.aop[i] * pow(2.,23) / M_PI), 24);
         aData.ma[i] = twosCompliment((uint32_t) round(aData.ma[i] * pow(2.,23) / M_PI), 24);
         aData.af0[i] = twosCompliment(((uint32_t) round(aData.af0[i] * pow(2.,20))), 11);
         aData.af1[i] = twosCompliment(((uint32_t) round(aData.af1[i] * pow(2.,38))), 11);
      }

      return aData;
   }

   uint32_t twosCompliment(uint32_t data, int size)
   {
      if (data > 0x1000)
         data = (data << (32 - size)) >> (32-size); 
      else data = (uint32_t) round(data);

      return data;
   }

   uint32_t startaf0;
   uint32_t endaf0;
};

uint32_t threesCompliment(uint32_t data, int size)
{
   if (data > 0x1000)
      data = (data << (32 - size)) >> (32-size); 
   else data = (uint32_t) round(data);

   return data;
}
/*
int main(void)
{
std::ifstream file("./current.txt");

AlmanacData aData(file);
AlmanacSubframes dataIHope(aData);

for (int i = 0; i<31; i++)
{
std::cout<<std::hex<<dataIHope.allSVs[i].word1<<", ";
std::cout<<dataIHope.allSVs[i].word2<<", ";
std::cout<<dataIHope.allSVs[i].word3<<", ";
std::cout<<dataIHope.allSVs[i].word4<<", ";
std::cout<<dataIHope.allSVs[i].word5<<", ";
std::cout<<dataIHope.allSVs[i].word6<<", ";
std::cout<<dataIHope.allSVs[i].word7<<", ";
std::cout<<dataIHope.allSVs[i].word8<<", ";
std::cout<<dataIHope.allSVs[i].word9<<", ";
std::cout<<dataIHope.allSVs[i].word10<<std::endl;
};
   //std::cout<<cleanLine(clean)<<std::endl;
   return 0;
   }*/
