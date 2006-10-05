// file: xStats.cpp

#include "xStats.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION (xStats);

void xStats :: setUp (void) 
{ 
}	

/*
****Test to assess the quality of the Add member of the Stats
****class which is designed to add a gpstk::Vector or an individual 
****sample to an instance of Stats

**** This test partially indirectly tests Minimum, Maximum
**** Average,Variance,StdDev which are all memebers of the
**** Stats class
*/
void xStats :: addTest (void)
{
	gpstk::Stats<double> addTest;
	CPPUNIT_ASSERT_NO_THROW(addTest.Add(10.,1));
	
	addTest.Add(20.,1);
	addTest.Add(30.,1);
	//Create a vector of 1 member with a value of 40.0
	gpstk::Vector<double> one(1,40.);
	//Create a vector with one member with a value of 1
	gpstk::Vector<double> weight(1,1);
	
	
	CPPUNIT_ASSERT_NO_THROW(addTest.Add(one,weight));
	
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, addTest.N());
	CPPUNIT_ASSERT_EQUAL(10., addTest.Minimum());
	CPPUNIT_ASSERT_EQUAL(40., addTest.Maximum());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(25., addTest.Average(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(166.66667, addTest.Variance(),1E-3);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(12.9099445, addTest.StdDev(),1E-6);
	CPPUNIT_ASSERT_EQUAL(4., addTest.Normalization());
	CPPUNIT_ASSERT_EQUAL(true, addTest.Weighted());
	
	gpstk::Stats<double>  addTest0;
	gpstk::Vector<double> weight0(0,1);
	addTest0.Add(10.,0);
	addTest0.Add(20.,0);
	addTest0.Add(30.,0);
	addTest0.Add(one,weight0);
	
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, addTest0.N());
	CPPUNIT_ASSERT_EQUAL(10., addTest0.Minimum());
	CPPUNIT_ASSERT_EQUAL(40., addTest0.Maximum());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(25., addTest0.Average(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(166.66667, addTest0.Variance(),1E-3);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(12.9099445, addTest0.StdDev(),1E-6);
	CPPUNIT_ASSERT_EQUAL(0., addTest0.Normalization());
	CPPUNIT_ASSERT_EQUAL(false, addTest0.Weighted());
	
	
	gpstk::Vector<double> two(4,20.);
	gpstk::Vector<double> weight2(1,0);
	try
	{
		CPPUNIT_ASSERT_THROW(addTest.Add(two,weight2),gpstk::Exception);
	}
	catch(gpstk::Exception& e)
	{
	}
	
}

/*
**** Test to assess the quality of the Reset member of the Stats
**** class which is designed to reset a Stats instance to an unmodified
**** state

**** This test finishes the indirect testing of Minimum, Maximum
**** Average,Variance and StdDev which are all memebers of the
**** Stats class
*/


void xStats :: resetTest (void) 
{
	gpstk::Stats<double> resetTest;
	resetTest.Add(10.,1);
	resetTest.Add(20.,1);
	resetTest.Add(30.,1);
	gpstk::Vector<double> one(1,40.);
	gpstk::Vector<double> weight(1,1);
	resetTest.Add(one,weight);
	
	CPPUNIT_ASSERT_NO_THROW(resetTest.Reset());
	
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, resetTest.N());
	CPPUNIT_ASSERT_EQUAL(0., resetTest.Minimum());
	CPPUNIT_ASSERT_EQUAL(0., resetTest.Maximum());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest.Average(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest.Variance(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest.StdDev(),1E-6);
	CPPUNIT_ASSERT_EQUAL(0., resetTest.Normalization());
	CPPUNIT_ASSERT_EQUAL(false, resetTest.Weighted());
	resetTest.Add(10.,1);
	CPPUNIT_ASSERT_EQUAL(1., resetTest.Normalization());	
}

/*
**** Test to assess the quality of the Subtract member of the Stats
**** class which, according to comments by the coder, is designed to
**** "remove a sample from the computation of statistics"

**** Note: Removed average tests from suite.  Averages make little sense
**** without a weight within the subtract method.
*/
void xStats :: subtractTest (void)
{
	gpstk::Stats<double> subTest;
	subTest.Add(30.,1);
	subTest.Add(40.,1);
	subTest.Add(10.,1);
	subTest.Add(20.,1);
	//CPPUNIT_ASSERT_DOUBLES_EQUAL(25,subTest.Average(),1e-4);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4,subTest.N());
	subTest.Subtract(30);
	//CPPUNIT_ASSERT_EQUAL((40+10+20)/3.,subTest.Average());
	CPPUNIT_ASSERT_EQUAL((unsigned) 3,subTest.N());
	
	gpstk::Stats<double> subTest2;
	subTest2.Add(30.,1);
	subTest2.Add(40.,1);
	subTest2.Subtract(30);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,subTest2.Variance(),1e-4);
	//CPPUNIT_ASSERT_EQUAL(40.,subTest2.Average());
	CPPUNIT_ASSERT_EQUAL((unsigned) 1,subTest2.N());
	subTest2.Subtract(40);
	CPPUNIT_ASSERT_EQUAL((unsigned) 0,subTest2.N());
	//CPPUNIT_ASSERT_EQUAL(0.,subTest2.Average());
}

/*
**** Test to assess the quality of the LoadTest member of the Stats
**** class which, according to comments by the coder, is designed to
**** "define private members directly"
*/
void xStats :: loadTest (void)
{
	gpstk::Stats<int> loadTest;
	loadTest.Load((unsigned) 4, 0,20,1,2,true,4);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, loadTest.N());
	CPPUNIT_ASSERT_EQUAL(0, loadTest.Minimum());
	CPPUNIT_ASSERT_EQUAL(20, loadTest.Maximum());
	CPPUNIT_ASSERT_EQUAL(true, loadTest.Weighted());
	CPPUNIT_ASSERT_EQUAL(4, loadTest.Normalization());
	//30*10/2
	CPPUNIT_ASSERT_EQUAL(1, loadTest.Average());
	//var*10^4/2^4
	CPPUNIT_ASSERT_EQUAL(2, loadTest.Variance());
	
	
}

/*
**** Test to assess the quality of the overloaded += operator of the Stats
**** class which is designed to add data from one Stats instance onto another
**** Stats instance with the operator +=
*/
void xStats :: addEqualsTest (void)
{
	gpstk::Stats<int> addEquals1;
	addEquals1.Add(10,1);
	addEquals1.Add(20,1);
	
	gpstk::Stats<int> addEquals2;
	addEquals2.Add(30,0);
	addEquals2.Add(40,0);
	
	try
	{
		CPPUNIT_ASSERT_THROW(addEquals1+=addEquals2,gpstk::Exception);
	}
	catch(gpstk::Exception& e)
	{
	}
	gpstk::Stats<int> addEquals3;
	addEquals3.Add(30,1);
	addEquals3.Add(40,1);
	CPPUNIT_ASSERT_NO_THROW(addEquals1+=addEquals3);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, addEquals1.N());
	CPPUNIT_ASSERT_EQUAL(10, addEquals1.Minimum());
	CPPUNIT_ASSERT_EQUAL(40, addEquals1.Maximum());
	CPPUNIT_ASSERT_EQUAL(166, addEquals1.Variance());
	CPPUNIT_ASSERT_EQUAL(25, addEquals1.Average());	
	CPPUNIT_ASSERT_EQUAL(true,addEquals1.Weighted());
	CPPUNIT_ASSERT_EQUAL(4,addEquals1.Normalization());
	
	gpstk::Stats<int> addEquals4;
	gpstk::Stats<int> addEquals5;
	
	CPPUNIT_ASSERT_NO_THROW(addEquals4+=addEquals5);
	addEquals5.Add(30,0);
	
	addEquals4+=addEquals5;
	addEquals4.Add(10,0);
	
	
	addEquals5+=addEquals4;
	//CPPUNIT_ASSERT_EQUAL((unsigned) 2, addEquals1.N());
	CPPUNIT_ASSERT_EQUAL(10, addEquals5.Minimum());
	CPPUNIT_ASSERT_EQUAL(30, addEquals5.Maximum());
	CPPUNIT_ASSERT_EQUAL(156, addEquals5.Variance());
	CPPUNIT_ASSERT_EQUAL((30+30+10)/3, addEquals5.Average());	
	CPPUNIT_ASSERT_EQUAL(false,addEquals5.Weighted());
	CPPUNIT_ASSERT_EQUAL(0,addEquals5.Normalization());
}


/*
**** Test to assess the quality of the Add member of the TwoSampleStats
**** class which is designed to add a gpstk::Vector or a sample to an
**** instance of TwoSampleStats

**** This test partially indirectly tests MinX, MaxX
**** MinY, MaxY,AverageX,AverageY,VarianceX,VarianceY,StdDevX,StdDevY 
**** which are all memeber of the TwoSampleStats class
*/
void xStats :: add2Test (void)
{
	gpstk::TwoSampleStats<double> add2Test1;
	CPPUNIT_ASSERT_NO_THROW(add2Test1.Add(10.,20.));
	gpstk::Vector<double> addVectx(1,20.);
	gpstk::Vector<double> addVecty(1,30.);
	
	CPPUNIT_ASSERT_NO_THROW(add2Test1.Add(addVectx,addVecty));
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, add2Test1.N());
	CPPUNIT_ASSERT_EQUAL(10., add2Test1.MinimumX());
	CPPUNIT_ASSERT_EQUAL(20., add2Test1.MaximumX());
	CPPUNIT_ASSERT_EQUAL(20., add2Test1.MinimumY());
	CPPUNIT_ASSERT_EQUAL(30., add2Test1.MaximumY());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(15.0, add2Test1.AverageX(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0, add2Test1.AverageY(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0, add2Test1.VarianceX(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0, add2Test1.VarianceY(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(50.0), add2Test1.StdDevX(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(50.0), add2Test1.StdDevY(),1E-6);
	
}

/*
**** Test to assess the quality of the Reset member of the TwoSampleStats
**** class which is designed to return a TwoSampleStats intance to an
**** unmodified state

**** This test finishes indirectly testing the rest of MinX, MaxX
**** MinY, MaxY,AverageX,AverageY,VarianceX,VarianceY,StdDevX,StdDevY 
**** which are all members of the TwoSampleStats class.
*/
void xStats :: reset2Test (void)
{
	gpstk::TwoSampleStats<double> resetTest1;
  	resetTest1.Add(10.,20.);
	gpstk::Vector<double> resetx(1,20.);
	gpstk::Vector<double> resety(1,30.);
	resetTest1.Add(resetx,resety);
	
	CPPUNIT_ASSERT_NO_THROW(resetTest1.Reset());
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, resetTest1.N());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.MinimumX(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.MaximumX(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.MinimumY(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.MaximumY(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.AverageX(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.AverageY(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.VarianceX(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.VarianceY(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.StdDevX(), 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0., resetTest1.StdDevY(), 1e-6);	
	
}

void xStats :: subtract2Test (void)
{
	gpstk::TwoSampleStats<double> sub2Test;
	sub2Test.Add(30.,0.);
	sub2Test.Add(40.,10.);
	sub2Test.Add(10.,20.);
	sub2Test.Add(20.,30.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(25,sub2Test.AverageX(),1e-4);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(15,sub2Test.AverageY(),1e-4);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4,sub2Test.N());
	sub2Test.Subtract(20,30);
	CPPUNIT_ASSERT_DOUBLES_EQUAL((30+40+10)/3.,sub2Test.AverageX(),1e-4);
	CPPUNIT_ASSERT_DOUBLES_EQUAL((0+10+20)/3.,sub2Test.AverageY(),1e-4);
	CPPUNIT_ASSERT_EQUAL((unsigned) 3,sub2Test.N());
	
	sub2Test.Add(20.,30.);
	gpstk::Vector<double> subVectorX(1,20);
	gpstk::Vector<double> subVectorY(1,30);
	sub2Test.Subtract(subVectorX,subVectorY);
	CPPUNIT_ASSERT_DOUBLES_EQUAL((30+40+10)/3.,sub2Test.AverageX(),1e-4);
	CPPUNIT_ASSERT_DOUBLES_EQUAL((0+10+20)/3.,sub2Test.AverageY(),1e-4);
	CPPUNIT_ASSERT_EQUAL((unsigned) 3,sub2Test.N());
	
	sub2Test.Subtract(10.,20.);
	sub2Test.Subtract(40.,10.);
	sub2Test.Subtract(30.,0.);
	
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,sub2Test.AverageX(),1e-4);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,sub2Test.AverageY(),1e-4);
}

/*
****Test to assess the quality of the Slope member of the TwoSampleStats
****class which is designed to return the slope of the best fit line
****Y=slope*X + intercept
*/
void xStats :: slopeTest (void)
{
	gpstk::TwoSampleStats<double> slopeTest;
	CPPUNIT_ASSERT_NO_THROW(slopeTest.Slope());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,slopeTest.Slope(),1e-6);
	slopeTest.Add(0.,0.);
	slopeTest.Add(10.,10.);
	slopeTest.Add(20.,20.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,slopeTest.Slope(),1e-6);
	
	gpstk::TwoSampleStats<double> slopeTest2;
	slopeTest2.Add(0.,0.);
	slopeTest2.Add(10.,5.);
	slopeTest2.Add(20.,10.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(.5,slopeTest2.Slope(),1e-6);
}

/*
****Test to assess the quality of the Intercept member of the TwoSampleStats
****class which is designed to return the intercept with the Y axis of the best
****fit line Y=slope*X + intercept
*/
void xStats :: interceptTest (void)
{
	gpstk::TwoSampleStats<double> interceptTest;
	CPPUNIT_ASSERT_NO_THROW(interceptTest.Intercept());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,interceptTest.Intercept(),1e-6);
	interceptTest.Add(0.,0.);
	interceptTest.Add(10.,10.);
	interceptTest.Add(20.,20.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,interceptTest.Intercept(),1e-6);
	
	gpstk::TwoSampleStats<double> interceptTest2;
	interceptTest2.Add(10.,10.);
	interceptTest2.Add(20.,15.);
	interceptTest2.Add(30.,20.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5.,interceptTest2.Intercept(),1e-6);
}

/*
****Test to assess the quality of the SigmaSlop member of the TwoSampleStats
****class which is designed to return the slope of the best fit line
****Y=slope*X + intercept

****This test has not been checked throughly by hand, as I do not have the correct
****tools at the moment.
*/
void xStats :: sigmaSlopeTest (void)
{
	gpstk::TwoSampleStats<double> sigmaSlopeTest;
	CPPUNIT_ASSERT_NO_THROW(sigmaSlopeTest.SigmaSlope());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,sigmaSlopeTest.SigmaSlope(),1e-6);
	sigmaSlopeTest.Add(0.,0.);
	sigmaSlopeTest.Add(10.,10.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,sigmaSlopeTest.SigmaSlope(),1e-6);
	sigmaSlopeTest.Add(20.,21.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(.0288675,sigmaSlopeTest.SigmaSlope(),1e-6);
	
}

/*
****Test to assess the quality of the Correlation member of the TwoSampleStats
****class which is designed to return the correlation between X and Y
*/
void xStats :: correlationTest (void)
{
	gpstk::TwoSampleStats<double> correlationTest;
	CPPUNIT_ASSERT_NO_THROW(correlationTest.Correlation());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,correlationTest.Correlation(),1e-6);
	correlationTest.Add(0.,0.);
	correlationTest.Add(10.,10.);
	correlationTest.Add(20.,20.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.,correlationTest.Correlation(),1e-6);
}

/*
****Test to assess the quality of the SigmaYX member of the TwoSampleStats
****class which is designed to return the conditional uncertainty of y given x
*/
void xStats :: sigmayxTest (void)
{
	gpstk::TwoSampleStats<double> sigmayxTest;
	CPPUNIT_ASSERT_NO_THROW(sigmayxTest.SigmaYX());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,sigmayxTest.SigmaYX(),1e-6);
	sigmayxTest.Add(0.,0.);
	sigmayxTest.Add(10.,10.);
	sigmayxTest.Add(20.,20.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,sigmayxTest.SigmaYX(),1e-6);
}

/*
****Test to assess the quality of the += operator of the TwoSampleStats
****class which is designed to overload the += operator to add on data to
****TwoSampleStats instantiantions
*/
void xStats :: addEquals2Test (void)
{
	gpstk::TwoSampleStats<double> addEquals2Test1;
	addEquals2Test1.Add(10.,20.);
	gpstk::Vector<double> addVectx(1,20.);
	gpstk::Vector<double> addVecty(1,30.);
	gpstk::TwoSampleStats<double> addEquals2Test2;
	addEquals2Test2.Add(addVectx,addVecty);
	CPPUNIT_ASSERT_NO_THROW(addEquals2Test1+=addEquals2Test2);
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, addEquals2Test1.N());
	CPPUNIT_ASSERT_EQUAL(10., addEquals2Test1.MinimumX());
	CPPUNIT_ASSERT_EQUAL(20., addEquals2Test1.MaximumX());
	CPPUNIT_ASSERT_EQUAL(20., addEquals2Test1.MinimumY());
	CPPUNIT_ASSERT_EQUAL(30., addEquals2Test1.MaximumY());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(15.0, addEquals2Test1.AverageX(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0, addEquals2Test1.AverageY(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0, addEquals2Test1.VarianceX(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0, addEquals2Test1.VarianceY(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(50.0), addEquals2Test1.StdDevX(),1E-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(50.0), addEquals2Test1.StdDevY(),1E-6);
}

/*
**** Test to assess the quality of the median function for finding the median
**** of a gpstk::Vector  
*/
void xStats :: medianTest (void)
{
	/*
	gpstk::Vector<double> medianTest(10,10.);
	cout << gpstk::median(medianTest);
	CPPUNIT_ASSERT_EQUAL(0.,gpstk::median(medianTest));
	for (int i =0;i<11;i++)
	{
		medianTest(i) = i;
	}
	//CPPUNIT_ASSERT_EQUAL(5.,gpstk::median(medianTest));
	medianTest[0] = 20;
	CPPUNIT_ASSERT_EQUAL(5.5,gpstk::median(medianTest));
	*/
	
}
