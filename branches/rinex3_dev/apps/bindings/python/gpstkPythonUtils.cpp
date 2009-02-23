// GPSTk wrap for Python through SWIG
// These are functions I wrote to make the binding work better

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
