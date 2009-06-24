dnl This file is part of the FreeType Jam project.
dnl See http://www.freetype.org/jam/ for more details
dnl
dnl Process this file with autoconf to produce a configure script.
dnl

AC_INIT
AC_CONFIG_SRCDIR([Makefile.in])

dnl release version number
dnl
jam_version='2.5.2'
AC_SUBST(jam_version)

dnl checks for system type
AC_CANONICAL_TARGET([])

dnl checks for programs
AC_PROG_CC
AC_PROG_CPP

dnl get Compiler flags right.
SUFEXE=
XX_CFLAGS=
case "$host" in
  *-cygwin*)
    CFLAGS=
    SUFEXE=.exe
    XX_CFLAGS="-D__cygwin__"
    ;;
  *-dec-osf*)
    CFLAGS=
    XX_CFLAGS="-std1 -g3"
    ;;
esac
AC_SUBST(XX_CFLAGS)
AC_SUBST(SUFEXE)

AC_CHECK_PROG(RMF, rm, rm -f)
AC_CHECK_PROG(RMDIR, rmdir, rmdir)

dnl Since this file will be finally moved to another directory we make
dnl the path of the install script absolute.  This small code snippet has
dnl been taken from automake's `ylwrap' script.
AC_PROG_INSTALL
case "$INSTALL" in
  /*)
    ;;
  */*)
    INSTALL="`pwd`/$INSTALL" ;;
esac

AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)

dnl we need to expand $bindir directly, instead of the default
dnl '${exec_prefix}/bin', so perform a little Autoconf magic
dnl this is really disgusting !!
AC_DEFUN([AC_EXPAND_DIR],
[$1=$2
$1=`( test "x$prefix" = xNONE && prefix="$ac_default_prefix"
      test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
      eval echo \""[$]$1"\"
    )`
])

AC_EXPAND_DIR(my_bindir, $bindir)
AC_SUBST(my_bindir)

dnl create the Unix-specific Makefile and version information
dnl
AC_CONFIG_FILES([../../Makefile:Makefile.in
                 ../../patchlevel.h:patchlevel.h.in
                 ../../Jamrules:Jamrules.in])

AC_OUTPUT

dnl end of configure.ac
