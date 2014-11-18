/* $Id: fsd_util.c 185 2010-05-13 17:10:00Z mmatloka $ */
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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/util.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: fsd_util.c 185 2010-05-13 17:10:00Z mmatloka $";
#endif


static char *
fsd_expand_printf_ph( const char *fmt );


void
fsd_free_vector( char **vector )
{
	char **i;
	if( vector == NULL )
		return;
	for( i = vector;  *i != NULL;  i++ )
		fsd_free( *i );
	fsd_free( vector );
}


char **
fsd_copy_vector( const char *const * vector )
{
	unsigned n_items, i;
	char** volatile buf = NULL;
	char** volatile result = NULL;

	TRY
	 {
		if( vector )
			for( n_items = 0;  vector[ n_items ] != NULL;  n_items++ ) {}
		else
			n_items = 0;

		fsd_calloc( buf, n_items+1, char* );
		for( i = 0;  i < n_items;  i++ )
			buf[i] = fsd_strdup( vector[i] );
	 }
	ELSE
	 { result = buf;  buf = NULL; }
	FINALLY
	 { fsd_free_vector( buf ); }
	END_TRY

	return result;
}


char *
fsd_replace( char *str, const char *placeholder, const char *value )
{
	size_t ph_len, v_len;
	char *found = NULL;
	char* volatile s = str;

	if( str == NULL )
		fsd_exc_raise_code( FSD_ERRNO_INTERNAL_ERROR );

	ph_len = strlen( placeholder );
	v_len  = strlen( value );

	do {
		size_t s_len;
		s_len = strlen( s );
		found = strstr( s, placeholder );
		if( found )
		 {
			char* volatile result;
			size_t pos = found - s;
			TRY
			 {
				fsd_calloc( result, s_len - ph_len + v_len + 1, char );
				memcpy( result, s, pos );
				memcpy( result+pos, value, v_len );
				memcpy( result+pos+v_len, s+pos+ph_len, s_len-pos-ph_len );
				result[ s_len-ph_len+v_len ] = 0;
			 }
			FINALLY
			 {
				fsd_free( s );
				s = result;
			 }
			END_TRY
		 }
	} while( found );

	return s;
}


char *
fsd_explode( const char *const *vector, char glue, int n )
{
	char* volatile result = NULL;
	char* volatile buf = NULL;
	char *s;
	const char *const *i;
	unsigned idx, max=(unsigned)n;
	size_t size =0;

	TRY
	 {
		for( i = vector, idx = 0;  idx < max && *i != NULL;  i++, idx++ )
		 {
			if( i != vector )
				size++;
			size += strlen(*i);
		 }

		fsd_calloc( buf, size+1, char );
		s = buf;
		for( i = vector, idx = 0;  idx < max && *i != NULL;  i++, idx++ )
		 {
			if( i != vector )
				*s++ = glue;
			strcpy( s, *i );
			s += strlen( *i );
		 }
	 }
	ELSE
	 { result = buf;  buf = NULL; }
	FINALLY
	 { fsd_free( buf ); }
	END_TRY

	return result;
}


char *
fsd_strdup( const char *s )
{
	char *result;
	if( s == NULL )
		return NULL;
	result = strdup( s );
	if( result == NULL )
		fsd_exc_raise_sys( ENOMEM );
	return result;
}


char *
fsd_strndup( const char *s, size_t n )
{
	char *result;
	if( s == NULL )
		return NULL;
	result = strndup( s, n );
	if( result == NULL )
		fsd_exc_raise_sys( ENOMEM );
	return result;
}


int
fsd_atoi( const char *string )
{
	const char *s = string;
	int sign = +1;
	int v = 0;

	while( isspace(*(const unsigned char*)s) )
		s++;
	switch( *s )
	 {
		case '+':  sign = +1;  s++;  break;
		case '-':  sign = -1;  s++;  break;
		default:  break;
	 }
	while( isspace(*(const unsigned char*)s) )
		s++;
	if( '0' <= *s  &&  *s <= '9' )
	 {
		while( '0' <= *s  &&  *s <= '9' )
		 {
			if( v > INT_MAX/10 )
				goto range_error;
			v *= 10;
			v += *s++ - '0';
			if( v < 0 )
				goto range_error;
		 }
	 }
	else
		goto nan_error;

	while( isspace(*(const unsigned char*)s) )
		s++;
	if( *s != '\0' )
		goto nan_error;

	return sign * v;

range_error:
	fsd_exc_raise_fmt( FSD_ERRNO_INVALID_ARGUMENT,
			"value out of range: %s", string );
	return 0;

nan_error:
		fsd_exc_raise_fmt( FSD_ERRNO_INVALID_ARGUMENT,
				"not an number: %s", string );
	return 0;
}


void
fsd_str_append(
		bool *truncated,
		char **p, char *end,
		const char *fmt, ...
		)
{
	va_list args;
	va_start( args, fmt );
	*p += fsd_vsnprintf( truncated, *p, end-*p, fmt, args );
	va_end( args );
}


size_t
fsd_snprintf(
		bool *truncated,
		char *str, size_t size,
		const char *fmt, ...
		)
{
	va_list args;
	size_t result;
	va_start( args, fmt );
	result = fsd_vsnprintf( truncated, str, size, fmt, args );
	va_end( args );
	return result;
}


size_t
fsd_vsnprintf(
		bool *truncated,
		char *str, size_t size,
		const char *fmt, va_list args
		)
{
	int n;
	char *fmt_buf = NULL;
	fmt_buf = fsd_expand_printf_ph( fmt );
	if( fmt_buf )
		fmt = fmt_buf;
	n = vsnprintf( str, size, fmt, args );
	if( fmt_buf )
		fsd_free( fmt_buf );
	if( 0 <= n  &&  (size_t)n < size )
	 {}
	else
	 {
		str[size-1] = '\0';
		*truncated = true;
	 }
	return n;
}


char *
fsd_asprintf( const char *fmt, ... )
{
	va_list args;
	char *result = NULL;
	va_start( args, fmt );
	result = fsd_vasprintf( fmt, args );
	va_end( args );
	return result;
}


char *
fsd_vasprintf( const char *fmt, va_list args )
{
	char *result = NULL;
	char *fmt_buf = NULL;
	int rc;
	fmt_buf = fsd_expand_printf_ph( fmt );
	if( fmt_buf )
		fmt = fmt_buf;
	rc = vasprintf( &result, fmt, args );
	if( fmt_buf )
		fsd_free( fmt_buf );
	if( rc == -1 )
		fsd_exc_raise_sys( ENOMEM );
	return result;
}


/**
 * ``%m`` placeholder which is a glibc extension
 * can be safely used in fsd_*printf functions.
 */
char *
fsd_expand_printf_ph( const char *fmt )
{
	char* volatile result = NULL;

	if( strstr(fmt, "%m") == NULL )
		return result;

	TRY
	 {
		const char *pos;
		while( (pos = strstr(fmt, "%m")) != NULL
				&&  (pos == fmt  ||  pos[-1] != '%') )
		 {
			char* volatile errno_msg = NULL;
			volatile bool own_errno_msg = false;
			TRY
			 {
				char *buf = NULL;
				size_t fmt_len = strlen(fmt);
				size_t ph_pos = pos - fmt;
				size_t errno_msg_len;
				errno_msg = fsd_astrerror( errno, (bool*)&own_errno_msg );
				errno_msg_len = strlen(errno_msg);
				fsd_calloc( buf, fmt_len-2+errno_msg_len+1, char );
				/* replace ``%m`` with strerror(errno) */
				strncat( buf, fmt, ph_pos );
				strcat( buf+ph_pos, errno_msg );
				strcat( buf+ph_pos+errno_msg_len, fmt+ph_pos+2 );
				fsd_free( result );
				fmt = result = buf;
			 }
			FINALLY
			 {
				if( errno_msg && own_errno_msg )
					fsd_free( errno_msg );
			 }
			END_TRY
		 }
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( result );
		fsd_exc_reraise();
	 }
	END_TRY

	return result;
}


const char *
fsd_strerror_r( int errnum, char *buffer, size_t buffer_size )
{
#	ifdef HAVE_DECL_STRERROR_R
#		ifdef _GNU_SOURCE
			return strerror_r( errnum, buffer, buffer_size );
#		else
			strerror_r( errnum, buffer, buffer_size );
			return buffer;
#		endif /* !_GNU_SOURCE */
#	else /* assume strerror is thread safe -- returned string is constant */
		return strerror(errnum);
#	endif /* ! HAVE_STRERROR_R */
}


char *
fsd_astrerror( int errnum, bool *malloced )
{
#ifdef HAVE_DECL_STRERROR_R
	char *buffer = NULL;
	size_t buffer_size = 0;
	char *result = NULL;

	do {
		if( buffer_size == 0 )
			buffer_size = 1024;
		else
			buffer_size *= 2;
		fsd_realloc( buffer, buffer_size, char );
#		ifdef _GNU_SOURCE
		result = strerror_r( errnum, buffer, buffer_size );
		if( result == buffer && strlen(result)+1 == buffer_size )
			continue;
#		else
		if( strerror_r( errnum, buffer, buffer_size ) && errno == ERANGE )
		 {
			result = buffer;
			continue;
		 }
#		endif /* !_GNU_SOURCE */
	} while( false );
	*malloced = (buffer == result);
	if( buffer != result )
		fsd_free( buffer );
	return result;
#else /* ! HAVE_DECL_STRERROR_R */
	/* assume strerror is thread safe -- returned string is constant */
	*malloced = false;
	return strerror(errnum);
#endif /* ! HAVE_DECL_STRERROR_R */
}


void
fsd_get_time( struct timespec *ts )
{
	struct timeval tv;
	int rc;
	rc = gettimeofday( &tv, NULL );
	if( rc )
		fsd_exc_raise_sys( 0 );
	else
	 {
		ts->tv_sec = tv.tv_sec;
		ts->tv_nsec = 1000 * tv.tv_usec;
	 }
}


void
fsd_ts_add( struct timespec *a, const struct timespec *b )
{
	const int nano = 1000000000;
	a->tv_sec += b->tv_sec;
	a->tv_nsec += b->tv_nsec;
	if( a->tv_nsec >= nano )
	 {
		a->tv_nsec -= nano;
		a->tv_sec ++;
	 }
}


int
fsd_ts_cmp( const struct timespec *a, const struct timespec *b )
{
	if( a->tv_sec != b->tv_sec )
		return a->tv_sec - b->tv_sec;
	else
		return a->tv_nsec - b->tv_nsec;
}


void
fsd_read_file(
		const char *filename, bool must_exist,
		char **content, size_t *length
		)
{
	volatile int fd = -1;
	char* volatile buffer = NULL;
	size_t size = 0;
	size_t capacity = 0;
	ssize_t n_read;

	TRY
	 {
		*content = NULL;
		*length = 0;

		fd = open( filename, O_RDONLY );
		if( fd == -1 )
		 {
			if( errno==ENOENT && !must_exist )
			 {
				*content = NULL;
				*length = 0;
			 }
			else
				fsd_exc_raise_sys( 0 );
		 }
		else
		 {
			do {
				capacity = size + 1024;
				fsd_realloc( buffer, capacity, char );
				n_read = read( fd, buffer + size, capacity-size );
				if( n_read == (ssize_t)-1 )
					fsd_exc_raise_sys( 0 );
				else
					size += n_read;
			} while( n_read != 0 );

			fsd_realloc( buffer, size+1, char );
			buffer[ size ] = '\0';
		 }
	 }
	ELSE
	 {
		*content = buffer;  buffer = NULL;
		*length = size;
	 }
	FINALLY
	 {
		fsd_free( buffer );
		if( fd >= 0 )
			close( fd );
	 }
	END_TRY
}


char *
fsd_getcwd(void)
{
	char* volatile buffer = NULL;
	char* volatile result = NULL;
	size_t size = 64;

	TRY
	 {
		fsd_calloc( buffer, size, char );
		while( true )
		 {
			char *result = getcwd( buffer, size );
			if( result != NULL )
				break;
			else if( errno == ERANGE )
			 {
				size *= 2;
				fsd_realloc( buffer, size, char );
			 }
			else
				fsd_exc_raise_sys( 0 );
		 }
		fsd_realloc( buffer, strlen(buffer)+1, char );
	 }
	ELSE
	 { result = buffer;  buffer = NULL; }
	FINALLY
	 { fsd_free( buffer ); }
	END_TRY

	return result;
}


const char *
fsd_strsignal( int signum )
{
#define DETECT_SIG( signame ) \
	case signame:  return #signame;
	switch( signum )
	 {
		/* signals described by POSIX.1 */
		DETECT_SIG( SIGHUP  )
		DETECT_SIG( SIGINT	)
		DETECT_SIG( SIGQUIT )
		DETECT_SIG( SIGILL	)
		DETECT_SIG( SIGABRT )
		DETECT_SIG( SIGFPE	)
		DETECT_SIG( SIGKILL )
		DETECT_SIG( SIGSEGV )
		DETECT_SIG( SIGPIPE )
		DETECT_SIG( SIGALRM )
		DETECT_SIG( SIGTERM )
		DETECT_SIG( SIGUSR1 )
		DETECT_SIG( SIGUSR2 )
		DETECT_SIG( SIGCHLD )
		DETECT_SIG( SIGCONT )
		DETECT_SIG( SIGSTOP )
		DETECT_SIG( SIGTSTP )
		DETECT_SIG( SIGTTIN )
		DETECT_SIG( SIGTTOU )
		/* signals described in SUSv2 and SUSv3 / POSIX 1003.1-2001 */
#ifdef SIGBUS
		DETECT_SIG( SIGBUS	)
#endif
#ifdef SIGPOLL
		DETECT_SIG( SIGPOLL	  )
#endif
#ifdef SIGPROF
		DETECT_SIG( SIGPROF	  )
#endif
#ifdef SIGSYS
		DETECT_SIG( SIGSYS	  )
#endif
#ifdef SIGTRAP
		DETECT_SIG( SIGTRAP	  )
#endif
#ifdef SIGURG
		DETECT_SIG( SIGURG	  )
#endif
#ifdef SIGVTALRM
		DETECT_SIG( SIGVTALRM )
#endif
#ifdef SIGXCPU
		DETECT_SIG( SIGXCPU	  )
#endif
#ifdef SIGXFSZ
		DETECT_SIG( SIGXFSZ	  )
#endif
		default:
#		ifdef HAVE_SYS_SIGLIST
			return sys_siglist[signum];
#		else
			return "unknown signal?!";
#		endif
			break;
	 }
}

