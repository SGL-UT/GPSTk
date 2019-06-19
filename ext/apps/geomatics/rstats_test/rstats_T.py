#!/usr/bin/python3

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
    try:
        outs,errs = proc.communicate(timeout=300)
    except TimeoutError:
        printf("Failed - timeout")
        proc.kill()
        outs,errs = proc.communicate()

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
lines = outstr.split("\n")
for line in lines:
    line = line.rstrip("\r\n")
    if re.match(r'^KMU',line):
        F = line.split()
        for i in range(0,10):
            print(F[i],end="")
        print("")
    else:
        print(line)
#print(outstr)
outstr=RunCmd(cmdexe,cmdpath,"testfft.data -q -x 1 -y 9 -p 8 --fft 0.0034722")
print(outstr)

# -------------------------------------------------------------------------------------------
