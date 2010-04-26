#pragma ident "$Id: $"

/**
* @file SphericalHarmonicGravity.cpp
* 
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



#include "SphericalHarmonicGravity.hpp"
#include "ASConstant.hpp"
#include "IERS.hpp"
#include "ReferenceFrames.hpp"
#include "SpecialFunctions.hpp"

namespace gpstk
{
	   /* Constructor.
	    * @param n Desired degree.
	    * @param m Desired order.
	    */
	SphericalHarmonicGravity::SphericalHarmonicGravity(int n, int m)
		: desiredDegree(n),
        desiredOrder(m),
        correctSolidTide(false),
        correctOceanTide(false),
        correctPoleTide(false)
	{
      const int size = desiredDegree;

		V.resize( size + 3, size + 3, 0.0);
		W.resize( size + 3, size + 3, 0.0);

      //Sn0.resize(gmData.maxDegree, 0.0);

	}

	
	   /* Evaluates the two harmonic functions V and W.
	    * @param r ECI position vector.
	    * @param E ECI to ECEF transformation matrix.
	    */
	void SphericalHarmonicGravity::computeVW(Vector<double> r, Matrix<double> E)
	{	
		// dimension should be checked here
		// I'll do it latter...
		if((r.size()!=3) || (E.rows()!=3) || (E.cols()!=3))
		{
			Exception e("Wrong input for computeVW");
			GPSTK_THROW(e);
		}

		// Rotate from ECI to ECEF
		Vector<double> r_bf = E * r; 

      const double R_ref = gmData.refDistance;

		// Auxiliary quantities
		double r_sqr =  dot(r_bf, r_bf);
		double rho   =  R_ref * R_ref / r_sqr;

      // Normalized coordinates
		double x0 = R_ref * r_bf(0) / r_sqr;          
		double y0 = R_ref * r_bf(1) / r_sqr;   
		double z0 = R_ref * r_bf(2) / r_sqr;


      //
		// Evaluate harmonic functions 
		//   V_nm = (R_ref/r)^(n+1) * P_nm(sin(phi)) * cos(m*lambda)
		// and 
		//   W_nm = (R_ref/r)^(n+1) * P_nm(sin(phi)) * sin(m*lambda)
		// up to degree and order n_max+1
		//

		// Calculate zonal terms V(n,0); set W(n,0)=0.0
		V[0][0] = R_ref / std::sqrt(r_sqr);
		W[0][0] = 0.0;

		V[1][0] = z0 * V[0][0];
		W[1][0] = 0.0;

		for(int n = 2; n <= (desiredDegree+2); n++) 
		{
			V[n][0] = ((2*n - 1) * z0 * V[n-1][0] - (n - 1) * rho * V[n-2][0]) /n;
			W[n][0] = 0.0;
		}

		// Calculate tesseral and sectorial terms
		for (int m = 1; m <= (desiredOrder + 2); m++) 
		{
			// Calculate V(m,m) .. V(n_max+1,m)

			V[m][m] = (2 * m - 1) * ( x0 * V[m-1][m-1] - y0 * W[m-1][m-1] );
			W[m][m] = (2 * m - 1) * ( x0 * W[m-1][m-1] + y0 * V[m-1][m-1] );

			if (m <= (desiredDegree+1) ) 
			{
				V[m+1][m] = (2 * m + 1) * z0 * V[m][m];
				W[m+1][m] = (2 * m + 1) * z0 * W[m][m];
			}

			for (int n = (m+2); n <= (desiredDegree+2); n++) 
			{
				V[n][m] = ((2*n-1)*z0*V[n-1][m] - (n+m-1)*rho*V[n-2][m]) / (n-m);
				W[n][m] = ((2*n-1)*z0*W[n-1][m] - (n+m-1)*rho*W[n-2][m]) / (n-m);
			}

		}  // End 'for (int m = 1; m <= (desiredOrder + 2); m++) '

	}  // End of method 'SphericalHarmonicGravity::computeVW()'


	   /* Computes the acceleration due to gravity in m/s^2.
	    * @param r ECI position vector.
	    * @param E ECI to ECEF transformation matrix.
	    * @return ECI acceleration in m/s^2.
	    */
	Vector<double> SphericalHarmonicGravity::gravity(Vector<double> r, Matrix<double> E)
	{
		// dimension should be checked here
		// I'll do it latter...
		if((r.size()!=3) || (E.rows()!=3) || (E.cols()!=3))
		{
			Exception e("Wrong input for computeVW");
			GPSTK_THROW(e);
		}

      Matrix<double> CS = gmData.unnormalizedCS;

	
		// Calculate accelerations ax,ay,az
		double ax(0.0), ay(0.0), az(0.0);

		for (int m = 0; m <= (desiredOrder+1); m++)
		{
			for (int n = m; n <= (desiredDegree+1) ; n++)
			{
				if (m==0) 
				{
					double C = CS[n][0];					// = C_n,0

					ax -=       C * V[n+1][1];
					ay -=       C * W[n+1][1];
					az -= (n+1)*C * V[n+1][0];
				}
				else 
				{
					double C = CS[n][m];   // = C_n,m
					double S = CS[m-1][n]; // = S_n,m
					double Fac = 0.5 * (n-m+1) * (n-m+2);
					
               ax += 0.5*(-C*V[n+1][m+1] - S*W[n+1][m+1]) + Fac*(C*V[n+1][m-1] + S*W[n+1][m-1]);
					ay += 0.5*(-C*W[n+1][m+1] + S*V[n+1][m+1]) + Fac*(-C*W[n+1][m-1] + S*V[n+1][m-1]);
					az += (n-m+1)*(-C*V[n+1][m] - S*W[n+1][m]);
				}

			}  // End of 'for (int n = m; n <= (desiredDegree+1) ; n++)'

		}  // End of 'for (int m = 0; m <= (desiredOrder+1); m++)'

		// Body-fixed acceleration
		Vector<double> a_bf(3,0.0);
		a_bf(0) = ax;
		a_bf(1) = ay;
		a_bf(2) = az;

      a_bf = a_bf * ( gmData.GM / (gmData.refDistance * gmData.refDistance) );

		// Inertial acceleration
		Matrix<double> Etrans = transpose(E);
		Vector<double> out = Etrans * a_bf;				// this line may be wrong  matrix * vector

		return out;

	}  // End of method 'SphericalHarmonicGravity::gravity'


	   /* Computes the partial derivative of gravity with respect to position.
	    * @return ECI gravity gradient matrix.
	    * @param r ECI position vector.
	    * @param E ECI to ECEF transformation matrix.
	    */
	Matrix<double> SphericalHarmonicGravity::gravityGradient(gpstk::Vector<double> r, gpstk::Matrix<double> E)
	{
		// dimension should be checked here
		// I'll do it latter...
		if((r.size()!=3) || (E.rows()!=3) || (E.cols()!=3))
		{
			Exception e("Wrong input for gravityGradient");
			GPSTK_THROW(e);
		}

      Matrix<double> CS = gmData.unnormalizedCS;

	
		double xx = 0.0;     
		double xy = 0.0;
		double xz = 0.0;
		double yy = 0.0;
		double yz = 0.0;
		double zz = 0.0;

		Matrix<double> out(3, 3, 0.0);

		for (int m = 0; m <= (desiredOrder+1); m++) 
		{
			for (int n = m; n <= (desiredDegree+1); n++) 
			{
				double Fac = (n-m+2)*(n-m+1);
				
            double C = CS[n][m];
				double S = (m==0) ? 0.0 : CS[m-1][n];	// yan changed
				//S = (m==0)?Sn0(n):CS[m-1][n];	// yan changed

				zz += Fac*(C*V[n+2][m] + S*W[n+2][m]);

				if (m==0) 
				{
					C = CS[n][0];   // = C_n,0

					Fac = (n+2)*(n+1);
					xx += 0.5 * (C*V[n+2][2] - Fac*C*V[n+2][0]);
					xy += 0.5 * C * W[n+2][2];
					
               Fac = n + 1;
					xz += Fac * C * V[n+2][1];
					yz += Fac * C * W[n+2][1];
				}
				if (m > 0)
				{
					C = CS[n][m];
					S = CS[m-1][n];
					
               double f1 = 0.5*(n-m+1);
					double f2 = (n-m+3)*(n-m+2)*f1;

					xz += f1*(C*V[n+2][m+1]+S*W[n+2][m+1])-f2*(C*V[n+2][m-1]+S*W[n+2][m-1]);
					yz += f1*(C*W[n+2][m+1]-S*V[n+2][m+1])+f2*(C*W[n+2][m-1]-S*V[n+2][m-1]);         //* bug in JAT, I fix it
          
					if (m == 1)
					{
						Fac = (n+1)*n;
						xx += 0.25*(C*V[n+2][3]+S*W[n+2][3]-Fac*(3.0*C*V[n+2][1]+S*W[n+2][1]));
						xy += 0.25*(C*W[n+2][3]-S*V[n+2][3]-Fac*(C*W[n+2][1]+S*V[n+2][1]));
					}
					if (m > 1) 
					{
						f1 = 2.0*(n-m+2)*(n-m+1);
						f2 = (n-m+4)*(n-m+3)*f1*0.5;
						xx += 0.25*(C*V[n+2][m+2]+S*W[n+2][m+2]-f1*(C*V[n+2][m]+S*W[n+2][m])+f2*(C*V[n+2][m-2]+S*W[n+2][m-2]));

						xy += 0.25*(C*W[n+2][m+2]-S*V[n+2][m+2]+f2*(-C*W[n+2][m-2]+S*V[n+2][m-2]));
					}
				}
			}
			yy = -xx - zz;
			
			out(0,0) = xx;
			out(0,1) = xy;
			out(0,2) = xz;
			out(1,0) = xy;
			out(1,1) = yy;
			out(1,2) = yz;
			out(2,0) = xz;
			out(2,1) = yz;
			out(2,2) = zz;

		}  // for (int m = 0; m <= desiredOrder; m++) 

      const double R_ref = gmData.refDistance;
		out = out * (gmData.GM / (R_ref * R_ref * R_ref));

		// Rotate to ECI
		Matrix<double> Etrans = transpose(E);
		out = Etrans*(out*E);

		return out;			// the result should be checked

	}  // End of 'SphericalHarmonicGravity::gravityGradient()'

	
	   
		/** Call the relevant methods to compute the acceleration.
		 * @param utc Time reference class
		 * @param rb  Reference body class
		 * @param sc  Spacecraft parameters and state
		 * @return the acceleration [m/s^s]
	    */
	void SphericalHarmonicGravity::doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc)
	{

      Matrix<double> C2T = ReferenceFrames::J2kToECEFMatrix(utc);

		/*
		// debuging
		-0.96093274494562253,0.27678089792921495,0.00077086494829907383
		-0.27678077751454710,-0.96093305341706370,0.00026086203590256260
		0.00081295123707397028,3.7310272463024317e-005,0.99999966885906000
		
		C2T(0,0) = -0.96093274494562253;
		C2T(0,1) = 0.27678089792921495;
		C2T(0,2) = 0.00077086494829907383;

		C2T(1,0) = -0.27678077751454710;
		C2T(1,1) = -0.96093305341706370;
		C2T(1,2) = 0.00026086203590256260;

		C2T(2,0) = 0.00081295123707397028;
		C2T(2,1) = 3.7310272463024317e-005;
		C2T(2,2) = 0.99999966885906000;*/
		
		// corrcet earth tides
		correctCSTides(utc, correctSolidTide, correctOceanTide, correctPoleTide);

		// Evaluate harmonic functions
		computeVW(sc.R(), C2T);			// update VM

		// a
		a = gravity(sc.R(), C2T);
		
		// da_dr
		da_dr = gravityGradient(sc.R(), C2T);
		
		//da_dv
		da_dv.resize(3,3,0.0);
		
		//da_dp
		
	}

	// Correct tides to coefficients 
	void SphericalHarmonicGravity::correctCSTides(UTCTime t,int solidFlag,int oceanFlag,int poleFlag)
	{
		// copy CS
      Matrix<double> CS = gmData.unnormalizedCS;
		Vector<double> Sn0(CS.rows(),0.0);

		// 
		double mjd = t.MJD();
      double leapYears = (mjd-gmData.refMJD)/365.25;

		double detC20 = normFactor(2,0)*leapYears*gmData.dotC20;
		double detC21 = normFactor(2,1)*leapYears*gmData.dotC21;
		double detS21 = normFactor(2,1)*leapYears*gmData.dotS21;

		CS(2,0) += detC20;
		CS(2,1) += detC21;
		CS(0,2) += detS21;
		
		// correct solid tide
		if(solidFlag!=0)
		{
			// C20 C21 C22 C30 C31 C32 C33 C40 C41 C42
			double dc[10] = {0.0};
			double ds[10] = {0.0};
			solidTide.getSolidTide(t.mjdUTC(),dc,ds);

			// c
			CS(2,0) += normFactor(2,0)*dc[0];
			CS(2,1) += normFactor(2,1)*dc[1];
			CS(2,2) += normFactor(2,2)*dc[2];
			CS(3,0) += normFactor(3,0)*dc[3];
			CS(3,1) += normFactor(3,1)*dc[4];
			CS(3,2) += normFactor(3,2)*dc[5];
			CS(3,3) += normFactor(3,3)*dc[6];
			CS(4,0) += normFactor(4,0)*dc[7];
			CS(4,1) += normFactor(4,1)*dc[8];
			CS(4,2) += normFactor(4,2)*dc[9];
			/// s
			Sn0(2)  += normFactor(2,0)*ds[0];	// s20
			CS(0,2) += normFactor(2,1)*ds[1];
			CS(1,2) += normFactor(2,2)*ds[2];
			Sn0(3)  += normFactor(3,0)*ds[3];	// s30
			CS(0,3) += normFactor(3,1)*ds[4];
			CS(1,3) += normFactor(3,2)*ds[5];
			CS(2,3) += normFactor(3,3)*ds[6];	
			Sn0(4)  += normFactor(4,0)*ds[7];	// s40
			CS(0,4) += normFactor(4,1)*ds[8];
			CS(1,4) += normFactor(4,2)*ds[9];

		}
		
		// correct ocean tide
		if(oceanFlag!=0)
		{
			// C20 C21 C22 C30 C31 C32 C33 C40 C41 C42 C43 C44
			double dc[12] = {0.0};
			double ds[12] = {0.0};
			oceanTide.getOceanTide(t.mjdUTC(),dc,ds);
			
			// c
			CS(2,0) += normFactor(2,0)*dc[0];
			CS(2,1) += normFactor(2,1)*dc[1];
			CS(2,2) += normFactor(2,2)*dc[2];
			CS(3,0) += normFactor(3,0)*dc[3];
			CS(3,1) += normFactor(3,1)*dc[4];
			CS(3,2) += normFactor(3,2)*dc[5];
			CS(3,3) += normFactor(3,3)*dc[6];
			CS(4,0) += normFactor(4,0)*dc[7];
			CS(4,1) += normFactor(4,1)*dc[8];
			CS(4,2) += normFactor(4,2)*dc[9];
			CS(4,3) += normFactor(4,3)*dc[10];
			CS(4,4) += normFactor(4,4)*dc[11];


			/// s
			Sn0(2)  += normFactor(2,0)*ds[0];	// s20
			CS(0,2) += normFactor(2,1)*ds[1];
			CS(1,2) += normFactor(2,2)*ds[2];
			Sn0(3)  += normFactor(3,0)*ds[3];	// s30
			CS(0,3) += normFactor(3,1)*ds[4];
			CS(1,3) += normFactor(3,2)*ds[5];
			CS(2,3) += normFactor(3,3)*ds[6];
			Sn0(4)  += normFactor(4,0)*ds[7];	// s40
			CS(1,4) += normFactor(4,1)*ds[8];
			CS(2,4) += normFactor(4,2)*ds[9];
			CS(3,4) += normFactor(4,1)*ds[10];
			CS(4,4) += normFactor(4,2)*ds[11];

		}
		
		// correct pole tide
		if(poleFlag!=0)
		{
			double dC21=0.0;
			double dS21=0.0;
			poleTide.getPoleTide(t.mjdUTC(),dC21,dS21);

			CS(2,1) += normFactor(2,1)*dC21;
			CS(0,2) += normFactor(2,1)*dS21;
		}

	}  // End of method 'SphericalHarmonicGravity::correctCSTides()'


	double SphericalHarmonicGravity::normFactor(int n, int m) 
	{
      // The input should be n >= m >= 0

      double fac(1.0);
      for(int i = (n-m+1); i <= (n+m); i++)
      {
         fac = fac * double(i);
      }

      double delta  = (m == 0) ? 1.0 : 0.0;

      double num = (2.0 * n + 1.0) * (2.0 - delta);

      // We should make sure fac!=0, but it won't happen on the case,
      // so we just skip handling it
		double out = std::sqrt(num/fac);                  
		
		return out;

	}  // End of method 'SphericalHarmonicGravity::normFactor())


}  // End of namespace 'gpstk'

