"""The GPS Toolkit, python extension package - data module."""
#----------------------------------------
# Help find data files
#----------------------------------------
def full_path( filename ):
    """
    Define a method that determines the full file path to a data file
    even after an install, assuming the data pacage is kept as a sub-package
    within the gpstk package file tree structure.
    """
    import os
    this_dir, this_filename = os.path.split(__file__)
    full_path = os.path.join( this_dir, filename )
    return( full_path )

#----------------------------------------
# use pkgutil load test data byte strings
#----------------------------------------
def load_data( ):
    """
    Define a method that determines loads all data as byte
    strings and stores those data into attributes in the
    gpstk.data namespace.

    Current byte string attributes added will be the following:
        gpstk.data.rinex3nav
        gpstk.data.rinex3obs
        gpstk.data.rinexmet
        gpstk.data.sem
        gpstk.data.sp3
        gpstk.data.yuma
    """
    import pkgutil

    try:
        rinex3nav = pkgutil.get_data( 'gpstk', 'data/rinex3nav_data.txt' )
        rinex3obs = pkgutil.get_data( 'gpstk', 'data/rinex3obs_data.txt' )
        rinexmet  = pkgutil.get_data( 'gpstk', 'data/rinexmet_data.txt' )
        sem       = pkgutil.get_data( 'gpstk', 'data/sem_data.txt' )
        sp3       = pkgutil.get_data( 'gpstk', 'data/sp3_data.txt' )
        yuma      = pkgutil.get_data( 'gpstk', 'data/yuma_data.txt' )
    except IOError:
        print( "FAIL: gpstk.data.load_data(): pkgutil.get_data() failed to load byte strings from GPSTk package data files" )
