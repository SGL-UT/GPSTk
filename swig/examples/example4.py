#!/usr/bin/env python

"""
An example reading a RINEX obs, nav and met file and using
PRSolution2 to computer a receiver position and compare
this to the position in the obs header.
"""
import gpstk
import sys

obsfn = gpstk.getPathData() + '/arlm200z.15o'
navfn = gpstk.getPathData() + '/arlm200z.15n'
metfn = gpstk.getPathData() + '/arlm200z.15m'

navHeader, navData = gpstk.readRinex3Nav(navfn)
ephStore = gpstk.gpstk.Rinex3EphemerisStore()
for navDataObj in navData:
    ephStore.addEphemeris(navDataObj)

tropModel = gpstk.GGTropModel()
metHeader, metData = gpstk.readRinexMet(metfn)

obsHeader, obsData = gpstk.readRinex3Obs(obsfn)

indexP1 = obsHeader.getObsIndex('C1W')
indexP2 = obsHeader.getObsIndex('C2W')
raimSolver = gpstk.PRSolution2()

for obsObj in obsData:
    for metObj in metData:
        if metObj.time >= obsObj.time:
            break
        else:
            metDataDict = metObj.getData()
            temp = metDataDict[gpstk.RinexMetHeader.TD]
            pressure = metDataDict[gpstk.RinexMetHeader.PR]
            humidity = metDataDict[gpstk.RinexMetHeader.HR]
            tropModel.setWeather(temp, pressure, humidity)

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
        satVector = gpstk.seqToVector(prnList, outtype='vector_SatID')
        rangeVector = gpstk.seqToVector(rangeList)
        raimSolver.RAIMCompute(time, satVector, rangeVector, ephStore, tropModel)

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
            pos = gpstk.Triple(raimSolver.Solution[0],  raimSolver.Solution[1], raimSolver.Solution[2])
            err = obsHeader.antennaPosition - pos
            print err
