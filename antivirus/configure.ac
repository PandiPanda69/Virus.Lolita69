AC_PREREQ([2.68])
AC_INIT(antivirus, 0.1, sebastien [dt] meriot [at] gmail [dt] com)
AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_MAKE_SET

if test "x$GCC" = "xyes"
then
	CFLAGS="$CFLAGS -Wall"
fi

AC_HAVE_LIBRARY(boost_regex, , AC_MSG_ERROR("Library Boost-Regex not found."))
AC_HAVE_LIBRARY(boost_system, , AC_MSG_ERROR("Library Boost-System not found."))
AC_HAVE_LIBRARY(boost_filesystem, , AC_MSG_ERROR("Library Boost-Filesystem not found."))
AC_HAVE_LIBRARY(pthread, , AC_MSG_ERROR("Library pthread not found."))
AC_HAVE_LIBRARY(zip, , AC_MSG_ERROR("Library zip not found."))
AC_HAVE_LIBRARY(magic, , AC_MSG_ERROR("Library magic not found."))
dnl AC_HAVE_LIBRARY(log4cplus, , AC_MSG_ERROR("Library Log4CPlus not found."))

AC_CHECK_HEADERS([boost/regex.hpp], , AC_MSG_ERROR("Boost-Regex headers not found."))
AC_CHECK_HEADERS([magic.h], , AC_MSG_ERROR("Magic headers not found."))

dnl !!! Not working and dunno why !!!
dnl AC_CHECK_HEADERS([boost/filesystem.hpp], , AC_MSG_ERROR("Boost-Filesystem headers not found."))

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/antivirus/Makefile
	src/database_packer/Makefile
	src/database_injector/Makefile
	src/libc_hijack/Makefile
	])

AC_OUTPUT
