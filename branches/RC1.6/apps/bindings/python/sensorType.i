// GPSTk wrap for Python through SWIG 1.3.25
// Two nested structs from RinexMetHeader.hpp

struct sensorType
{
	std::string model;
	std::string type;
	double accuracy;
	RinexMetType obsType;
};

struct sensorPosType
{
	gpstk::Triple position;
	double height;
	RinexMetType obsType;
};
