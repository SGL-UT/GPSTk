#!/usr/bin/env python

import gpstk
import inspect
import os


def is_function(x):
    t = eval(x)
    return hasattr(t, '__call__')


def is_class(x):
    t = eval(x)
    return inspect.isclass(t)


def should_doc(x):
    return '._' not in x and '__' not in x


def get_dir(x):
    list = dir(x)
    return filter(lambda y: should_doc(y), list)


def get_constructor_doc(x):
        try:
            lines = eval(x).__init__.__doc__.splitlines()
            output = ""
            textRead = False
            for l in lines:
                output = output + '   | ' + l + '\n'
                if len(l.strip()) == 0 and textRead:
                    break
                else:
                    textRead = True
            output = '   | Constructors: \n' + output
            return output
        except AttributeError:
            return ""


def write_page(x):
    def write_class(x):
        class_file = open(x + '.rst', 'w')
        class_file.write(x + '\n=========================================================\n\n')
        class_file.write(get_constructor_doc(x))
        class_file.write('\n\n.. autoclass:: ' + x + '\n')
        class_file.write('   :members:\n')
        class_file.write('   :inherited-members:\n')
        class_file.write('   :undoc-members:\n')
        class_file.close()
    def write_function(x):
        function_file = open(x + '.rst', 'w')
        function_file.write(x + '\n=========================================================\n\n')
        function_file.write('.. autofunction:: ' + x + '\n')
        function_file.close()
    if is_function(x):
        write_function(x)
    if is_class(x):
        write_class(x)


def main():
    os.system('rm -f gpstk.*.rst')
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
    f.write('.. _quickref_label:\n\n')
    f.write('Quick Reference\n====================\n\n')
    desc = ("Here we provide a list of all members avaliable in the gpstk namespace."
            "This is not meant to be complete documentation. Refer to the C++ API and "
            "the additional changes pages for more information. \n"
            "You should also try to use the help(x) function, where x is the object "
            "that you are interested in.\n\n"
            "This is completely AUTO-GENERATED documentation. All text on the functions and classes "
            "is originally derived from the C++ source files.")
    f.write(desc + '\n\n')
    f.write('.. py:currentmodule:: gpstk\n\n')

    f.write('\nClasses\n****************\n\n')
    f.write('.. toctree::\n   :maxdepth: 1\n\n')
    for x in classes:
        write_page(x)
        f.write('   ' + x + '\n')

    f.write('\nFunctions\n****************\n\n')
    f.write('.. toctree::\n   :maxdepth: 1\n\n')
    for x in functions:
        write_page(x)
        f.write('   ' + x + '\n')

    f.write('\nExceptions\n****************\n\n')
    f.write('.. toctree::\n   :maxdepth: 1\n\n')
    for x in exceptions:
        write_page(x)
        f.write('   ' + x + '\n')

    f.write('\nC++ Standard library wrappers\n*********************************\n\n')
    f.write('.. toctree::\n   :maxdepth: 1\n\n')
    for x in cpp:
        write_page(x)
        f.write('   ' + x + '\n')

    f.write('\nConstants\n****************\n\n')
    for x in constants:
        f.write('* .. py:data:: ' + x + '\n')

    f.close()


if __name__ == '__main__':
    main()
