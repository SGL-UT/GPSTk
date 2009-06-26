#ifndef GPSTK_HELMERT_TRANSFORM_TESTS
#define GPSTK_HELMERT_TRANSFORM_TESTS

#include <iostream>

#include "HelmertTransform.hpp"
#include "ReferenceFrame.hpp"
#include "Position.hpp"
#include "Exception.hpp"

bool testNoRotationNoScaleNoTranslation();
bool testArbitraryTranslation();
bool testScaling();
bool testSmallRotation();
bool testXAxisRotation();
bool testYAxisRotation();
bool testZAxisRotation();
bool testTranslationScaling();
bool testRotationScaling();
bool testRotationTranslation();
bool realData();
bool isEverythingDocumented() { return false; }

std::ostream& printBool(const bool print, std::ostream& out);
std::ostream& operator<<(std::ostream& out, gpstk::Position& pos);
std::ostream& operator<<(std::ostream& out, const bool right);
bool compare(gpstk::Position& left, gpstk::Position& right);

#endif
