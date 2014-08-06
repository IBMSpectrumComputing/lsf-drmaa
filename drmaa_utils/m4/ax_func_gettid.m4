# $Id$
#
# SYNOPSIS
#
#   AX_FUNC_GETTID([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Check for gettid system call.  When found this macro
#   defines HAVE_GETTID.  It may be used after following
#   definition (it is not defined in Linux headers)::
#
#     #include <sys/types.h>
#     #include <sys/syscall.h>
#     pid_t gettid(void) {  return (pid_t)syscall( __NR_gettid );  }
#
# LAST MODIFICATION
#
#   2008-06-13
#
# LICENSE
#
#   Written by Łukasz Cieśnik <lukasz.ciesnik@fedstage.com>
#   and placed under Public Domain
#

AC_DEFUN([AX_FUNC_GETTID],[
AC_MSG_CHECKING([for gettid])
AC_REQUIRE([AC_PROG_CC])
AH_TEMPLATE([HAVE_GETTID], [Define to 1 if you have the gettid() syscall.])

AC_LANG_PUSH([C])
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
@%:@include <sys/types.h>
@%:@include <sys/syscall.h>
@%:@include <unistd.h>
pid_t gettid(void) { return (pid_t)syscall(__NR_gettid); }]],
	[[pid_t tid = gettid(); return 0;]]),
[ax_func_gettid_ok=yes], [ax_func_gettid_ok=no])
AC_LANG_POP([C])

AC_MSG_RESULT([$ax_func_gettid_ok])
if test x$ax_func_gettid_ok = xyes; then
	AC_DEFINE([HAVE_GETTID], [1])
	ifelse([$1], , :, [$1])
else
	ifelse([$2], , :, [$2])
fi
])
