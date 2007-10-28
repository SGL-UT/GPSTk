
/**
 * @file SunPosition.hpp
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

        // Here we will store the results
        Xvt res;

        res = sun_position_CIS(t);
        res = CIS2CTS(res, t);

        return res;
    } // End SunPosition::getXvt



    /* Function to compute Sun position in CIS system (coordinates in meters)
     * @param t Epoch
     */
    Xvt sun_position_CIS(const DayTime& t)
    {
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
        double yearfrac = ( ( (4.0*(doy-1.0/((double)iy4+1.0)) - (double)iy4 - 2.0 ) + 4.0 * fd ) / 1461.0 );
        double time(years+yearfrac);

        // Compute the geometric mean longitude of the Sun
        double* intpar;
        double elm( modf((4.881628 + TWO_PI*yearfrac + 0.0001342*time)/TWO_PI, intpar) );

        // Mean longitude of perihelion
        double gamma(4.90823 + 3.0005e-4*time);

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
        modf((4.72 + 83.9971*time)/TWO_PI, intpar);
        double elmm(*intpar);

        // Useful definitions
        double coselt(cos(elt));
        double sineps(sin(eps0));
        double coseps(cos(eps0));
        double w1(-r*sin(elt));
        double selmm(sin(elmm));
        double celmm(cos(elm));

        Xvt result;

        // Sun position is the opposite of Earth position
        result.x.theArray[0] = (r*coselt+REMB*celmm)*AU_CONST;
        result.x.theArray[1] = (REMB*selmm-w1)*coseps*AU_CONST;
        result.x.theArray[2] = (-w1*sineps)*AU_CONST;

        return result;
    } // End sun_position_CIS()


    /* Function to change from CIS to CTS(ECEF) coordinate system (coordinates in meters)
     * @param posCis    Coordinates in CIS system (in meters).
     * @param t         Epoch
     */
    Xvt CIS2CTS(const Xvt posCIS, const DayTime& t)
    {

        // Angle of Earth rotation, in radians
        double ts( UTC2SID(t)*TWO_PI/24.0 );

        Xvt res;

        res.x.theArray[0] = cos(ts)*posCIS.x.theArray[0] + sin(ts)*posCIS.x.theArray[1];
        res.x.theArray[1] = -sin(ts)*posCIS.x.theArray[0] + cos(ts)*posCIS.x.theArray[1];
        res.x.theArray[2] = posCIS.x.theArray[2];

        return res;
    } // End CIS2CTS()


    /* Function to convert from UTC to sidereal time
     * @param t         Epoch
     *
     * @return sidereal time in hours
     */
    double UTC2SID(const DayTime& t)
    {

        // Fraction of day, in hours
        double frofday( t.secOfDay()/3600.0 );

        // Temporal value, in centuries
        double tt( (t.JD() - 2451545.0)/36525.0 );

        double sid( 24110.54841 + tt*( (8640184.812866) + tt*( (0.093104) - (6.2e-6*tt)) ) );

        double* intpar;
        sid = modf( (sid/3600.0 + frofday)/24.0, intpar);

        if(sid<0.0) sid = sid + 24.0;

        return sid;
    }


} // end namespace gpstk
