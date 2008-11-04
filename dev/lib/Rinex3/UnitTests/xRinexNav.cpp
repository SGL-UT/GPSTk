#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavFilterOperators.hpp"
#include "StringUtils.hpp"
#include <iostream>


using namespace std;

bool fileEqualTest (char*, char*);

int main()
{
	gpstk::RinexNavStream RinexNavStream("RinexNavExample.99n");
	gpstk::RinexNavStream out("TestOutput.99n",ios::out);
	gpstk::RinexNavStream dmp("RinexDump",ios::out);
	gpstk::RinexNavHeader RinexNavHeader;
	gpstk::RinexNavData RinexNavData;
	RinexNavStream >> RinexNavHeader;
	out << RinexNavHeader;
	
	while (RinexNavStream >> RinexNavData)
	{
		out << RinexNavData;
	}

        cout << fileEqualTest("RinexNavExample.99n","TestOutput.99n") << endl;
return 0;
}

bool fileEqualTest (char* handle1, char* handle2)
{
	bool isEqual = false;
	int counter = 0;
	ifstream File1;
	ifstream File2;
	
	std::string File1Line;
	std::string File2Line;
	
	File1.open(handle1);
	File2.open(handle2);
	getline (File1, File1Line);
	getline (File2, File2Line);
	getline (File1, File1Line);
	getline (File2, File2Line);
	
	while (!File1.eof())
	{
		if (File2.eof()) 
		{
			cout << "ONE" << counter << endl;
			
			return isEqual;
			}
		getline (File1, File1Line);
		getline (File2, File2Line);

		if (File1Line != File2Line)
		{
			cout << "TWO"  << counter << endl;
			cout << File1Line << endl;
			cout << File2Line << endl;
			return isEqual;
		}
	}
	if (!File2.eof()){
	cout << "THREE" << counter  << endl;
		return isEqual;
		}
	else
		return isEqual = true;
}
