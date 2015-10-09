#!/usr/bin/env python

import doxy2swig  # local file
import glob
import os
import sys
import subprocess
import argparse


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


def generate_docs(args):
    # This present script should live in '$gpstk_root/swig',
    # so to get the $gpstk_root, just strip off the file name
    # and use split() to pop off the 'swig' directory from the
    # remaining file path.
    script_fullpath_name = os.path.realpath(__file__)
    path_gpstk_swig = os.path.dirname( script_fullpath_name )
    (gpstk_root, swig_dir) = os.path.split( path_gpstk_swig )

    # Build a list of all the XML files that dOxygen output previously
    xml_glob_pattern = os.path.sep.join((args.src_dir, 'xml', '*.xml'))
    xml_files = glob.glob( xml_glob_pattern )
    num_files = len( xml_files )
    if num_files == 0:
        print 'WARNING: No doxygen-xml files found, docstrings cannot be generated.'
        return

    # create directories for swig doc files
    path_gpstk_swig_doc = args.dst_dir
    if not os.path.exists( path_gpstk_swig_doc ):
        os.makedirs( path_gpstk_swig_doc )

    # for each doxygen xml file, create a converted swig .i file for associated docstrings
    for f_xml in xml_files:
        (f, xml_ext) = os.path.splitext( f_xml ) # remove the .xml ending
        name_root = file_name(f)
        output_file = path_gpstk_swig_doc + os.path.sep + name_root + '.i'
        if ('index' not in name_root):  # don't try to use index.xml
            try:
                doxy2swig.convert(f_xml, output_file, False, False)
                clean_errors(output_file)
                print 'Parsed ', f, 'to', output_file
            except Exception as e:
                print 'ERROR:', f_xml, 'can not be parsed'
                print '\t', e

    # Add the includes for each converted xml file.i to the doc.i file
    out_file = open(path_gpstk_swig_doc + os.path.sep + 'doc.i', 'w')
    out_file.write('# This is an AUTO-GENERATED file by doc.py.\n')
    out_file.write('# Do not modify it unless you know what you are doing.\n')
    doc_files = glob.glob(path_gpstk_swig_doc + os.path.sep + '*.i')
    for f_i in doc_files:
        if 'doc.i' not in f_i:
            out_file.write('%include ' + f_i + '\n')
    out_file.close()
    print '\nFinished with documentation.'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="""
    GPSTk python-swig binding documentation generator.
    This reads every file in the $doc_dir/xml (where doxygen places its xml)
    Then uses $gpstk_root/swig/doxy2swig.py to create docstring output for a SWIG .i file.
    These .i files are placed in $gpstk_root/swig/doc.
    Then doc.i is auto-generated to include all of these new files in the doc/ folder.
    """)

    parser.add_argument("-d", default=0, dest="debug", action="count",
        help="Increase the level of debug output.")

    parser.add_argument(dest="src_dir",
        help="Directory where the doxygen xml files are.")

    parser.add_argument(dest="dst_dir",
        help="Directory to write swig.i files to.")

    args = parser.parse_args()

    generate_docs(args)
