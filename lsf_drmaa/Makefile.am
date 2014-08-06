# $Id: Makefile.am 2304 2009-04-09 16:27:37Z lukasz $
#
# FedStage DRMAA for LSF
# Copyright (C) 2007-2008  FedStage Systems
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

RAGEL = @RAGEL@
RAGELFLAGS = 

lib_LTLIBRARIES = libdrmaa.la
libdrmaa_la_SOURCES = \
 drmaa.c \
 job.c job.h \
 native.c native.h \
 session.c session.h \
 util.c util.h

libdrmaa_la_CPPFLAGS =  -I$(top_srcdir)/drmaa_utils/ -I$(top_srcdir)/lsd_drmaa @LSF_INCLUDES@
if GCC
  libdrmaa_la_CPPFLAGS += -Wno-long-long
endif

libdrmaa_la_LIBADD = ../drmaa_utils/drmaa_utils/libdrmaa_utils.la @LSF_LIBS@
libdrmaa_la_LDFLAGS = @LSF_LDFLAGS@ \
  -version-info @LSF_DRMAA_VERSION_INFO@

native.c : native.rl
	$(RAGEL) $(RAGELFLAGS) -o native.c native.rl

EXTRA_DIST = native.rl
BUILT_SOURCES = native.c

if DEVELOPER_MODE
  CLEANFILES = $(BUILT_SOURCES)
endif

if GCC
native.lo: CFLAGS += -Wno-unused
endif

dist_sysconf_DATA = lsf_drmaa.conf.example
