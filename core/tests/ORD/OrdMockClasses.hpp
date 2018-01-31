
#ifndef LIB_ORD_MOCKS_HPP_
#define LIB_ORD_MOCKS_HPP_

#include "gmock/gmock.h"

#include <iostream>
#include <string>

#include "Exception.hpp"
#include "XvtStore.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "SatID.hpp"
#include "TimeSystem.hpp"
#include "ord.hpp"

using namespace gpstk;
using namespace std;

class MockXvtStore : public gpstk::XvtStore<SatID> {

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

class MockXvt : public gpstk::Xvt {

    public:
    MOCK_METHOD0(computeRelativityCorrection, double());
};

#endif /* LIB_ORD_MOCKS_HPP_ */
