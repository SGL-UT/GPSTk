#!/usr/env python

import sys

if len(sys.argv) < 2:
    print "Useage: script <path-to-gpstk-module> [<data-dir>] [<output-dir>]"
    sys.exit(-1)

sys.path=[sys.argv[1]]+sys.path
sys.argv[1:] = sys.argv[2:]
print "Using gpstk bindings at {}".format(sys.argv[1])

if len(sys.argv)>1:
    data_dir=sys.argv[1];
    sys.argv[1:] = sys.argv[2:]

if len(sys.argv)>1:
    output_dir=sys.argv[1]
    sys.argv[1:] = sys.argv[2:]
