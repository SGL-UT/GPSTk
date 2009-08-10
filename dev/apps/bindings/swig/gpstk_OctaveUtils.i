// GPSTk wrap for Octave through SWIG 1.3.25

// Wrap for gpstk_OctaveUtils.cpp



std::ios::openmode ios_out();
std::ios::openmode ios_in();
std::ios::openmode ios_out_trunc();
bool read(FFStream& s, const FFData& d);
bool write(FFStream& s, FFData& d);
RinexObsData::RinexPrnMap::iterator RinexPrnMap_find(RinexObsData& roe, RinexPrn prn);
RinexObsData::RinexPrnMap::iterator RinexPrnMap_end(RinexObsData& roe);
RinexObsData::RinexDatum getPseudoRangeCode(RinexObsData roe, RinexPrn prn, RinexObsType type);
RinexObsData::RinexDatum getPseudoRangeCode(RinexObsData::RinexPrnMap::iterator ptr, RinexObsHeader::RinexObsType type);
bool RinexPrnMap_compare(RinexObsData::RinexPrnMap::iterator x, RinexObsData::RinexPrnMap::iterator y);