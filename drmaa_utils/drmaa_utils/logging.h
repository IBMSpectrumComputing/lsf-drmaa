/* $Id: logging.h 2 2009-10-12 09:51:22Z mamonski $ */
/*
 *  FedStage DRMAA utilities library
 *  Copyright (C) 2006-2008  FedStage Systems
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DRMAA_UTILS__LOGGING_H
#define __DRMAA_UTILS__LOGGING_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdio.h>

#include <drmaa_utils/compat.h>
#include <drmaa_utils/util.h>

#define _log_fmt(level, kind, args) \
	do { \
		if( (int)fsd_verbose_level <= level ) \
			_fsd_log( level, __FILE__, __FUNCTION__, kind, \
					fsd_asprintf args ); \
	} while(0)

#define _log_empty(level, kind) \
	do { \
		if( (int)fsd_verbose_level <= level ) \
			_fsd_log( level, __FILE__, __FUNCTION__, kind, NULL ); \
	} while(0)

#define _log_nop \
	do { /* nothing */ } while(0)


#define fsd_log_trace(args)   _log_fmt(FSD_LOG_TRACE, _FSD_LOG_MSG, args)
#define fsd_log_debug(args)   _log_fmt(FSD_LOG_DEBUG, _FSD_LOG_MSG, args)
#define fsd_log_enter(args)   _log_fmt(FSD_LOG_TRACE, _FSD_LOG_ENTER, args)
#define fsd_log_return(args)  _log_fmt(FSD_LOG_TRACE, _FSD_LOG_RETURN, args)


#define fsd_log_info(args)     _log_fmt(FSD_LOG_INFO, _FSD_LOG_MSG, args)
#define fsd_log_warning(args)  _log_fmt(FSD_LOG_WARNING, _FSD_LOG_MSG, args)
#define fsd_log_error(args)    _log_fmt(FSD_LOG_ERROR, _FSD_LOG_MSG, args)
#define fsd_log_fatal(args)    _log_fmt(FSD_LOG_FATAL, _FSD_LOG_MSG, args)

enum{ _FSD_LOG_MSG, _FSD_LOG_ENTER, _FSD_LOG_RETURN };
void _fsd_log( int level, const char *file, const char *function,
	int kind, char *message );

void fsd_log_fmt( int level, const char *fmt, ... )
	__attribute__(( format( printf, 2, 3 ) ));

void fsd_log_fmtv( int level, const char *fmt, va_list args );

void fsd_log_stacktrace( int skip, int limit );


/**
 * Specify place where log messages shall be written.
 * By default they are written to standard error stream (2).
 * @param fd   File descriptor to write to.
 */
void
fsd_set_logging_fd( int fd );

typedef enum {
	FSD_LOG_ALL,
	FSD_LOG_TRACE,
	FSD_LOG_DEBUG,
	FSD_LOG_INFO,
	FSD_LOG_WARNING,
	FSD_LOG_ERROR,
	FSD_LOG_FATAL,
	FSD_LOG_NONE
} fsd_verbose_level_t;

void
fsd_set_verbosity_level( fsd_verbose_level_t level );

extern fsd_verbose_level_t fsd_verbose_level;

#endif /* __DRMAA_UTILS__LOGGING_H */

