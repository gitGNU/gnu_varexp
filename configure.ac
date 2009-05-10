dnl configure.ac for libvarexp

AC_PREREQ(2.59)
AC_INIT([libvarexp], [1.2], [simons@cryp.to])
AM_INIT_AUTOMAKE([foreign 1.9 -Wall])
AC_CONFIG_SRCDIR([varexp.hpp])

dnl Get rid of the lousy '-g -O2' defaults.
CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

dnl Write results.
AC_CONFIG_FILES([Makefile tests/Makefile])
AC_OUTPUT
