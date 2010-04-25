#pragma ident "$Id: $"

/**
 * @file UTCTime.cpp
 * class to convert among different time systems
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

#include "UTCTime.hpp"
#include "IERS.hpp"


namespace gpstk
{

   using namespace std;

	   // Seconds per day.
	const double UTCTime::DAY_TO_SECOND = 86400.0; 
	   
      // Constant used for conversion to Terrestrial Time.
	const double UTCTime::TT_TAI = 32.184;  
	   
      // Constant used for conversion to GPS time.
	const double UTCTime::TAI_GPS = 19.0;


	DayTime UTCTime::asUT1()
	{
		DayTime T(this->DOYyear(),this->DOYday(),this->DOYsecond()); // UTC
		T += IERS::UT1mUTC(this->mjdUTC());
		return T;
	}

	DayTime UTCTime::asUTC()
	{
		DayTime T(this->DOYyear(),this->DOYday(),this->DOYsecond());
		return T;
	}

	DayTime UTCTime::asTT()
	{
		DayTime T = (*this).asTAI();
		T += TT_TAI;
		return T;
	}

	DayTime UTCTime::asTDB()
	{
       
		DayTime TT = this->asTT();

		struct MJDTime mjdTT;
		mjdTT.MJDint = (long)floor(TT.MJD());
		mjdTT.MJDfr = TT.MJD() - mjdTT.MJDint;

		double tdbtdt = 0.0;
		double tdbtdtdot = 0.0;
		long oldmjd = 0;
		long l = 0;

		while ( mjdTT.MJDfr >= 1.0 ) 
		{
			mjdTT.MJDint++ ;
			mjdTT.MJDfr-- ;
		}
		while ( mjdTT.MJDfr < 0.0 ) 
		{
			mjdTT.MJDint-- ;
			mjdTT.MJDfr++ ;
		}

		if ( mjdTT.MJDint != oldmjd ) 
		{
			oldmjd = mjdTT.MJDint ;
			l = oldmjd + 2400001 ;

			tdbtdt = ctatv (l, 0.0) ;
			tdbtdtdot = ctatv (l, 0.5) - ctatv (l, -0.5) ;
		}

		double TDB_minus_TT = ( tdbtdt + (mjdTT.MJDfr - 0.5) * tdbtdtdot );

		DayTime T = TT;
		T += TDB_minus_TT;

		return T;
	}

	DayTime UTCTime::asTAI()
	{
		DayTime T(this->DOYyear(),this->DOYday(),this->DOYsecond()); // UTC
      T += IERS::TAImUTC(this->mjdUTC());		// TAI
		return T;
	}

	DayTime UTCTime::asGPST()
	{
		DayTime T(this->DOYyear(),this->DOYday(),this->DOYsecond()); // UTC
      T += IERS::TAImUTC(this->mjdUTC());	// TAI
		T += -UTCTime::TAI_GPS;			// GPST
		return T;
	}

      /// Return BD(Compass) Time
   DayTime UTCTime::asBDT()
   {
      DayTime bdt = this->asGPST();
      bdt -= 14.0;

      return bdt;
   }

	
	   /*
	    * Computes the cumulative relativistic time correction to
	    * earth-based clocks, TDB-TDT, for a given time. Routine
	    * furnished by the Bureau des Longitudes, modified by
	    * removal of terms much smaller than 0.1 microsecond.
	    * @param jdno. Julian day number of lookup
	    * @param fjdno. Fractional part of Julian day number
	    * @return Time difference TDB-TDT (seconds)
	    */
	double UTCTime::ctatv(long jdno, double fjdno)
	{

		double t, tt, t1, t2, t3, t4, t5, t24, t25, t29, t30, t31 ;

		t = ((jdno-2451545) + fjdno)/(365250.0) ;
		tt = t*t ;

      t1  =     1656.674564 * std::sin(  6283.075943033*t + 6.240054195)
         +        22.417471 * std::sin(  5753.384970095*t + 4.296977442)
         +        13.839792 * std::sin( 12566.151886066*t + 6.196904410)
         +         4.770086 * std::sin(   529.690965095*t + 0.444401603)
         +         4.676740 * std::sin(  6069.776754553*t + 4.021195093)
         +         2.256707 * std::sin(   213.299095438*t + 5.543113262)
         +         1.694205 * std::sin(    -3.523118349*t + 5.025132748)
         +         1.554905 * std::sin( 77713.772618729*t + 5.198467090)
         +         1.276839 * std::sin(  7860.419392439*t + 5.988822341)
         +         1.193379 * std::sin(  5223.693919802*t + 3.649823730)
         +         1.115322 * std::sin(  3930.209696220*t + 1.422745069)
         +         0.794185 * std::sin( 11506.769769794*t + 2.322313077)
         +         0.600309 * std::sin(  1577.343542448*t + 2.678271909)
         +         0.496817 * std::sin(  6208.294251424*t + 5.696701824)
         +         0.486306 * std::sin(  5884.926846583*t + 0.520007179)
         +         0.468597 * std::sin(  6244.942814354*t + 5.866398759)
         +         0.447061 * std::sin(    26.298319800*t + 3.615796498)
         +         0.435206 * std::sin(  -398.149003408*t + 4.349338347)
         +         0.432392 * std::sin(    74.781598567*t + 2.435898309)
         +         0.375510 * std::sin(  5507.553238667*t + 4.103476804) ;

      t2  =        0.243085 * std::sin(  -775.522611324*t + 3.651837925)
         +         0.230685 * std::sin(  5856.477659115*t + 4.773852582)
         +         0.203747 * std::sin( 12036.460734888*t + 4.333987818)
         +         0.173435 * std::sin( 18849.227549974*t + 6.153743485)
         +         0.159080 * std::sin( 10977.078804699*t + 1.890075226)
			+         0.143935 * std::sin(  -796.298006816*t + 5.957517795)
			+         0.137927 * std::sin( 11790.629088659*t + 1.135934669)
			+         0.119979 * std::sin(    38.133035638*t + 4.551585768)
			+         0.118971 * std::sin(  5486.777843175*t + 1.914547226)
			+         0.116120 * std::sin(  1059.381930189*t + 0.873504123)
			+         0.101868 * std::sin( -5573.142801634*t + 5.984503847)
			+         0.098358 * std::sin(  2544.314419883*t + 0.092793886)
			+         0.080164 * std::sin(   206.185548437*t + 2.095377709)
			+         0.079645 * std::sin(  4694.002954708*t + 2.949233637)
			+         0.075019 * std::sin(  2942.463423292*t + 4.980931759)
			+         0.064397 * std::sin(  5746.271337896*t + 1.280308748)
			+         0.063814 * std::sin(  5760.498431898*t + 4.167901731)
			+         0.062617 * std::sin(    20.775395492*t + 2.654394814)
			+         0.058844 * std::sin(   426.598190876*t + 4.839650148)
			+         0.054139 * std::sin( 17260.154654690*t + 3.411091093) ;

		t3  =        0.048373 * std::sin(   155.420399434*t + 2.251573730)
			+         0.048042 * std::sin(  2146.165416475*t + 1.495846011)
			+         0.046551 * std::sin(    -0.980321068*t + 0.921573539)
			+         0.042732 * std::sin(   632.783739313*t + 5.720622217)
			+         0.042560 * std::sin(161000.685737473*t + 1.270837679)
			+         0.042411 * std::sin(  6275.962302991*t + 2.869567043)
			+         0.040759 * std::sin( 12352.852604545*t + 3.981496998)
			+         0.040480 * std::sin( 15720.838784878*t + 2.546610123)
			+         0.040184 * std::sin(    -7.113547001*t + 3.565975565)
			+         0.036955 * std::sin(  3154.687084896*t + 5.071801441)
			+         0.036564 * std::sin(  5088.628839767*t + 3.324679049)
			+         0.036507 * std::sin(   801.820931124*t + 6.248866009)
			+         0.034867 * std::sin(   522.577418094*t + 5.210064075)
			+         0.033529 * std::sin(  9437.762934887*t + 2.404714239)
			+         0.033477 * std::sin(  6062.663207553*t + 4.144987272)
			+         0.032438 * std::sin(  6076.890301554*t + 0.749317412)
			+         0.032423 * std::sin(  8827.390269875*t + 5.541473556)
			+         0.030215 * std::sin(  7084.896781115*t + 3.389610345)
			+         0.029862 * std::sin( 12139.553509107*t + 1.770181024)
			+         0.029247 * std::sin(-71430.695617928*t + 4.183178762) ;

		t4  =        0.028244 * std::sin( -6286.598968340*t + 5.069663519)
			+         0.027567 * std::sin(  6279.552731642*t + 5.040846034)
			+         0.025196 * std::sin(  1748.016413067*t + 2.901883301)
			+         0.024816 * std::sin( -1194.447010225*t + 1.087136918)
			+         0.022567 * std::sin(  6133.512652857*t + 3.307984806)
			+         0.022509 * std::sin( 10447.387839604*t + 1.460726241)
			+         0.021691 * std::sin( 14143.495242431*t + 5.952658009)
			+         0.020937 * std::sin(  8429.241266467*t + 0.652303414)
			+         0.020322 * std::sin(   419.484643875*t + 3.735430632)
			+         0.017673 * std::sin(  6812.766815086*t + 3.186129845)
			+         0.017806 * std::sin(    73.297125859*t + 3.475975097)
			+         0.016155 * std::sin( 10213.285546211*t + 1.331103168)
			+         0.015974 * std::sin( -2352.866153772*t + 6.145309371)
			+         0.015949 * std::sin(  -220.412642439*t + 4.005298270)
			+         0.015078 * std::sin( 19651.048481098*t + 3.969480770)
			+         0.014751 * std::sin(  1349.867409659*t + 4.308933301)
			+         0.014318 * std::sin( 16730.463689596*t + 3.016058075)
			+         0.014223 * std::sin( 17789.845619785*t + 2.104551349)
			+         0.013671 * std::sin(  -536.804512095*t + 5.971672571)
			+         0.012462 * std::sin(   103.092774219*t + 1.737438797) ;

		t5  =        0.012420 * std::sin(  4690.479836359*t + 4.734090399)
			+         0.011942 * std::sin(  8031.092263058*t + 2.053414715)
			+         0.011847 * std::sin(  5643.178563677*t + 5.489005403)
			+         0.011707 * std::sin( -4705.732307544*t + 2.654125618)
			+         0.011622 * std::sin(  5120.601145584*t + 4.863931876)
			+         0.010962 * std::sin(     3.590428652*t + 2.196567739)
			+         0.010825 * std::sin(   553.569402842*t + 0.842715011)
			+         0.010396 * std::sin(   951.718406251*t + 5.717799605)
			+         0.010453 * std::sin(  5863.591206116*t + 1.913704550)
			+         0.010099 * std::sin(   283.859318865*t + 1.942176992)
			+         0.009858 * std::sin(  6309.374169791*t + 1.061816410)
			+         0.009963 * std::sin(   149.563197135*t + 4.870690598)
			+         0.009370 * std::sin(149854.400135205*t + 0.673880395) ;

		t24 = t * (102.156724 * std::sin(  6283.075849991*t + 4.249032005)
			+         1.706807 * std::sin( 12566.151699983*t + 4.205904248)
			+         0.269668 * std::sin(   213.299095438*t + 3.400290479)
			+         0.265919 * std::sin(   529.690965095*t + 5.836047367)
			+         0.210568 * std::sin(    -3.523118349*t + 6.262738348)
			+         0.077996 * std::sin(  5223.693919802*t + 4.670344204) ) ;

		t25 = t * (  0.059146 * std::sin(    26.298319800*t + 1.083044735)
			+         0.054764 * std::sin(  1577.343542448*t + 4.534800170)
			+         0.034420 * std::sin(  -398.149003408*t + 5.980077351)
			+         0.033595 * std::sin(  5507.553238667*t + 5.980162321)
			+         0.032088 * std::sin( 18849.227549974*t + 4.162913471)
			+         0.029198 * std::sin(  5856.477659115*t + 0.623811863)
			+         0.027764 * std::sin(   155.420399434*t + 3.745318113)
			+         0.025190 * std::sin(  5746.271337896*t + 2.980330535)
			+         0.024976 * std::sin(  5760.498431898*t + 2.467913690)
			+         0.022997 * std::sin(  -796.298006816*t + 1.174411803)
			+         0.021774 * std::sin(   206.185548437*t + 3.854787540)
			+         0.017925 * std::sin(  -775.522611324*t + 1.092065955)
			+         0.013794 * std::sin(   426.598190876*t + 2.699831988)
			+         0.013276 * std::sin(  6062.663207553*t + 5.845801920)
			+         0.012869 * std::sin(  6076.890301554*t + 5.333425680)
			+         0.012152 * std::sin(  1059.381930189*t + 6.222874454)
			+         0.011774 * std::sin( 12036.460734888*t + 2.292832062)
			+         0.011081 * std::sin(    -7.113547001*t + 5.154724984)
			+         0.010143 * std::sin(  4694.002954708*t + 4.044013795)
			+         0.010084 * std::sin(   522.577418094*t + 0.749320262)
			+         0.009357 * std::sin(  5486.777843175*t + 3.416081409) ) ;

		t29 = tt * ( 0.370115 * std::sin(                     4.712388980)
			+         4.322990 * std::sin(  6283.075849991*t + 2.642893748)
			+         0.122605 * std::sin( 12566.151699983*t + 2.438140634)
			+         0.019476 * std::sin(   213.299095438*t + 1.642186981)
			+         0.016916 * std::sin(   529.690965095*t + 4.510959344)
			+         0.013374 * std::sin(    -3.523118349*t + 1.502210314) ) ;

		t30 = t * tt * 0.143388 * std::sin( 6283.075849991*t + 1.131453581) ;

		return (t1+t2+t3+t4+t5+t24+t25+t29+t30) * 1.0e-6 ;

	}  // End of method 'UTCTime::ctatv()'


   double UTCTime::xPole()
   {
      return IERS::xPole( this->mjdUTC() );
   }

   double UTCTime::yPole()
   {
      return IERS::yPole( this->mjdUTC() );
   }

   double UTCTime::UT1mUTC()
   {
      return IERS::UT1mUTC( this->mjdUTC() );
   }

   double UTCTime::TAImUTC()
   {
      return IERS::TAImUTC( this->mjdUTC() );
   }

   double UTCTime::GPSTmUTC()
   {
      return (this->asGPST()-this->asUTC());
   }
	   

	void UTCTime::test()
	{
      
		cout<<"... testing UTCTime ..."<<endl;

      IERS::loadIERSFile("finals.data");
		
		UTCTime utc(short(2002),short(1),short(1),short(0),short(0),0.0);

		cout << "UTC "<< utc << endl
			 << "UT1 "<< utc.asUT1()<< endl
			 << "TT  "<< utc.asTT()	<< endl
			 << "TAI "<< utc.asTAI()<< endl
			 << "TDB "<< utc.asTDB()<< endl
			 << "GPST"<< utc.asGPST()<<endl;
		
		cout << "TAI-GPST "<<utc.asTAI()-utc.asGPST()<<endl;
		cout << "TT-TAI   "<<utc.asTT()-utc.asTAI()<<endl;
		cout << "UTC-TAI  "<<utc.asUTC()-utc.asTAI()<<endl;
		cout << "UT1-UTC  "<<utc.asUT1()-utc.asUTC()<<endl;
		cout << "TDB-TT   "<<utc.asTDB()-utc.asTT()<<endl;

		cout << "It's seems to be ok !" <<endl;
		
		DayTime utc2;
		DayTime gpst1;
		UTC2TT(utc,gpst1);
		TT2UTC(gpst1,utc2);
		
		cout<<setw(20)<<setprecision(8)<<utc2-utc<<endl;
      
      int a = 0;

	}

	
      // GPS time to UTC time
      // @param gpst    GPST as input 
      // @param utc     UTC as output
	void GPST2UTC(const DayTime& gpst, DayTime& utc)
	{
		UTCTime T;

		double mjdGPST = gpst.MJD();
		double mjdTAI = mjdGPST+ UTCTime::TAI_GPS/UTCTime::DAY_TO_SECOND;

         // input should be utc
      double dtu = IERS::TAImUTC(mjdTAI);					
		double mjdUTC = mjdTAI - dtu / UTCTime::DAY_TO_SECOND;

      dtu = IERS::TAImUTC(mjdUTC);
		mjdUTC = mjdTAI - dtu / UTCTime::DAY_TO_SECOND;		

		utc = gpst;						         // GPST
		utc += UTCTime::TAI_GPS;		      // TAI
      utc += -IERS::TAImUTC(mjdUTC);		// UTC

	}
      // UTC time to GPS time 
      // @param utc    UTC as input 
      // @param gpst   GPST as output 
	void UTC2GPST(const DayTime& utc, DayTime& gpst)
	{
		UTCTime T(utc.DOYyear(), utc.DOYday(), utc.DOYsecond());
		gpst = T.asGPST();
		
	}

      // UT1 time to UTC time
      // @param ut1     UT1 as input 
      // @param utc     UTC as output 
	void UT12UTC(const DayTime& ut1, DayTime& utc)
	{
		UTCTime T(ut1.DOYyear(), ut1.DOYday(), ut1.DOYsecond());
		
		T -= IERS::UT1mUTC(ut1.MJD());	// input should be utc
		double mjdUTC = T.MJD();

		T.setYDoySod(ut1.DOYyear(), ut1.DOYday(), ut1.DOYsecond());
		T -= IERS::UT1mUTC(mjdUTC);
		
		utc = ut1;
		utc -= IERS::UT1mUTC(T.MJD());
	}

      // UTC time to UT1 time
      // @param utc     UTC as input 
      // @param ut1     UT1 as output
	void UTC2UT1(const DayTime& utc, DayTime& ut1)
	{
		UTCTime T(utc.DOYyear(), utc.DOYday(), utc.DOYsecond());
		ut1 = T.asUT1();
	}

      // TT time to UTC time
      // @param tt      TT as input 
      // @param utc     UTC as output 
	void TT2UTC(const DayTime& tt, DayTime& utc)
	{
		UTCTime T(tt.DOYyear(),tt.DOYday(),tt.DOYsecond());
		
		DayTime TAI  = tt;			 // TT
		TAI -= UTCTime::TT_TAI;		 // TAI

		utc = TAI;
      utc -= IERS::TAImUTC(TAI.MJD()); // input should be UTC  	
		
		double mjdUTC = utc.MJD();

		utc = TAI;
      utc -= IERS::TAImUTC(mjdUTC);

		mjdUTC = utc.MJD();

		utc = TAI;
      utc -= IERS::TAImUTC(mjdUTC);
	}

      // UTC time to TT time
      // @param UTC     UTC as input 
      // @param tt      TT as output 
	void UTC2TT(const DayTime& utc, DayTime& tt)
	{
		UTCTime T(utc.DOYyear(),utc.DOYday(),utc.DOYsecond());
		tt = T.asTT();
	}

      // TAI time to UTC time
      // @param tai     TAI as input 
      // @param utc     UTC as output 
	void TAI2UTC(const DayTime& tai, DayTime& utc)
	{
		UTCTime T(tai.DOYyear(),tai.DOYday(),tai.DOYsecond());
		
		utc = tai;
      utc -= IERS::TAImUTC(tai.MJD()); // input should be UTC  	

		double mjdUTC = utc.MJD();

		utc = tai;
      utc -= IERS::TAImUTC(mjdUTC);

		mjdUTC = utc.MJD();

		utc = tai;
      utc -= IERS::TAImUTC(mjdUTC);
	}
   
      // TAI time to UTC time
      // @param utc     UTC as input 
      // @param tai     TAI as output 
	void UTC2TAI(const DayTime& utc, DayTime& tai)
	{
		UTCTime T(utc.DOYyear(),utc.DOYday(),utc.DOYsecond());
		tai = T.asTAI();
	}


      // BDT time to UTC time
      // @param bdt     BDT as input 
      // @param utc     UTC as output 
   void BDT2UTC(const DayTime& bdt, DayTime& utc)
   {
      DayTime gpst(bdt);
      gpst += 14.0;

      GPST2UTC(gpst, utc);
   }

      // UTC time to BDT time
      // @param utc     UTC as input 
      // @param bdt     BDT as output 
   void UTC2BDT(const DayTime& utc, DayTime& bdt)
   {
      UTC2GPST(utc,bdt);  
      bdt -= 14.0;
   }


}  // End of 'namespace gpstk'