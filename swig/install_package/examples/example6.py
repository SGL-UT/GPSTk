#!/usr/bin/env python
"""
This example reads an existing RINEX observation file, and writes it back out again.
The purpose of this sample is to access every component within the Rinex3ObsHeader
and Rinex3ObsData, and regenerate it from the python-primitive values.
"""
import os
import sys
import tempfile
import argparse  # an incredibly useful module for command line processing

import gpstk

def tweakInternalHeaderState(rinex_obs_header):
    '''
    There's a slight defect in the Rinex3ObsHeader object which causes it
    to only be in the correct state for full file writing if the header
    has been read from a file.

    This method is a hack to write the fully-built header (no data) to a file and
    read it back in.

    That way the header can be used to write another file which _does_ have data.
    '''
    temp_file = tempfile.NamedTemporaryFile().name
    gpstk.writeRinex3Obs(temp_file, rinex_obs_header, [])
    tweaked_header, _ = gpstk.readRinex3Obs(temp_file)
    os.remove(temp_file)
    return tweaked_header

def process(input_file, output_file):
    try:
        print 'Reading {}.'.format(input_file)
        header, data = gpstk.readRinex3Obs(input_file)  # read in everything
        #print header
        
        new_header = gpstk.Rinex3ObsHeader()
        
        # Initially, valid = 0L, but other values get masked into it.
        new_header.version = header.version
        new_header.fileType = header.fileType
        new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validVersion
        
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
        
        # This is kind of odd.  The Rinex3ObsHeader can handle both V3 and V2
        # formatted files.  The two versions handle observation types differently.
        # - The first structure "mapObsTypes" holds the V3 definitions in a 
        # "dictionary of lists" (or "map of vectors" in c++)
        # - The second structure "R2ObsTypes" holds the V2 definitions and is 
        # stored as list of strings (or Vector of Strings in c++)
        # It's not clear whether either or both are necessary to write a file.
        # It DOES seem to be clear that both will be available after reading a file.
        
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
        
        #####
        # Unfortunately, sigStrengthUnit is tied to the wavelengthFactor property,
        # We cannot set the latter due to a missing SWIG Python adapter, and 
        # because the same "validity flag" governs both, we can't output
        # sigStrengthUnit without inadvertently outputting a bogus wavelengthFactor.
        #####
        #new_header.wavelengthFactor = header.wavelengthFactor
        #new_header.sigStrengthUnit = header.sigStrengthUnit
        #new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validSigStrengthUnit
        
        for comment in header.commentList: 
            new_header.commentList.append(comment)
        new_header.valid = new_header.valid | gpstk.Rinex3ObsHeader.validComment
        
        new_header.validEoH = True
        
        # This method is a huge hack to get around a problem in the Rinex3ObsData
        # writer.  See the method for details.
        new_header = tweakInternalHeaderState(new_header)
        #print new_header
        
        processed_data = []
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
            processed_data.append(nd)

        gpstk.writeRinex3Obs(output_file, new_header, processed_data)
        print "Wrote output file: {}".format(output_file)

    # We can catch any custom gpstk exception like this:
    except gpstk.Exception as e:
        print e

def main():
    '''
    The main method.  This parses the command line arguments and invokes the
    main processing method.
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", dest="inputFile", help="input file name", metavar="inpath", required=True)
    parser.add_argument("-o", dest="outputFile", default="out.obs", help="output file name [default: %(default)s]", metavar="outpath")
    args = parser.parse_args()

    process(args.inputFile, args.outputFile)

    return 0

if __name__ == '__main__':
    sys.exit(main())
