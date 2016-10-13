#!/usr/bin/env python
"""
An example of generating new rinex file (both v2,11 and v3.02.
First an existing v2.11 file is read in just so we don't have
to make up all the data that goes into the new files.
"""

import gpstk

ifn = gpstk.getPathData() + '/test_input_rinex2_obs_RinexObsFile.06o'
header, data = gpstk.readRinex3Obs(ifn, strict=True)

new_header = gpstk.Rinex3ObsHeader()

new_header.fileType = header.fileType

new_header.fileProgram = header.fileProgram
new_header.date = header.date
new_header.fileAgency = header.fileAgency
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validRunBy

new_header.markerName = header.markerName
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validMarkerName

new_header.observer = header.observer
new_header.agency = header.agency
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validObserver

new_header.recNo = header.recNo
new_header.recType = header.recType
new_header.recVers = header.recVers
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validReceiver

new_header.antNo = header.antNo
new_header.antType = header.antType
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validAntennaType 

newAntPosition = gpstk.Triple(header.antennaPosition[0],header.antennaPosition[1],header.antennaPosition[2])
new_header.antennaPosition = newAntPosition
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validAntennaPosition

newAntDelta = gpstk.Triple(header.antennaDeltaHEN[0],header.antennaDeltaHEN[1],header.antennaDeltaHEN[2])
new_header.antennaDeltaHEN = newAntDelta
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validAntennaDeltaHEN

for rinObsType in header.mapObsTypes:
    #print "Found obs-type:{}".format(rinObsType)
    newObsIds = []
    for rinObsId in header.mapObsTypes[rinObsType]:
        #print "Found obs-id:{}".format(str(rinObsId))
        #print " --type={} code={} band={}".format(rinObsId.type, rinObsId.code, rinObsId.band)
        newObsId = gpstk.RinexObsID(str(rinObsId))
        newObsIds.append(newObsId)
    new_header.mapObsTypes[rinObsType] = tuple(newObsIds) 
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validNumObs

for rin_obs_id in header.R2ObsTypes:
    #print "Found obs-id:{}".format(rin_obs_id)
    new_header.R2ObsTypes.append(rin_obs_id)
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validNumObs

# This creates a new CivilTime object that we can populate
new_header.firstObs = gpstk.CivilTime()
new_header.firstObs.year = header.firstObs.year
new_header.firstObs.month = header.firstObs.month
new_header.firstObs.day = header.firstObs.day
new_header.firstObs.hour = header.firstObs.hour
new_header.firstObs.minute = header.firstObs.minute
new_header.firstObs.second = header.firstObs.second
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validFirstTime

new_header.markerNumber = header.markerNumber
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validMarkerNumber

new_header.interval = header.interval
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validInterval


for comment in header.commentList: 
    new_header.commentList.append(comment)
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validComment       
new_header.validEoH = True

# As of Sep 2016, the following aren't working correctly so force
# these to be set. Since the input to this program is RINEX v2.11,
# what should happen is that the Rinex3ObsHeader should fill in default values
# for the SystemPhaseShift and not require the two glonass since there is no
# glonass data in the source
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validSystemPhaseShift
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validGlonassSlotFreqNo
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validGlonassCodPhsBias

print new_header

new_data = []
# Now we loop through all the epochs and process the data for each one
for d in data:

    # This creates a new CommonTime object with the system set to GPS.
    timec = gpstk.CommonTime(gpstk.TimeSystem(gpstk.TimeSystem.GPS))
    # This creates a new CommonTime object with the system set to GPS.
    mjd = int(d.time.getDays())
    sod = float(d.time.getSecondOfDay())
    timec.set(mjd, sod, gpstk.TimeSystem(gpstk.TimeSystem.GPS))

    # Assign values to a new Rinex Obs Data object
    nd = gpstk.Rinex3ObsData()
    nd.time = timec
    nd.auxHeader = d.auxHeader
    nd.clockOffset = d.clockOffset
    nd.epochFlag= d.epochFlag
    nd.numSVs = d.numSVs

    for satkey in d.obs.keys():
        newSatKey = gpstk.RinexSatID(satkey.toString())
        satObss = d.obs[newSatKey]
        # satObss is a tuple of  RinexDatum
        newSatObss = []
        for satObs in satObss:
            #print "{} {} {} {}".format(satkey.toString(), satObs.data, satObs.lli, satObs.ssi)
            newSatObs = gpstk.RinexDatum()
            newSatObs.data = satObs.data
            newSatObs.lli = satObs.lli
            newSatObs.ssi = satObs.ssi
            newSatObss.append(newSatObs)

        nd.obs[newSatKey] = tuple(newSatObss)

    #print "O{}".format(d)
    #print "S{}".format(nd)
    new_data.append(nd)

new_header.version = 3.02
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validVersion
ofn = "example6_v302.o"
gpstk.writeRinex3Obs(ofn, new_header, new_data)
print "Wrote output file: {}".format(ofn)

# The following doesn't work because the v2 translation map needs to be filled in from
# the v3 translation tables
new_header.version = 2.11
new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validVersion
ofn = "example6_v211.o"
gpstk.writeRinex3Obs(ofn, new_header, new_data)
print "Wrote output file: {}".format(ofn)
