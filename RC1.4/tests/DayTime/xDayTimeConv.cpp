// file: DayTimeConvTest.cpp

#include "DayTimeConvTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (DayTimeConvTest);


void DayTimeConvTest :: setUp (void)
{ 
	//Set microsecond tolerance
	gpstk::DayTime::setDayTimeTolerance(DayTime::DAYTIME_TOLERANCE);
}

//Tests DayTime using different constructers and then tests equality

void DayTimeConvTest :: constrTest (void)
{
	for (int z=1;z<=3;z++){
		switch (z) {
			case 1:
				//Directly from ICD-GPS-200
				//Beginning of GPS Time, as defined by ICD-GPS-200
				ccases = constrSetup(1980,1,6,0,0,0,0,0.,0,1981,44244.);
				break;
			case 2:
				//From GPS Signals and Performan, Misra and Enge, p. 91
				//GPS 10 bit week rollover epoch
				ccases = constrSetup(1999,8,22,0,0,0,0,0.,0,2000,51412.);
				break;
			case 3:
				//From Hoffman-Wellenhof, et al.
				//The J200 standard epoch
				ccases = constrSetup(2000,1,1,12,0,0,1042,561600.,
						374400,2000,2451545-2400000.5);
				break;
					    
		}
		
		//Assign values that will be tested from pointers to constrSetup
		for (int j = 0;j<=3;j++) 
		{ 
			ccase[j] = *ccases[j];
		}
		
		//Test to make sure there is no difference between different constructers
		for (int k = 0;k <= 3;k++) 
		{
			CPPUNIT_ASSERT_EQUAL (ccase[0], ccase[k]);
		}		
	}
}	

//Function that takes in constructer parameters and outputs a reference to the
//newly constructed DayTime objects

gpstk::DayTime** DayTimeConvTest :: constrSetup (short year,short month,short dom,
		short hour,short minute,double seconds,short week,double sow,long zcount,
		short hintYear,double MJD)
{ 
	//Construct objects
	gpstk::DayTime constr1(year,month,dom,hour,minute,seconds); //Calendar constructer
	gpstk::DayTime constr2(week%1024, sow, hintYear);       //10 bit GPS+SOW wk w/hint
	gpstk::DayTime constr3(week%1024,zcount,hintYear);     //10 bit GPS+Z count w/hint
	gpstk::DayTime constr4(MJD);                         //JD converted to MJD incline 
	cout << constr1.printf("%-9B %02d,  %Y %02H:%02M:%018.15f \n");
	
	//Create an array of references to the constructed objects
	gpstk::DayTime* constrs[4];
	constrs[0] = &constr1;
	constrs[1] = &constr2;
	constrs[2] = &constr3;
	constrs[3] = &constr4;
	return constrs;
	
}

//Tests DayTime using different mutators and makes sure that they are equal

void DayTimeConvTest :: mutatTest (void)
{
	for (int i=1;i<=3;i++){
		switch (i) {
			case 1:
				//Directly from ICD-GPS-200
				//Beginning of GPS Time, as defined by ICD-GPS-200
				mcases = mutatSetup(1980,1,6,0,0,0,6,0.,0,0.,0,1981,44244);
				break;
			case 2:
				//From GPS Signals and Performan, Misra and Enge, p. 91
				//GPS 10 bit week rollover epoch
				mcases = mutatSetup(1999,8,22,0,0,0,234,0.,0,0.,0,2000,51412);
				break;
			case 3:
				//From Hoffman-Wellenhof, et al.
				//The J200 standard epoch
				mcases = mutatSetup(2000,1,1,12,0,0,1,43200.,1042,
					   561600.,374400,2000,2451545-2400000.5);
				break;
		}
		
		//Assign values that will be tested from pointers to mutatSetup
		for (int j=0;j<=5;j++)
		{
			mcase[j] = *mcases[j]; 
		}
	
		//Test to make sure there is no difference between different mutators
		for (int k = 1;k<=3;k++)
		{
			CPPUNIT_ASSERT_EQUAL (mcase[0], mcase[k]);
		}
		CPPUNIT_ASSERT_EQUAL (mcase[0].GPSzcount(),mcase[4].GPSzcount());
		CPPUNIT_ASSERT (fabs(mcase[0].MJD() - mcase[5].MJD())<.001/DayTime::SEC_DAY);
	}
}

//Function that takes in mutator parameters and outputs a reference to the
//newly mutated DayTime objects

gpstk::DayTime** DayTimeConvTest :: mutatSetup (short year,short month,short dom,
				    short hour,short minute,double seconds,short doy,
				    double sod,short week,double sow,long zcount,
				    short hintYear,double MJD)
{ 
	gpstk::DayTime msetup[6];
	
	//Mutate the objects
	msetup[0].setYMD(year,month,dom);
	msetup[0].setHMS(hour,minute,seconds);
	msetup[1].setYMDHMS(year,month,dom,hour,minute,seconds);
	msetup[2].setYDoy(year,doy);
	msetup[2].setSecOfDay(sod);
	msetup[3].setGPS(week%1024, sow, hintYear);
	msetup[4].setGPS(week%1024,zcount,hintYear);
	msetup[5].setMJD(MJD);
	cout << msetup[5].printf("%-9B %02d,  %Y %02H:%02M:%018.15f \n");
	
	//Create an array of references to the mutated objects
	gpstk::DayTime* mutats[6];
	for (int i =0;i<=5;i++)
	{
		mutats[i] = &msetup[i];
	}
	return mutats;
	
}

//This test tests accessors and mutators using randomly generated dates

void DayTimeConvTest :: randTest (void)
{
	//Beginning and ending dates for the random date generation
	gpstk::DayTime dtBegin(1995,1,1,0,0,0), dtEnd(2015,1,1,0,0,0), dt;
	long ndates = 20; // Number of dates that will be generated
	
	//Set up componets to randomly generate date
	unsigned int seed = (unsigned int) dt.GPSsow();
	srand(seed);
	unsigned long dayDiff = (unsigned long) ceil(dtEnd.MJD() -dtBegin.MJD());
	
	//Loop over number of dates
	for  (int j=0;j<ndates;++j)
	{
		//Randomly generate a date
		double dayDelta = floor( rand()*1./RAND_MAX *dayDiff);
		double sodDelta = rand()*1./RAND_MAX;
		
		//Create a DayTime object and set it to the random day
		gpstk::DayTime testDate;
		testDate.setMJD(dtBegin.MJD()+dayDelta+sodDelta);
		
		//Setup the mutators using mutatSetup
		rcases = mutatSetup(testDate.year(),testDate.month(),testDate.day(),
			   testDate.hour(),testDate.minute(),testDate.second(),
			   testDate.DOY(),testDate.DOYsecond(),testDate.GPSfullweek()%1024,
			   testDate.GPSsow(),testDate.GPSzcount(),testDate.year(),
			   testDate.MJD());
		
		//Assign values that will be tested from pointers to mutatSetup
		for (int i = 0;i<=5;i++)
		{
	        	rcase[i] = *rcases[i];
		}
		
		//Test to make sure there is no difference between different mutators for the 
		//randomly generated dates
		for (int i=1;i<=3;i++)
		{
			CPPUNIT_ASSERT_EQUAL (rcase[0], rcase[i]);
		}
		CPPUNIT_ASSERT_EQUAL (rcase[0].GPSzcount(),rcase[4].GPSzcount());
		CPPUNIT_ASSERT (fabs(rcase[0].MJD() - rcase[5].MJD())<.001/DayTime::SEC_DAY);	
	}
		
}

