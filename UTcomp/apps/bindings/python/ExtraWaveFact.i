// GPSTk wrap for Python through SWIG 1.3.25
// RinexObsHeader::ExtraWaveFact
// Requires the following in gpstk.i:



struct ExtraWaveFact
{
	std::vector < RinexPrn > prnList;
	short wavelengthFactor[2];
};
