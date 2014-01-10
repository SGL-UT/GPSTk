#!/usr/bin/env python


"""GPSTk python-swig binding documentation generator.

This reads every file in the gpstk/dev/doc/xml (where doxygen places its xml)
and uses doxy2swig.py to create docstring output for a SWIG .i file. These
files are placed in gpstk/dev/apps/bindings/swig/doc. doc.i is auto-generated
to include all of these new files in the doc/ folder.

Usage:
  python doc.py
"""


import doxy2swig  # local file
import glob
import os
import sys
import subprocess


def file_name(path):
    """Returns the file name part of a path/file string."""
    head, tail = os.path.split(path)
    return tail


def clean_errors(file):
    """Removes any instance of ::throw in a file that doxy2swig produces."""
    # doxy2swig.py has a minor bug where it can make documentation
    # slightly incorrect. For example, if Cat is a class which has a
    # constructor that can throw an exception; (i.e. Cat::Cat() throws())
    # doxy2swig may create documentation for Cat::Cat::throws, which does
    # not exist. Therefore, they are simply removed to prevent SWIG
    # from crashing.

    # read in
    f = open(file, 'r')
    data = f.read()
    f.close()

    # remove all ::throw
    data = data.replace('::throw', '')

    # write out
    f = open(file, 'w')
    f.write(data)
    f.close()


def generate_docs():
    # yes, it's a magic number below.
    # gpstk_folder should be the path ending with 'gpstk/'
    # the number is the number of chars to cut off from this file's path
    gpstk_folder = os.path.realpath(__file__)[:-45]
    xml_files = glob.glob(gpstk_folder + 'dev/doc/xml/*.xml')
    num_files = len(xml_files)
    if num_files == 0:
        print 'WARNING: No doxygen-xml files found, docstrings cannot be generated.'

    gpstk_swig = ''
    gpstk_py_doc = gpstk_swig + 'doc/'
    if not os.path.exists(gpstk_py_doc):
        os.makedirs(gpstk_py_doc)

    for f_xml in xml_files:
        f = f_xml[:-4]  # remove the .xml ending
        name = file_name(f)
        output_file = gpstk_py_doc + name + '.i'
        if ('index' not in name):  # don't try to use index.xml
            try:
                doxy2swig.convert(f_xml, output_file, False, False)
                clean_errors(output_file)
                print 'Parsed ', f, 'to', output_file
            except Exception as e:
                print 'ERROR:', f_xml, 'can not be parsed'
                print '\t', e

    out_file = open(gpstk_py_doc + 'doc.i', 'w')
    out_file.write('# This is an AUTO-GENERATED file by doc.py.\n')
    out_file.write('# Do not modify it unless you know what you are doing.\n')
    doc_files = glob.glob(gpstk_py_doc + '*.i')
    for f_i in doc_files:
        if 'doc.i' not in f_i:
            out_file.write('%include ' + f_i + '\n')
    out_file.close()
    print '\nFinished with documentation.'


if __name__ == '__main__':
    generate_docs()
