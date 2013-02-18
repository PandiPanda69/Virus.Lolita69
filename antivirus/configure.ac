AC_PREREQ([2.68])
AC_INIT(antivirus, 0.1, sebastien [dt] meriot [at] gmail [dt] com)
AM_INIT_AUTOMAKE

AC_PROG_CXX
AC_PROG_MAKE_SET

if test "x$GCC" = "xyes"
then
	CFLAGS="$CFLAGS -Wall"
fi

AC_HAVE_LIBRARY(boost_regex, , AC_MSG_ERROR("Library Boost-Regex not found."))
dnl AC_HAVE_LIBRARY(log4cplus, , AC_MSG_ERROR("Library Log4CPlus not found."))

AC_CHECK_HEADERS([boost/regex.hpp], , AC_MSG_ERROR("Boost-Regex headers not found."))
dnl AC_CHECK_HEADERS([log4cplus/logger.h], , AC_MSG_ERROR("Log4Cplus headers not found.))

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/antivirus/Makefile
	src/database_packer/Makefile
	])

AC_OUTPUT
