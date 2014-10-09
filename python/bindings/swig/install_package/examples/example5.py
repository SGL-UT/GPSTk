#!/usr/bin/env python

"""
A GPSTk example showing how to walk RINEX data sets and use the ObsEpoch classes

Usage:
  python example5.py <RINEX2 Obs file>
  
Example:
  python ./example5.py rinex2obs_data.txt

"""

import argparse
import gpstk

parser = argparse.ArgumentParser()
parser.add_argument('rinex2obs_filename')
args = parser.parse_args()

try:
    print 'Reading ' + args.rinex2obs_filename + '.'

    # read in header and get a generator for the data
    header, data = gpstk.readRinex3Obs(args.rinex2obs_filename)
    print header

    for d in data:
        civtime = gpstk.CivilTime(d.time)
        print civtime
        oe=gpstk.ObsEpoch()
        oe.time = d.time
        for sv in d.obs.keys():
            print sv,
            epoch = d.obs[sv]
            soe = gpstk.SvObsEpoch()
            soe.svid = sv
            for i in range(len(epoch)):
                rinex2_obs_type = header.R2ObsTypes[i]
                oid = header.mapObsTypes['G'][i]
                print "{}({})={}".format(str(oid), rinex2_obs_type, epoch[i].data),
                soe[oid] = epoch[i].data
            oe[sv] = soe
            print
        print oe
        print

# We can catch any custom gpstk exception like this:
except gpstk.exceptions.Exception as e:
    print e
