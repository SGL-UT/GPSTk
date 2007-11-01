#include "xDayTimeInc.hpp"
      
CPPUNIT_TEST_SUITE_REGISTRATION (xINCDayTime);

void xINCDayTime :: setUp (void)
{
	totalIncrements=0;  
	gpstk::DayTime dtorig(2000,12,1,0,0,0.);
}

void xINCDayTime :: StartUp (void)
{

	cout << endl << "Testing DayTime increment safety." << endl << endl;
	cout << setw(18) << "Increment operator";
     	cout << setw(18) << "# of increments";
     	cout << setw(22) << "Difference (seconds)";
     	cout << setw(21) << "Difference (meters)";
     	cout << setw(23) << "Acceptable Diff (sec)";
	cout << endl;

      	cout << setw(18) << "------------------";
      	cout << setw(18) << "---------------";
      	cout << setw(22) << "--------------------";
      	cout << setw(21) << "-------------------";
     	cout << setw(23) << "--------------------";
      	cout << endl;
}
void xINCDayTime :: plusEqualsTest (void)
{
	StartUp();
	
	TEST_METHOD(operator+=, 60,      1, 1e-15);
  	TEST_METHOD(operator+=, 3600,    1, 1e-15);
      	TEST_METHOD(operator+=, 86400,   1, 1e-15);
      	TEST_METHOD(operator+=, 7*86400, 1, 1e-15);
      	cout << endl;
	
}

void xINCDayTime :: addMilliSecTest (void)
{
	
	TEST_METHOD(addMilliSeconds, 1000,         1, 1e-15);
      	TEST_METHOD(addMilliSeconds, 60*1000,      1, 1e-15);
      	TEST_METHOD(addMilliSeconds, 3600*1000,    1, 1e-15);
      	TEST_METHOD(addMilliSeconds, 86400*1000,   1, 1e-15);
      	TEST_METHOD(addMilliSeconds, 86400*2*1000, 1, 1e-15);
      	TEST_METHOD(addMilliSeconds, 86400*7*1000, 1, 1e-15);
      	cout << endl;
	
}
void xINCDayTime :: addMicroSecTest (void)
{
	
	TEST_METHOD(addMicroSeconds, 1000*1000,      1, 1e-3);
      	TEST_METHOD(addMicroSeconds, 60*1000*1000,   1, 1e-3);
      	TEST_METHOD(addMicroSeconds, 5*60*1000*1000, 1, 1e-3);
      	cout << endl;
	
}

void xINCDayTime :: addSecondsTest (void)
{
	TEST_METHOD(addSeconds, 60,                  1., 1e-12);
	TEST_METHOD(addSeconds, 3600,                1., 1e-12);
	TEST_METHOD(addSeconds, 86400,               1., 1e-12);
	TEST_METHOD(addSeconds, 7*86400,             1., 1e-12);
	cout << endl;
	TEST_METHOD(addSeconds, 1000,             1e-3, 1e-12);
	TEST_METHOD(addSeconds, 60*1000,          1e-3, 1e-12);
	TEST_METHOD(addSeconds, 3600*1000,        1e-3, 1e-12);
	TEST_METHOD(addSeconds, 86400*1000,       1e-3, 1e-12);
	TEST_METHOD(addSeconds, 86400*2*1000,     1e-3, 1e-12);
	TEST_METHOD(addSeconds, 86400*7*1000,     1e-3, 1e-12);
	cout << endl;
	TEST_METHOD(addSeconds, 1000*1000,        1e-6, 1e-12);
	TEST_METHOD(addSeconds, 60*1000*1000,     1e-6, 1e-12);
	TEST_METHOD(addSeconds, 5*60*1000*1000,   1e-6, 1e-12);
	cout << endl;
	TEST_METHOD(addSeconds, (1000*1000),        1e-9, 1e-12);
	TEST_METHOD(addSeconds, (100*1000*1000),    1e-9, 1e-12);
	cout << endl;
	TEST_METHOD(addSeconds, (1000*1000),        1e-11, 1e-12);
	TEST_METHOD(addSeconds, (100*1000*1000),    1e-11, 1e-12);
	cout << endl;
	FinishUp();
}

void xINCDayTime :: FinishUp (void)
{
	gpstk::DayTime dtorig(2000,12,1,0,0,0.);
	DayTime endTime;
        cout << endl << setprecision(4);
   	cout << endTime.printf("Completed on %B %d, %Y %H:%02M:%02S") << endl;
      	cout << "Processing time " << endTime-startTime << " seconds." << endl;
      	cout << endl;
}
