#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([GPSTK], [1.3], [http://www.gpstk.org/], [gpstk])
AM_INIT_AUTOMAKE([1.7.6 foreign])
AC_CONFIG_SRCDIR([src/DayTime.hpp])

#gpstk 1.3 -> libgpstk.so.13.0, etc...
GPSTK_MAJOR_VERSION=13
GPSTK_MINOR_VERSION=0
GPSTK_SO_VERSION=$GPSTK_MAJOR_VERSION:$GPSTK_MINOR_VERSION:0
AC_SUBST(GPSTK_SO_VERSION)

# Checks for programs.
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_PROG_LIBTOOL

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stddef.h stdlib.h string.h strings.h sys/param.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([bzero floor ftime getcwd gettimeofday memset mkdir modf pow re_comp regcomp sqrt strstr strtol strtoul])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/rxio/Makefile
                 src/Makefile
                 apps/Makefile
                 apps/DataAvailability/Makefile
                 apps/MDPtools/Makefile
                 apps/Rinextools/Makefile
                 apps/checktools/Makefile
                 apps/converters/Makefile
                 apps/differential/Makefile
                 apps/difftools/Makefile
                 apps/filetools/Makefile
                 apps/geomatics/Makefile
                 apps/geomatics/cycleslips/Makefile
                 apps/geomatics/kalman/Makefile
                 apps/geomatics/lib/Makefile
                 apps/geomatics/relposition/Makefile
                 apps/geomatics/robust/Makefile
                 apps/ionosphere/Makefile
                 apps/mergetools/Makefile
                 apps/multipath/Makefile
                 apps/positioning/Makefile
                 apps/receiver/Makefile
                 apps/rfw/Makefile
                 apps/reszilla/Makefile
                 apps/time/Makefile
                 apps/visibility/Makefile
                 tests/Makefile
                 examples/Makefile])
AC_OUTPUT
