#!/usr/bin/python
#==============================================================================
#
#  This file is part of GPSTk, the GPS Toolkit.
#
#  The GPSTk is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation; either version 3.0 of the License, or
#  any later version.
#
#  The GPSTk is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with GPSTk; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
#  
#  Copyright 2004-2019, The University of Texas at Austin
#
#==============================================================================

#==============================================================================
#
#  This software developed by Applied Research Laboratories at the University of
#  Texas at Austin, under contract to an agency or agencies within the U.S. 
#  Department of Defense. The U.S. Government retains all rights to use,
#  duplicate, distribute, disclose, or release this software. 
#
#  Pursuant to DoD Directive 523024 
#
#  DISTRIBUTION STATEMENT A: This software has been approved for public 
#                            release, distribution is unlimited.
#
#==============================================================================

# Test rstats

# ---------------------------------------------------------------------------
import argparse, re, sys, shlex, subprocess

# ---------------------------------------------------------------------------
def RunCmd(cmd1,cmd2,cmd3):
    '''given a command, echo it, run it and print results'''
    print("============= rstats {}".format(cmd3))

    cmd = cmd1+cmd2+cmd3
    #print("full cmd is {}".format(cmd))

    proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    #try:                                               # python 3
    #    outs,errs = proc.communicate(timeout=300)      # python 3
    #except TimeoutError:                               # python 3
    #    print("Failed - timeout")                      # python 3
    #    proc.kill()                                    # python 3
    #    outs,errs = proc.communicate()                 # python 3
    outs,errs = proc.communicate()                    # python 2

    outstr = outs.decode("utf-8").rstrip("\r\n")
    errstr = errs.decode("utf-8").rstrip("\r\n")

    if not errstr=="":
        print("Run error: "+errstr)
        sys.exit(1)

    return outstr

# ---------------------------------------------------------------------------
# build command line
cmdline = argparse.ArgumentParser(description="Test rstats")

# positional (required) args
cmdline.add_argument('--path',action='store',metavar='path',default='.',
                     help='path of testfiles SDexam01.txt and testfft.data (.)')
cmdline.add_argument('--pathexe',action='store',metavar='path',default='.',
                     help='path of rstats executable (.)')

# parse cmd line --------------------------------
args = cmdline.parse_args()
path = args.path
pathexe = args.pathexe

cmdexe = pathexe+"/rstats "
cmdpath = path+"/"
print("Command exe with data path is "+cmdexe+cmdpath)

# -------------------------------------------------------------------------------------------
outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bc -br -b2 -bw")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 -bc --beg 66600 --end 69000")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 -br --min 10. --max 40.")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --KS")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bw")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 --brief")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -brw")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -br")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bw")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bc")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -b2")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --outs 0.5")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --plot")
outstr = outstr.replace(cmdpath,"")   # remove input file path
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --qplot --stdout")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --bin 30")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --sum")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --sum+")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --fit 5,65250,67700 --stdout")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --seq --label SEQS")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 4 -x 1 -y 2 --disc 0.8,0")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 5 -x 1 -y 2 --fdf 0.8")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 5 -x 1 -y 2 --fdf2 0.8,0.02,2")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 5 -x 1 -y 2 --wind 10")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 5 -x 1 -y 2 --xwind 10")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 5 -x 1 -y 2 --fix 4")
print(outstr)

outstr=RunCmd(cmdexe,cmdpath,"SDexam01.txt -q -p 5 -x 1 -y 2 --wnj 1.e-4")
# remove last 2 columns in KMU output (residuals - subject to numerical noise)
lines = outstr.split("\n")
for line in lines:
    line = line.rstrip("\r\n")
    if re.match(r'^KMU',line):
        F = line.split()
        #print("KMU",end="")                         # python 3
        print("KMU"),                              # python 2
        for i in range(1,10):
            #print(" "+F[i],end="")                  # python 3
            print(F[i]),                           # python 2
        print("")
    else:
        print(line)

outstr=RunCmd(cmdexe,cmdpath,"testfft.data -q -x 1 -y 9 -p 8 --fft 0.0034722")
print(outstr)

# -------------------------------------------------------------------------------------------
