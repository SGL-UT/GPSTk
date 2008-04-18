/****************************************************************************/
/* This program reads in clock phase data provided any of the phase parsers */
/*  and removes outlier data, fills gaps, and then outputs the modified     */
/*  data in a format to be used by oallandev or ohadamarddev                */
/*                                                                          */
/* Written by Tim Craddock                                                  */
/* Last Updated March 20, 2008                                              */
/****************************************************************************/

#include <iostream>
#include <vector>

#include <stdio.h>
#include <math.h>

using namespace std;

int main()
{
	vector <double> timedata;  // Stores incoming time data
	vector <double> phasedata; // Stores incoming clock phase data
	
	vector <double> ffdata; // Stores the fractional frequency data, calculated from clock phase data
	double mavg, stddev;    // Stores the average slope and standard deviation of phase/fractional frequency data
	
	double time, phase, phasetotal;     // used as temprorary input variables
	unsigned long int plength, fflength, i, outliers,totalOutliers; // used to itteratem store lengths, and count the number of outliers found
	
	// Inputs time and clock phase data from the standard input
	i = 0;
	while(!feof(stdin))
	{
		cin >> time >> phase;
		timedata.resize(i+1);
		phasedata.resize(i+1);
		timedata[i] = time;
		phasedata[i] = phase;
		i++;
	}
	plength = i-1;
	
	// Converts clock phase data to fractional frequencies
	// uses y[i] = (x[i+1] - x[i])/Tau
	// where Tau is the averaging time
	
	fflength = plength - 1;
	for(i = 0; i < fflength; i++)
	{
		ffdata.resize(i+1);
		ffdata[i] = (phasedata[i+1] - phasedata[i]) / (timedata[i+1] - timedata[i]);
	}
        totalOutliers=0;
        do{
	   outliers=0;
           // Computes the averages fractional frequency
	   mavg = 0.0;
	   for(i = 0; i < fflength; i++) mavg += ffdata[i];
      	   mavg /= (double)fflength-totalOutliers;
	
	   // Computes the standard deviation of the fractional frequency
	   stddev = 0.0;
	   //fprintf(stdout, "fflength %i\n", fflength);
           for(i = 0; i < fflength; i++)
	   { 
               stddev += (ffdata[i] - mavg)*(ffdata[i] - mavg);
	       //fprintf(stdout, "%f, ", ffdata[i]);
	       //fprintf(stdout, "total %e\n", stddev);
           }
           //fprintf(stdout, "total %e\n", stddev);
           stddev /= (double)fflength-1-totalOutliers;
	   stddev = sqrt(stddev);
	
	   // Sets all fractional frequency values that are farther than 3 Sigma from the average to zero
	  //  thereby removing outliers

	      for(i = 0; i < fflength; i++)
	      {
		if(sqrt(pow(ffdata[i] - mavg, 2)) >= 3.0*stddev && ffdata[i]!=0)
		{
			ffdata[i] = 0.0;
			outliers++;
                        //fprintf(stdout, "%d", i);
		}
	      }
	      totalOutliers+=outliers;

	}while(outliers);
        
	// Converts fractional frequency data back to clock phase data
	// uses x[i+1] = x[i] + y[i]*Tau
	// where Tau is the averaging time
	// Fills in any missing gaps by doing so
	phasetotal=0;
        phasedata[0]=0;
        for(i = 0; i < fflength; i++)
	{
	  if(ffdata[i]==0){
	    phasedata[i]=0;
	    phasetotal+=mavg*(timedata[i+1]-timedata[i]);
            phasedata[i+1] = phasetotal;
          }
	  else{
	    phasetotal+=ffdata[i]*(timedata[i+1]-timedata[i]);
            phasedata[i+1] = phasetotal;
	  }  
	}
	
	// Outputs modified data to the standard output where it can be used by Ghetto32
	for(i = 0; i < plength; i++)
	{
	  fprintf(stdout, "%.1f %.25f\n", (timedata[i]), phasedata[i]);
	}
	//fprintf(stdout, "Number of outlier %d\n", outliers);
	//fprintf(stdout, "Stddev %.25f\n", stddev);
	//fprintf(stdout, "Mavg %.25f\n", mavg);
	return(0);
}
