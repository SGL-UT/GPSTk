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
