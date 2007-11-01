
/**
 * @file SunPosition.cpp
 * Returns the approximate position of the Sun at the given epoch in the 
 * ECEF system.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "SunPosition.hpp"


namespace gpstk
{

    // Returns the position of Sun ECEF coordinates (meters) at the indicated time.
    // @param[in] t the time to look up
    // @return the Xvt of the Sun at time
    // @throw InvalidRequest If the request can not be completed for any
    //    reason, this is thrown. The text may have additional
    //    information as to why the request failed.
    Xvt SunPosition::getXvt(const DayTime& t) const throw(InvalidRequest)
    {

        // Test if the time interval is correct
        if ( (t < initialTime) || (t > finalTime) )
        {
            InvalidRequest ir("Provided epoch is out of bounds.");
            GPSTK_THROW(ir);
        }

        // Here we will store the results
        Xvt res;

        res = SunPosition::getXvtCIS(t);
        res = CIS2CTS(res, t);

        return res;
    } // End SunPosition::getXvt



    /* Function to compute Sun position in CIS system (coordinates in meters)
     * @param t Epoch
     */
    Xvt SunPosition::getXvtCIS(const DayTime& t) const throw(InvalidRequest)
    {

        // Test if the time interval is correct
        if ( (t < initialTime) || (t > finalTime) )
        {
            InvalidRequest ir("Provided epoch is out of bounds.");
            GPSTK_THROW(ir);
        }

        // Astronomical Unit value (AU), in meters
        const double AU_CONST(1.49597870e11);

        //Mean Earth-Moon barycenter (EMB) distance (AU)
        const double REMB(3.12e-5);

        // Compute the years, and fraction of year, pased since J1900.0
        int y(t.year());    // Current year
        int doy(t.DOY());   // Day of current year
        double fd( (t.secOfDay()/86400.0 ) );   // Fraction of day
        int years( (y - 1900) );    // Integer number of years since J1900.0
        int iy4( ( ((y%4)+4)%4 ) ); // Is it a leap year?
        // Compute fraction of year
        double yearfrac = ( ( (double)(4*(doy-1/(iy4+1)) - iy4 - 2) + 4.0 * fd ) / 1461.0 );
        double time(years+yearfrac);

        // Compute the geometric mean longitude of the Sun
        double elm( fmod((4.881628 + TWO_PI*yearfrac + 0.0001342*time), TWO_PI) );

        // Mean longitude of perihelion
        double gamma(4.90823 + 0.00030005*time);

        // Mean anomaly
        double em(elm-gamma);

        // Mean obliquity
        double eps0(0.40931975 - 2.27e-6*time);

        // Eccentricity
        double e(0.016751 - 4.2e-7*time);
        double esq(e*e);

        // True anomaly
        double v(em + 2.0*e*sin(em) + 1.25*esq*sin(2.0*em));

        // True ecliptic longitude
        double elt(v+gamma);

        // True distance
        double r( (1.0 - esq)/(1.0 + e*cos(v)) );

        // Moon's mean longitude
        double elmm( fmod((4.72 + 83.9971*time),TWO_PI) );

        // Useful definitions
        double coselt(cos(elt));
        double sineps(sin(eps0));
        double coseps(cos(eps0));
        double w1(-r*sin(elt));
        double selmm(sin(elmm));
        double celmm(cos(elmm));

        Xvt result;

        // Sun position is the opposite of Earth position
        result.x.theArray[0] = (r*coselt+REMB*celmm)*AU_CONST;
        result.x.theArray[1] = (REMB*selmm-w1)*coseps*AU_CONST;
        result.x.theArray[2] = (-w1*sineps)*AU_CONST;

        return result;
    } // End SunPosition::getXvtCIS()


} // end namespace gpstk
