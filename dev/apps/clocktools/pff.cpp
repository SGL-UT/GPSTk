#include <iostream>

#include <stdio.h>

using namespace std;

int main()
{
	double dataA[2];
	double dataB[2];
	unsigned long int i = 0;

	double ff;

	while(!feof(stdin))
	{
		if(i != 0)
		{
			dataB[0] = dataA[0];
			dataB[1] = dataA[1];
		}

		cin >> dataA[0] >> dataA[1];
		if(feof(stdin)) break;

		if(i != 0)
		{
			ff = (dataA[1] - dataB[1])/(dataA[0]-dataB[0]);
			printf("%.1f %.15f\n", dataB[0], ff);
		}

		i++;
	}

	return(0);
}
