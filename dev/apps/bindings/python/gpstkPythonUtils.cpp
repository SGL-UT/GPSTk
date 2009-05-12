#pragma ident "$Id$"
// GPSTk wrap for Python through SWIG
// These are functions I wrote to make the binding work better

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

#include <iostream>
#include <ios>
#include <fstream>
#include "FFData.hpp"
#include "FFStream.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"

std::ios::openmode ios_out()
{
  return std::ios::out;
}

std::ios::openmode ios_in()
{
  std::ios::openmode result;
  result = std::ios::in;
  return result;
}

std::ios::openmode ios_out_trunc()
{
  std::ios::openmode result;
  result = std::ios::out|std::ios::trunc;
  return result;
}

bool read(FFStream& s, const FFData& d)
{
  return s << d;
}

bool write(FFStream& s, FFData& d)
{
  return s >> d;
}

RinexObsData::RinexPrnMap::iterator RinexPrnMap_find(RinexObsData& roe, RinexPrn prn)
{
  RinexObsData::RinexPrnMap::iterator result = roe.obs.find(prn);
  return result;
}

RinexObsData::RinexPrnMap::iterator RinexPrnMap_end(RinexObsData& roe)
{
  return roe.obs.end();
}

RinexObsData::RinexDatum getPseudoRangeCode(RinexObsData roe, RinexPrn prn, RinexObsHeader::RinexObsType type)
{
  return roe.obs[prn][type];
}

RinexObsData::RinexDatum getPseudoRangeCode(RinexObsData::RinexPrnMap::iterator ptr, RinexObsHeader::RinexObsType type)
{
  RinexObsData::RinexDatum x;
  x = (*ptr).second[type];
  return x;
}

bool RinexPrnMap_compare(RinexObsData::RinexPrnMap::iterator x, RinexObsData::RinexPrnMap::iterator y)
{
  return x == y;
}
