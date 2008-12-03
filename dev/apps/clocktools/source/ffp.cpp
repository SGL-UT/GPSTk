#include <iostream>

#include <stdio.h>

using namespace std;

int main()
{
	double dataA[2];
	double dataB[2];
	unsigned long int i;

	double x0=0.0;

	while(!feof(stdin))
	{
		dataB[0] = dataA[0];
		dataB[1] = dataA[1];

		cin >> dataA[0] >> dataA[1];
		if(feof(stdin)) break;

		if(i != 0)
		{
			x0 = dataB[1]*(dataA[0]-dataB[0]) + x0;
			printf("%.0f %.20f\n", dataA[0], x0);
		}
		
		i++;
	}

	return(0);
}

