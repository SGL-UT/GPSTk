//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  Copyright 2004, The University of Texas at Austin
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the University of
// Texas at Austin, under contract to an agency or agencies within the U.S.
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

#ifndef CORE_TESTS_ORD_ORDMOCKCLASSES_HPP_
#define CORE_TESTS_ORD_ORDMOCKCLASSES_HPP_

#include <iostream>
#include <string>

#include "gmock/gmock.h"

#include "Exception.hpp"
#include "XvtStore.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "SatID.hpp"
#include "TimeSystem.hpp"
#include "ord.hpp"

using gpstk::SatID;
using gpstk::CommonTime;
using gpstk::TimeSystem;
using gpstk::Xvt;
using gpstk::Position;

class MockXvtStore: public gpstk::XvtStore<SatID> {
 public:
    MOCK_METHOD0(clear, void());

    MOCK_CONST_METHOD0(hasVelocity, bool());
    MOCK_CONST_METHOD0(getTimeSystem, TimeSystem());
    MOCK_CONST_METHOD0(getInitialTime, CommonTime());
    MOCK_CONST_METHOD0(getFinalTime, CommonTime());

    MOCK_CONST_METHOD1(isPresent, bool(const SatID& id));

    MOCK_CONST_METHOD2(getXvt, Xvt(const SatID& id, const CommonTime& t));
    MOCK_CONST_METHOD2(dump, void(std::ostream& s, short detail));

    MOCK_METHOD2(edit, void(const CommonTime& tmin, const CommonTime& tmax));
};

class MockXvt: public gpstk::Xvt {
 public:
    MOCK_METHOD0(computeRelativityCorrection, double());
};

#endif  // CORE_TESTS_ORD_ORDMOCKCLASSES_HPP_
