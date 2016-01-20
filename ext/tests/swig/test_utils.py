#!/usr/env python

import os
import sys
from inspect import currentframe,getframeinfo

if len(sys.argv) < 2:
    print "Useage: script <path-to-gpstk-module> [<data-dir>] [<output-dir>]"
    sys.exit(-1)

print "Using gpstk bindings at {}".format(sys.argv[1])
sys.path=[sys.argv[1]]+sys.path

if len(sys.argv)>2: data_dir=sys.argv[2]
if len(sys.argv)>3: output_dir=sys.argv[3]

error_count=0
test_count=0
class_name=""
method_name=""
file_name=os.path.basename(getframeinfo(currentframe(1)).filename)

def test_base():
    pfi = getframeinfo(currentframe(2))
    global test_count, file_name, class_name, method_name
    test_count += 1
    print "GPSTkPythonTest, Class={}, Method={}, testFile={}, testLine={}, subtest={},".format(
    class_name, method_name, file_name, pfi.lineno, test_count),

def assert_equal(t, v, eps=0, msg=""):
    global error_count
    test_base()
    err = abs(t-v)
    if err > eps:
        error_count += 1
        print "failbit=1, {} - {} = {} > {}".format(t, v, err, eps)
    else:
        print "failbit=0"

def fail_test():
    global error_count
    test_base()
    error_count += 1
    print "failbit=1"

def pass_test():
    test_base()
    print "failbit=0"

def finish():
    print "Total failures for {}: {}".format(
        getframeinfo(currentframe(1)).filename,error_count)
    sys.exit(error_count)
