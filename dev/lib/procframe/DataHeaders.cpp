
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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================

#include "DataHeaders.hpp"


/**
 * @file DataHeaders.cpp
 * gpstk::DataHeaders: Set of several headers to be used by data structures.
 */

//using namespace gpstk::StringUtils;
using namespace std;


namespace gpstk
{

    // Convenience output method for sourceHeader
    std::ostream& sourceHeader::dump(std::ostream& s) const
    {
        s << source;
        return s;
    } // sourceHeader::dump()


    // stream output for sourceHeader
    std::ostream& operator<<(std::ostream& s, const sourceHeader& sh)
    {
        sh.dump(s);
        return s;
    }


    // Convenience output method for sourceEpochHeader
    std::ostream& sourceEpochHeader::dump(std::ostream& s) const
    {
        s << source << " " << epoch;
        return s;
    } // sourceEpochHeader::dump()


    // stream output for sourceEpochHeader
    std::ostream& operator<<(std::ostream& s, const sourceEpochHeader& seh)
    {
        seh.dump(s);
        return s;
    }


    // Convenience output method for sourceEpochRinexHeader
    std::ostream& sourceEpochRinexHeader::dump(std::ostream& s) const
    {
        s << source << " " << epoch << " " << antennaType << " " << antennaPosition << " " << epochFlag;
        return s;
    } // sourceEpochRinexHeader::dump()


    // stream output for sourceEpochRinexHeader
    std::ostream& operator<<(std::ostream& s, const sourceEpochRinexHeader& serh)
    {
        serh.dump(s);
        return s;
    }


    // Convenience output method for sourceTypeHeader
    std::ostream& sourceTypeHeader::dump(std::ostream& s) const
    {
        s << source << " " << type;
        return s;
    } // sourceTypeHeader::dump()


    // stream output for sourceTypeHeader
    std::ostream& operator<<(std::ostream& s, const sourceTypeHeader& sth)
    {
        sth.dump(s);
        return s;
    }


    // Convenience output method for sourceSatHeader
    std::ostream& sourceSatHeader::dump(std::ostream& s) const
    {
        s << source << " " << satellite;
        return s;
    } // sourceSatHeader::dump()


    // stream output for sourceSatHeader
    std::ostream& operator<<(std::ostream& s, const sourceSatHeader& ssh)
    {
        ssh.dump(s);
        return s;
    }


    // Convenience output method for sourceEpochSatHeader
    std::ostream& sourceEpochSatHeader::dump(std::ostream& s) const
    {
        s << source << " " << epoch << " " << satellite;
        return s;
    } // sourceEpochSatHeader::dump()


    // stream output for sourceEpochSatHeader
    std::ostream& operator<<(std::ostream& s, const sourceEpochSatHeader& sesh)
    {
        sesh.dump(s);
        return s;
    }


    // Convenience output method for sourceEpochTypeHeader
    std::ostream& sourceEpochTypeHeader::dump(std::ostream& s) const
    {
        s << source << " " << epoch << " " << type;
        return s;
    } // sourceEpochTypeHeader::dump()


    // stream output for sourceEpochTypeHeader
    std::ostream& operator<<(std::ostream& s, const sourceEpochTypeHeader& seth)
    {
        seth.dump(s);
        return s;
    }



}
