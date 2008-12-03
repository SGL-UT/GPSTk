#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char **argv)
{
	double data[2];
	double trunctime;

	if(argc != 2)
	{
		printf("Usage: trunc <time>\n");
	}
	else
	{
		trunctime = atof(argv[1]);

		while(!feof(stdin))
		{
			cin >> data[0] >> data[1];
			if(feof(stdin)) break;
			if(data[0] > trunctime) break;
			else printf("%.1f %.15f\n", data[0], data[1]);
		}
	}

	return(0);
}
