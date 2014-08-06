# $Id$
#
# SYNOPSIS
#
#   AX_FUNC_VA_COPY([ACTION-IF-FOUND[, [ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Test for va_copy function/macro.
#   When found it defines ``HAVE_VA_COPY`` and/or ``HAVE___VA_COPY``
#   when ``va_copy`` and/or ``__va_copy`` is found respectively.
#
# LAST MODIFICATION
#
#   2008-06-13
#
# LICENSE
#
#   Written by Łukasz Cieśnik <lukasz.ciesnik@fedstage.com>
#   and placed under Public Domain.
#

AC_DEFUN([AX_FUNC_VA_COPY], [

ax_func_va_copy_ok=no

AH_TEMPLATE([HAVE_VA_COPY],
		[Define to 1 if you have the va_copy function.])
AC_MSG_CHECKING([for va_copy])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>]],
			[[va_list a, b; va_copy(a, b);]])],[
	ax_func_va_copy_ok=yes
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_VA_COPY], [1])
  ],[
      AC_MSG_RESULT([no])
  ],[
      AC_MSG_RESULT([no])
])

AH_TEMPLATE([HAVE___VA_COPY],
		[Define to 1 if you have the __va_copy function.])
AC_MSG_CHECKING([for __va_copy])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>]],
			[[va_list a, b; __va_copy(a, b);]])],[
	ax_func_va_copy_ok=yes
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE___VA_COPY], [1])
	AH_BOTTOM([
#ifndef HAVE_VA_COPY
#	define va_copy(a,b) __va_copy(a,b)
#	define HAVE_VA_COPY 1
#endif
])
  ], [
  AC_MSG_RESULT([no])
  ], [
  AC_MSG_RESULT([no])
])

if test x$ax_func_va_copy_ok = xyes; then
	ifelse([$1], , :, [$1])
else
	ifelse([$2], , :, [$2])
fi
])
