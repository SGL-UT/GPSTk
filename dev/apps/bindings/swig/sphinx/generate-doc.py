#!/usr/bin/env python

import gpstk
import inspect
import pydoc
import os


def is_function(x):
    t = eval(x)
    return hasattr(t, '__call__')


def is_class(x):
    t = eval(x)
    return inspect.isclass(t)


def get_doc(x):
    return pydoc.render_doc(str, "Help on %s")


def should_doc(x):
    return '._' not in x and '__' not in x


def get_dir(x):
    list = dir(x)
    return filter(lambda y: should_doc(y), list)


def main():
    # Quick ref:
    classes = []
    functions = []
    constants = []
    exceptions = []
    cpp = []
    namespace = ['gpstk.' + x for x in dir(gpstk)]

    for x in namespace:
        if 'gpstk._' in x:
            pass
        elif x == 'gpstk.constants':
            for y in get_dir(gpstk.constants):
                constants.append('gpstk.constants.' + y)
        elif x == 'gpstk.exceptions':
            for y in get_dir(gpstk.exceptions):
                exceptions.append('gpstk.exceptions.' + y)
        elif x == 'gpstk.cpp':
            for y in get_dir(gpstk.cpp):
                cpp.append('gpstk.cpp.' + y)
        elif is_class(x):
            classes.append(x)
        elif is_function(x):
            functions.append(x)

    f = open('quickref.rst', 'w')
    f.write('Quick Reference\n====================\n\n')
    desc = ("Here we provide a list of all members avaliable in the gpstk namespace."
            "This is not meant to be complete documentation. Refer to the C++ API and "
            "the additional changes pages for more information. \n"
            "You should also try to use the help(x) function, where x is the object "
            "that you are interested in.")
    f.write(desc + '\n\n')
    f.write(".. py:currentmodule:: gpstk\n\n")

    f.write("\nClasses\n****************\n\n")
    for x in classes:
        f.write('.. py:class:: ' + x + '\n')

    f.write("\nFunctions\n****************\n\n")
    for x in functions:
        f.write('.. py:function:: ' + x + '\n')

    f.write("\nConstants\n****************\n\n")
    for x in constants:
        f.write('.. py:data:: ' + x + '\n')

    f.write("\nExceptions\n****************\n\n")
    for x in exceptions:
        f.write('.. py:class:: ' + x + '\n')

    f.write("\nC++ Standard library wrappers\n*********************************\n\n")
    for x in cpp:
        f.write('.. py:class:: ' + x + '\n')
    f.close()

if __name__ == '__main__':
    main()