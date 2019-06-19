#!/usr/bin/python3

# Generate all baseline cfg files for input to procnet
#    using genbasecfg.py, and input

# ---------------------------------------------------------------------------
# TODO

# ---------------------------------------------------------------------------
import argparse, re, sys, shlex, subprocess

# ---------------------------------------------------------------------------
def RunCmd(cmd):
    '''given a command, echo it, run it and print results'''
    print("============={}".format(cmd))

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

    print(outstr)
    return

# ---------------------------------------------------------------------------
# build command line
cmdline = argparse.ArgumentParser(description="Test rstats")

# positional (required) args
cmdline.add_argument('--path',action='store',metavar='path',default='.',
                     help='path of testfiles SDexam01.txt and testfft.data (.)')

# parse cmd line --------------------------------
args = cmdline.parse_args()
path = args.path

# -------------------------------------------------------------------------------------------
RunCmd("pwd")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bc -br -b2 -bw")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 -bc --beg 66600 --end 69000")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 -br --min 10. --max 40.")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --KS")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bw")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 --brief")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -brw")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -br")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bw")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bc")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -b2")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --outs 0.5")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --plot")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --qplot --stdout")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --bin 30")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --sum")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --sum+")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --fit 5,65250,67700 --stdout")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --seq --label SEQS")
RunCmd("rstats SDexam01.txt -q -p 4 -x 1 -y 2 --disc 0.8,0")
RunCmd("rstats SDexam01.txt -q -p 5 -x 1 -y 2 --fdf 0.8")
RunCmd("rstats SDexam01.txt -q -p 5 -x 1 -y 2 --fdf2 0.8,0.02,2")
RunCmd("rstats SDexam01.txt -q -p 5 -x 1 -y 2 --wind 10")
RunCmd("rstats SDexam01.txt -q -p 5 -x 1 -y 2 --xwind 10")
RunCmd("rstats SDexam01.txt -q -p 5 -x 1 -y 2 --fix 4")
RunCmd("rstats SDexam01.txt -q -p 5 -x 1 -y 2 --wnj 1.e-4")
RunCmd("rstats testfft.data -q -x 1 -y 9 -p 8 --fft 0.0034722")

# -------------------------------------------------------------------------------------------
