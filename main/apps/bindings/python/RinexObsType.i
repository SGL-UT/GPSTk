// GPSTk wrap for Python through SWIG 1.3.25
// Wrapping RinexObsHeader::RinexObsType separately

struct RinexObsType
{
	std::string type;
	std::string description;
	std::string units;
	unsigned int depend;
	RinexObsType();
	RinexObsType(std::string t, std::string d, std::string u,
		unsigned int dep=0);
	static const unsigned int C1depend;
	static const unsigned int L1depend;
	static const unsigned int L2depend;
	static const unsigned int P1depend;
	static const unsigned int P2depend;
	static const unsigned int EPdepend;
	static const unsigned int PSdepend;
};
