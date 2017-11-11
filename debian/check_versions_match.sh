#!/bin/sh

# debian/check_versions_match.sh - Verify that debian versions (in debian/control and debian/changelog)
#                                  match the upstream package version (as specified in CMakelists.txt)
#                                  Prints warnings and helpful hints on how to correct. Return a non-zero
#                                  exit code if there is a mismatch, zero otherwise
#
#                                  BONUS feature: call with environment variable FIXDEBIANFILESAUTOMAGICALLY=1,
#                                  and it'll fix things automagically and rerun itself to verify things worked.

EXIT_CODE=0

### Helper rountines

get_cmake_var() {
  # Extract a (simply and directly defined) variable from CMakeLists.txt and echo to stdout
  VARNAME=$1
  sed -ne "s/set([[:space:]]\+${VARNAME}[[:space:]]\+\"\(.\+\)\"[[:space:]]\+)/\1/p" CMakeLists.txt
}

fixcmd() {
  # Recommend to user a command to fix things; in future we may provide a command-line arg to execute the command
  # Usage fixcmd "cmd to fix stuff" "error message"

  if [ -n "$FIXDEBIANFILESAUTOMAGICALLY" ]
  then
     echo "WARNING: $2"
     echo "       : Since FIXDEBIANFILESAUTOMAGICALLY requested, executing: $1"
     bash -c "$1"
  else
     echo "ERROR: $2"
     echo " HINT: $1"
  fi

}


### Verify that version in debian/changelog matches version in CMakeLists.txt


CHANGELOG_UPSTREAM_VER=`dpkg-parsechangelog  | sed -ne "s/Version: \([[:digit:]]\+:\)\?\(.\+\)\(-.\+\)/\2/p"`
SOURCE_VER=$(get_cmake_var GPSTK_VERSION_MAJOR).$(get_cmake_var GPSTK_VERSION_MINOR).$(get_cmake_var GPSTK_VERSION_PATCH)

if [ ! $CHANGELOG_UPSTREAM_VER = $SOURCE_VER ]
then
   fixcmd "dch --newversion ${SOURCE_VER}-1 \"Update to upstream version ${SOURCE_VER}\"" \
          "debian/changelog version ($CHANGELOG_UPSTREAM_VER) doesn't match CMakeLists.txt version ($SOURCE_VER)"
   EXIT_CODE=1
fi

### Verify that package name matches SOVERSION

# Extract major.minor for shared library SONAME, matches algorithm in CMakelists.txt (grep for SOVERSION)
SOVERSION=`echo $SOURCE_VER | sed -ne "s/\([[:digit:]]\+\.[[:digit:]]\+\)\.[[:digit:]]\+/\1/p"  `
EXPECTEDLIBPKGNAME=libgpstk${SOVERSION}

[ -z "$SOVERSION" ] && {  echo "ERROR: Could't compute SOVERSION from source version ($SOURCE_VER)"; exit 1; }

NON_MATCHING_MENTIONS=`grep -o "libgpstk[0-9.]\+" debian/control | grep -xv "$EXPECTEDLIBPKGNAME" | tr "\n" " "`

if [ ! -z "$NON_MATCHING_MENTIONS" ]
then
   fixcmd "sed -i -e \"s/\(libgpstk\)[0-9.]\+/\\\1${SOVERSION}/\" debian/control" \
          "debian/control has mentions library package names ($NON_MATCHING_MENTIONS) other than expected $EXPECTEDLIBPKGNAME"
   EXIT_CODE=1
fi


### Exit back to user


if [ $EXIT_CODE -eq 1 -a -n "$FIXDEBIANFILESAUTOMAGICALLY" ]
then
   echo "Since FIXDEBIANFILESAUTOMAGICALLY was specified and things were corrected, rerun $0 to verify everything is fixed..."
   unset FIXDEBIANFILESAUTOMAGICALLY
   $0
   EXIT_CODE=$?
   
   if [ $EXIT_CODE -eq 0 ]
   then
      echo "   ... rerun passed!"
   else
      echo "   ... rerun failed!"
   fi
fi

exit $EXIT_CODE

