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
    MOCK_CONST_METHOD2(dump, void(std::ostream& s, short detail));  // NOLINT(runtime/int)

    MOCK_METHOD2(edit, void(const CommonTime& tmin, const CommonTime& tmax));
};

class MockXvt: public gpstk::Xvt {
 public:
    MOCK_METHOD0(computeRelativityCorrection, double());
};

class MockTropo: public gpstk::TropModel {
 public:
    // It turns out that you can't mock a method with a throw() specifier.
    // Google test doesn't support it.
    // https://stackoverflow.com/questions/4922595/mocking-a-method-with-throw-specifier
    // The workaround is to create a wrapper method that doesn't throw.
    virtual double dry_zenith_delay() const
        throw(gpstk::InvalidTropModel) {
        return dry_zenith_delay_wrap();
    }
    MOCK_CONST_METHOD0(dry_zenith_delay_wrap, double());

    virtual double wet_zenith_delay() const
        throw(gpstk::InvalidTropModel) {
        return wet_zenith_delay_wrap();
    }
    MOCK_CONST_METHOD0(wet_zenith_delay_wrap, double());

    MOCK_CONST_METHOD1(dry_mapping_function_wrap, double(double elevation));
    virtual double dry_mapping_function(double elevation) const
        throw(gpstk::InvalidTropModel) {
        return dry_mapping_function_wrap(elevation);
    }

    MOCK_CONST_METHOD1(wet_mapping_function_wrap, double(double elevation));
    virtual double wet_mapping_function(double elevation) const
        throw(gpstk::InvalidTropModel) {
        return wet_mapping_function_wrap(elevation);
    }

    MOCK_CONST_METHOD1(correction_wrap, double(double elevation));
    virtual double correction(double elevation) const
        throw(gpstk::InvalidTropModel) {
        return correction_wrap(elevation);
    }
};

class MockIono: public gpstk::IonoModelStore {
 public:
    MOCK_CONST_METHOD5(getCorrection_wrap,
            double(const CommonTime& time,
                   const Position& rxgeo,
                   double svel,
                   double svaz,
                   gpstk::IonoModel::Frequency freq));

    virtual double getCorrection(
            const CommonTime& time,
            const Position& rxgeo,
            double svel,
            double svaz,
            gpstk::IonoModel::Frequency freq) const
        throw(gpstk::IonoModelStore::NoIonoModelFound) {
        return getCorrection_wrap(time, rxgeo, svel, svaz, freq);
    }
};

#endif  // CORE_TESTS_ORD_ORDMOCKCLASSES_HPP_
