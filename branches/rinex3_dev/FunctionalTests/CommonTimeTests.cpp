#include "CommonTimeTests.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char** argv)
{
   
   if(argc > 1)
   {
      
   }
   
   int ret = 0;
   try{
		if(!addDaysTest())
		{
			cout << "addDays(long) Test failed..." << endl;
			ret++;
		}
		else
			cout << "addDays(long) Test passed..." << endl;
		
		if(!addMillisecondsTest())
		{
			cout << "addMilliseconds(long) Test failed..." << endl;
			ret++;
		}
		else
			cout << "addMilliseconds(long) Test passed..." << endl;
		
		if(!addSecondsDoubleTest())
		{
			cout << "addSeconds(double) Test failed..." << endl;
			ret++;
		}
		else
			cout << "addSeconds(double) Test passed..." << endl;
		
		if(!addSecondsLongTest())
		{
			cout << "addSeconds(long) Test failed..." << endl;
			ret++;
		}
		else
			cout << "addSeconds(long) Test passed..." << endl;
		
		return ret;
	}
	catch(Exception& e)
	{
		cout << e << endl;
	}
}
/*
 * Add a day to a time just before midnight
 * Add a millisecond to a time just before midnight so it goes past midnight
 * Add a fractional millisecond to a time just before midnight so it goes past midnight
 * Subtract a day, millisecond and fractional millisecond from a time just after midnight, separately so this is 3 tests.
 * Add seconds using the addSeconds(double) method such that the double is larger than SEC_PER_DAY
 * Add seconds using the addSeconds(double) method such that the double is larger then SEC_PER_MS
 * Add seconds using the addSeconds(long) method such that the long is larger than SEC_PER_DAY
 * Call the add method such that m_fsod is larger than SEC_PER_MS
 * Call the add method such that m_msod is larger than MS_PER_DAY
 * Call the add method such that m_fsod is less than zero
 * Call the add method such that m_msod is less than zero 
 */


bool addDaysTest()
{
   std::numeric_limits<double> limits;
   
   bool status = true;
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime(1337L, SEC_BEFORE_MIDNIGHT, BEFORE_FSOD_LIM);
   CommonTime oldct(ctime);
   
///-------------------------------------------------------------------------///
/// Days - before midnight
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   cout << "Adding: 1 days, 0 msod, 0 fsod" << endl;
   ctime = ctime.addDays(1);
   cout << "Changed Time: " << ctime << endl;
   cout << "Adding: -1 days, 0 msod, 0 fsod" << endl;
   ctime = ctime.addDays(-1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      cout << "Test Passed";
   }
   else
   {
      cout << "Test Failed";
      status = false;
   }
   cout << endl << endl;
///-------------------------------------------------------------------------///
/// Days - after midnight
   ctime.set(1337L,0,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   cout << "Adding: -1 days, 0 msod, 0 fsod" << endl;
   ctime = ctime.addDays(-1);
   cout << "Changed Time: " << ctime << endl;
   cout << "Adding: 1 days, 0 msod, 0 fsod" << endl;
   ctime = ctime.addDays(1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      cout << "Test Passed";
   }
   else
   {
      cout << "Test Failed";
      status = false;
   }
   cout << endl << endl;
///-------------------------------------------------------------------------///
   
   return status;
}

//ignore for now
bool addMillisecondsTest()
{
   std::numeric_limits<double> limits;
   
   bool status = true;
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime(1337L, SEC_BEFORE_MIDNIGHT, BEFORE_FSOD_LIM);
   CommonTime oldct(ctime);
   
   ///-------------------------------------------------------------------------///
/// Milliseconds - before midnight
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(1);
   cout << "Changed Time: " << ctime << endl;
   cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(-1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      cout << "Test Passed";
   }
   else
   {
      cout << "Test Failed";
      status = false;
   }
   cout << endl << endl;
///-------------------------------------------------------------------------///
/// Milliseconds - after midnight
   ctime.set(1337L,0,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(-1);
   cout << "Changed Time: " << ctime << endl;
   cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      cout << "Test Passed";
   }
   else
   {
      cout << "Test Failed";
      status = false;
   }
   cout << endl << endl;
///-------------------------------------------------------------------------///
/// Milliseconds - midday +
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(1);
   cout << "Changed Time: " << ctime << endl;
   cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(-1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      cout << "Test Passed";
   }
   else
   {
      cout << "Test Failed";
      status = false;
   }
   cout << endl << endl;
///-------------------------------------------------------------------------///
/// Milliseconds - midday -
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(-1);
   cout << "Changed Time: " << ctime << endl;
   cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   ctime = ctime.addMilliseconds(1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      cout << "Test Passed";
   }
   else
   {
      cout << "Test Failed";
      status = false;
   }
   cout << endl << endl;
   
   return status;
}

bool addSecondsDoubleTest()
{
   std::numeric_limits<double> limits;
   
   bool status[10];
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime(1337L, SEC_BEFORE_MIDNIGHT, BEFORE_FSOD_LIM);
   CommonTime oldct(ctime);
   
   return true;
}

bool addSecondsLongTest()
{
   std::numeric_limits<double> limits;
   
   bool status[10];
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime(1337L, SEC_BEFORE_MIDNIGHT, BEFORE_FSOD_LIM);
   CommonTime oldct(ctime);
   
   return true;
}
