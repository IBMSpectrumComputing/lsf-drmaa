/* $Id: logging.c 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_EXECINFO_H
#	include <execinfo.h>
#endif

#include <drmaa_utils/common.h>
#include <drmaa_utils/logging.h>
#include <drmaa_utils/lookup3.h>
#include <drmaa_utils/thread.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: logging.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


static int fsd_logging_output = 2;

fsd_verbose_level_t fsd_verbose_level =
#ifdef DEBUGGING
	FSD_LOG_TRACE
#else
	FSD_LOG_WARNING
#endif
;

static struct timeval fsd_logging_start = {0, 0};


void
fsd_set_verbosity_level( fsd_verbose_level_t level )
{
	fsd_verbose_level = level;
}

void
fsd_set_logging_fd( int fd )
{
	fsd_logging_output = fd;
}


int
fsd_get_logging_fd( )
{
	return fsd_logging_output;
}



void
fsd_color( char *output, size_t len, int n )
{
	uint32_t k = n;
	k = hashword( &k, 1, 0 );
	k %= 12;
	snprintf( output, len, "\033[0;%d;%dm", k>=6, 31+k%6 );
}


char
fsd_log_level_char( int level )
{
	switch( level )
	 {
		case FSD_LOG_TRACE:	return 't';
		case FSD_LOG_DEBUG:	return 'd';
		case FSD_LOG_INFO:	 return 'I';
		case FSD_LOG_WARNING:  return 'W';
		case FSD_LOG_ERROR:	return 'E';
		case FSD_LOG_FATAL:	return 'F';
		default:			   return '?';
	 }
}


void
_fsd_log( int level, const char *file, const char *function,
		int kind, char *message )
{
	const bool color = false;
	char colorbeg[16];
	const char *colorend;
	int tid;
	long int seconds, microseconds;
	const char *prefix;
	const char *p;

	if( level < (int)fsd_verbose_level )
		return;

	if( message == NULL )
		return;

	tid = fsd_thread_id();
	if( color )
	 {
		fsd_color( colorbeg, sizeof(colorbeg), tid );
		colorend = "\033[0m";
	 }
	else
	 {
		colorbeg[0] = '\0';
		colorend = "";
	 }

	 {
		struct timeval tv;
		gettimeofday( &tv, NULL );
		seconds = tv.tv_sec;
		microseconds = tv.tv_usec;
	 }
	if( fsd_logging_start.tv_sec == 0 )
	 {
		time_t t;
		struct tm utc;
		char rep[32];
		fsd_logging_start.tv_sec = seconds;
		fsd_logging_start.tv_usec = microseconds;
		t = seconds;
		gmtime_r( &t, &utc );
		strftime( rep, sizeof(rep), "%Y-%m-%d %H:%M:%S", &utc );
		fsd_log_debug(( "logging started at: %s.%02ld Z",
					rep, microseconds/10000 ));
	 }
	if( microseconds < fsd_logging_start.tv_usec )
	 {
		seconds --;
		microseconds += 1000000;
	 }
	seconds -= fsd_logging_start.tv_sec;
	microseconds -= fsd_logging_start.tv_usec;

	switch( kind )
	 {
		case _FSD_LOG_ENTER:   prefix = "->";  break;
		case _FSD_LOG_RETURN:  prefix = "<-";  break;
		default:
			prefix = " *";
			function = "";
			break;
	 }

	p = message;
	do {
		if( *p == '\n' )
		 {
			prefix = " |";
			function = "";
			p++;
		 }
		else
		 {
			const char *end;
			char *line = NULL;
			int rc;
			end = strchr( p, '\n' );
			if( end == NULL )
				end = p + strlen(p);
			rc = asprintf( &line, "%c #%s%04x%s [%6ld.%02ld] %s %s%.*s\n",
					fsd_log_level_char(level), colorbeg, tid, colorend,
					seconds, microseconds/10000, prefix, function, (int)(end-p), p
					);
			if( rc != -1 )
				write( fsd_logging_output, line, strlen(line) );
			else
				return;
			free( line );
			p = end;
		 }
	} while( *p != '\0' );

	free( message );
}


void
fsd_log_fmt( int level, const char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	fsd_log_fmtv( level, fmt, args );
	va_end( args );
}


void
fsd_log_fmtv( int level, const char *fmt, va_list args )
{
	_fsd_log( level, NULL, NULL, _FSD_LOG_MSG, fsd_vasprintf(fmt, args) );
}


#if defined(__GNUC__) && defined(HAVE_EXECINFO_H)
#define MAX_STACKTRACE 128
void
fsd_log_stacktrace( int skip, int limit )
{
	void **ptr_buf = NULL;
	const char **symbols = NULL;
	int i, n;

	if( limit == 0 )
		limit = 128;
	skip++; /* without fsd_log_stacktrace() frame */
	n = skip + limit;

	ptr_buf = (void**)calloc( n, sizeof(void*) );
	if( ptr_buf == NULL )
		return;
	n = backtrace( ptr_buf, n );
	symbols = (const char**)backtrace_symbols( ptr_buf, n );
	if( symbols != NULL )
	 {
		fsd_log_debug(( "Stacktrace (most recent call last):" ));
		for( i = n-skip;  i >= 0;  i-- )
			fsd_log_debug(( "\n  %s", symbols[i] ));
		free( symbols );
	 }
}
#else
void fsd_log_stacktrace( int skip, int limit ) {}
#endif



