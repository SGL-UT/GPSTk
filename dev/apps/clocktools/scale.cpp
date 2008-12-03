#include <iostream>
#include <stdio.h>

using namespace std;

int main()
{
	double data[2];

	while(!feof(stdin))
	{
		cin >> data[0] >> data[1];
		if(feof(stdin)) break;
		printf("%.1f %.15f\n", data[0], data[1]*0.000001);
	}

	return(0);
}
