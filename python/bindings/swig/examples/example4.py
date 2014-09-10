#!/usr/bin/env python

"""
A GPSTk example featuring the use of RINEX processing and using the PRSolution2 class.

This is a port of the older C++ example4.cpp.

Usage:
  python example4.py <RINEX3 Obs file> <RINEX3 Nav file> [-m <RINEX Met file>]'

Example:
  python example4.py rinex3obs_data.txt rinex3nav_data.txt -m rinexmet_data.txt

"""

import argparse
import gpstk
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('rinex3obs_filename')
    parser.add_argument('rinex3nav_filename')
    parser.add_argument('-m', '--rinexmet_filename')
    args = parser.parse_args()

    # Declaration of objects for storing ephemerides and handling RAIM
    bcestore = gpstk.GPSEphemerisStore()
    raimSolver = gpstk.PRSolution2()

    # Object for void-type tropospheric model (in case no meteorological
    # RINEX is available)
    noTropModel = gpstk.ZeroTropModel()

    # Object for GG-type tropospheric model (Goad and Goodman, 1974)
    #  Default constructor => default values for model
    ggTropModel = gpstk.GGTropModel()

    # Pointer to one of the two available tropospheric models. It points
    # to the void model by default
    tropModel = noTropModel


    navHeader, navData = gpstk.readRinex3Nav(args.rinex3nav_filename)
    for navDataObj in navData:
        ephem = navDataObj.toEngEphemeris()
        bcestore.addEphemeris(ephem)

    # Setting the criteria for looking up ephemeris:
    bcestore.SearchNear()

    if args.rinexmet_filename is not None:
        metHeader, metData = gpstk.readRinexMet(args.rinexmet_filename)
        tropModel = ggTropModel

    obsHeader, obsData = gpstk.readRinex3Obs(args.rinex3obs_filename)

    # The following lines fetch the corresponding indexes for some
    # observation types we are interested in. Given that old-style
    # observation types are used, GPS is assumed.
    try:
        indexP1 = obsHeader.getObsIndex('C1P')
    except:
        print 'The observation files has no L1 C/A pseudoranges.'
        sys.exit()

    try:
        indexP2 = obsHeader.getObsIndex('C2W')
    except:
        print 'The observation files has no L2 codeless pseudoranges.'
        indexP2 = -1

    for obsObj in obsData:
        # Find a weather point. Only if a meteorological RINEX file
        # was provided, the meteorological data linked list "rml" is
        # neither empty or at its end, and the time of meteorological
        # records are below observation data epoch.
        if args.rinexmet_filename is not None:
            for metObj in metData:
                if metObj.time >= obsObj.time:
                    break
                else:
                    metDataDict = metObj.getData()
                    temp = metDataDict[gpstk.RinexMetHeader.TD]
                    pressure = metDataDict[gpstk.RinexMetHeader.PR]
                    humidity = metDataDict[gpstk.RinexMetHeader.HR]
                    ggTropModel.setWeather(temp, pressure, humidity)

        if obsObj.epochFlag == 0 or obsObj.epochFlag == 1:
            # Note that we use lists here, but we will need types backed
            # by C++ std::vectors later. We'll just keep it easy and use
            # gpstk.seqToVector to convert them. If there was a speed
            # bottleneck we could use gpstk.cpp.vector_SatID and
            # gpstk.cpp.vector_double though.
            prnList = []
            rangeList = []

            # This part gets the PRN numbers and ionosphere-corrected
            # pseudoranges for the current epoch. They are correspondly fed
            # into "prnList" and "rangeList"; "obs" is a public attribute of
            # Rinex3ObsData to get the map of observations
            for satID, datumList in obsObj.obs.iteritems():
                # The RINEX file may have P1 observations, but the current
                 # satellite may not have them.
                P1 = 0.0
                try:
                    P1 = obsObj.getObs(satID, indexP1).data
                except gpstk.exceptions.Exception:
                    continue  # Ignore this satellite if P1 is not found

                ionocorr = 0.0

                  # If there are P2 observations, let's try to apply the
                  # ionospheric corrections
                if indexP2 >= 0:
                    # The RINEX file may have P2 observations, but the
                    # current satellite may not have them.
                    P2 = 0.0
                    try:
                        P2 = obsObj.getObs(satID, indexP2).data
                    except gpstk.exceptions.Exception:
                        continue  # Ignore this satellite if P1 is not found
                    # list 'vecList' contains RinexDatum, whose public
                    # attribute "data" indeed holds the actual data point
                    ionocorr = 1.0 / (1.0 - gpstk.GAMMA_GPS) * ( P1 - P2 )

                # Now, we include the current PRN number in the first part
                # of "it" iterator into the list holding the satellites.
                # All satellites in view at this epoch that have P1 or P1+P2
                # observations will be included.
                prnList.append(satID)

                # The same is done for the list of doubles holding the
                # corrected ranges
                rangeList.append(P1 - ionocorr)
                # WARNING: Please note that so far no further correction
                # is done on data: Relativistic effects, tropospheric
                # correction, instrumental delays, etc

            # The default constructor for PRSolution2 objects (like
            # "raimSolver") is to set a RMSLimit of 6.5. We change that
            # here. With this value of 3e6 the solution will have a lot
            # more dispersion.
            raimSolver.RMSLimit = 3e6


            # In order to compute positions we need the current time, the
            # vector of visible satellites, the vector of corresponding
            # ranges, the object containing satellite ephemerides, and a
            # pointer to the tropospheric model to be applied

            time = obsObj.time

            # the RAIMComputer method of PRSolution2 accepts a vector<SatID> as its
            # 2nd argument, but the list is of RinexSatID, which is a subclass of SatID.
            # Since C++ containers are NOT covariant, it is neccessary to change the
            # output to a vector or SatID's rather thta a vector of RinexSatID's.
            satVector = gpstk.cpp.seqToVector(prnList, outtype='vector_SatID')
            rangeVector = gpstk.cpp.seqToVector(rangeList)
            raimSolver.RAIMCompute(time, satVector, rangeVector, bcestore, tropModel)

            # Note: Given that the default constructor sets public
            # attribute "Algebraic" to FALSE, a linearized least squares
            # algorithm will be used to get the solutions.
            # Also, the default constructor sets ResidualCriterion to true,
            # so the rejection criterion is based on RMS residual of fit,
            # instead of RMS distance from an a priori position.


            if raimSolver.isValid():
                # Vector "Solution" holds the coordinates, expressed in
                # meters in an Earth Centered, Earth Fixed (ECEF) reference
                # frame. The order is x, y, z  (as all ECEF objects)
                x, y, z = raimSolver.Solution[0],  raimSolver.Solution[1], raimSolver.Solution[2]
                print "%12.5f %12.5f %12.5f" % (x, y, z)


if __name__ == '__main__':
    main()
