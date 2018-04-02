# $Id: ax_lsf.m4 1540 2008-10-21 11:32:15Z lukasz $
#
# SYNOPSIS
#
#   AX_LSF([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Check for LSF libraries and headers.
#
#   This macro calls::
#
#     AC_SUBST(LSF_INCLUDES)
#     AC_SUBST(LSF_LDFLAGS)
#     AC_SUBST(LSF_LIBS)
#
# LAST MODIFICATION
#
#   2008-10-13
#
# LICENSE
#
#   Written by:
#     Mariusz Mamoński <mariusz.mamonski@fedstage.com>
#     Łukasz Cieśnik <lukasz.ciesnik@fedstage.com>
#   Placed under Public Domain.
#

AC_DEFUN([AX_LSF],[
AC_ARG_WITH([lsf-inc], [AC_HELP_STRING([--with-lsf-inc=<include-dir>],
		[Path to LSF headers directory])])
AC_ARG_WITH([lsf-lib], [AC_HELP_STRING([--with-lsf-lib=<lib-dir>],
		[Path to directory with LSF libraries])])
AC_ARG_WITH([lsf-static], [AC_HELP_STRING([--with-lsf-static],
		[Link against static LSF libraries instead of shared ones])])

AC_SUBST(LSF_INCLUDES)
AC_SUBST(LSF_LDFLAGS)
AC_SUBST(LSF_LIBS)

AC_MSG_NOTICE([checking for LSF])

AC_MSG_CHECKING([for LSF compile flags])
ax_lsf_msg=
if test x$with_lsf_inc != x; then
	LSF_INCLUDES="-I${with_lsf_inc}"
else
	if test x"$LSF_ENVDIR" != x; then
		. $LSF_ENVDIR/lsf.conf
	fi

	if test x"$LSF_INCLUDEDIR" != x; then
		LSF_INCLUDES="-I$LSF_INCLUDEDIR"
	else
		ax_lsf_msg="no (LSF_ENVDIR and/or LSF_INCLUDEDIR not set)"
	fi
fi
AC_MSG_RESULT([$LSF_INCLUDES$ax_lsf_msg])

AC_MSG_CHECKING([for LSF library dir])
ax_lsf_msg=
if test x$with_lsf_lib == x; then
	if test x"$LSF_LIBDIR" != x; then
		with_lsf_lib=$LSF_LIBDIR
	else
		ax_lsf_msg="no (LSF_ENVDIR and/or LSF_LIBDIR not set)"
	fi
fi
AC_MSG_RESULT([$with_lsf_lib$ax_lsf_msg])

if test x$with_lsf_static != x; then
	LSF_LIBS="${with_lsf_lib}/libbat.a ${with_lsf_lib}/liblsf.a"
	AC_HAVE_LIBRARY([-ldl], [LSF_LIBS="$LSF_LIBS -ldl"])
	LSF_LDFLAGS=""
else
	LSF_LIBS="-lbat -llsf"
        LSF_LDFLAGS="-L${with_lsf_lib} -Wl,-R${with_lsf_lib}"
      
        case "$host_os" in
            *darwin*)
            LSF_LDFLAGS="-L${with_lsf_lib}"
            ;;
            *)
            LSF_LDFLAGS="-L${with_lsf_lib} -Wl,-R${with_lsf_lib}"
            ;;
        esac

fi

AC_HAVE_LIBRARY([-lnsl], [LSF_LIBS="$LSF_LIBS -lnsl"])
AC_HAVE_LIBRARY([-lm], [LSF_LIBS="$LSF_LIBS -lm"])
AC_HAVE_LIBRARY([-lrt], [LSF_LIBS="$LSF_LIBS -lrt"])

CPPFLAGS_save="$CPPFLAGS"
LDFLAGS_save="$LDFLAGS"
LIBS_save="$LIBS"
CPPFLAGS="$CPPFLAGS $LSF_INCLUDES"
LDFLAGS="$LDFLAGS $LSF_LDFLAGS"
LIBS="$LIBS $LSF_LIBS"

ax_lsf_ok="no"

AC_MSG_CHECKING([for usable LSF libraries/headers])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
	#include <lsf/lsbatch.h>
	#include <stdio.h>]],
		[[   if (lsb_init("test") < 0) {
		lsb_perror("lsb_init"); 
		return 1;
		} ]])],
	[ ax_lsf_ok="yes"],
	[ echo "*** The LSF test program failed to link or run. See the file config.log"
	echo "*** for the exact error that occured."],
	[
		ax_lsf_ok=yes
		echo $ac_n "cross compiling; assumed OK... $ac_c"
	])
CPPFLAGS="$CPPFLAGS_save"
LDFLAGS="$LDFLAGS_save"
LIBS="$LIBS_save"
AC_MSG_RESULT([$ax_lsf_ok])

if test x"$ax_lsf_ok" = xyes; then
	ifelse([$1], , :, [$1])
else
	ifelse([$2], , :, [$2])
fi
])
