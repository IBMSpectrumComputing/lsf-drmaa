/* $Id: util.h 2 2009-10-12 09:51:22Z mamonski $ */
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

/**
 * @file util.h
 * Various functions.
 */

#ifndef __DRMAA_UTILS__UTIL_H
#define __DRMAA_UTILS__UTIL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <time.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/compat.h>

char *fsd_explode( const char *const *vector, char glue, int n );
void fsd_free_vector( char **vector );
char **fsd_copy_vector( const char *const * vector );
char *fsd_replace( char *input, const char *placeholder, const char *value );

char *fsd_strdup( const char *s );
char *fsd_strndup( const char *s, size_t n );

int fsd_atoi( const char *s );

void
fsd_str_append(
		bool *truncated,
		char **p, char *end,
		const char *fmt, ...
		)
		__attribute__(( format( printf, 4, 5 ) ));

size_t
fsd_snprintf(
		bool *truncated,
		char *str, size_t size,
		const char *fmt, ...
		)
		__attribute__(( format( printf, 4, 5 ) ));

size_t
fsd_vsnprintf(
		bool *truncated,
		char *str, size_t size,
		const char *fmt, va_list args
		);

/**
 * Behaves like asprintf function from standard C library
 * except any errors are marked in error context structure.
 *
 * It substitutes `%m` format string as in glibc
 * (with `strerror(errno)`).
 */
char *fsd_asprintf( const char *fmt, ... )
		__attribute__(( format( __printf__, 1, 2 ) ));

/**
 * Behaves like vasprintf function from standard C library
 * except any errors are marked in error context structure.
 */
char *fsd_vasprintf( const char *fmt, va_list args );

/**
 * Implements GNU version of strerror_r function - thread-safe
 * function returning message describing error code.
 *
 * From Linux Programmer's Manual:
 *
 * <code> char *strerror_r(int errnum, char *buf, size_t buflen); </code>
 *
 * The GNU-specific strerror_r() returns a pointer to a string containing
 * the error message.  This may be either a pointer to a string that the
 * function stores in buf, or a pointer to some (immutable) static string
 * (in which case buf is unused).  If the function stores a string in
 * buf, then at most buflen bytes are stored (the string may be truncated
 * if buflen is too small) and the string always includes a terminating
 * null byte.
 */
const char *
fsd_strerror_r( int errnum, char *buffer, size_t buffer_size );

char *
fsd_astrerror( int errnum, bool *malloced );

/** Retrievs current system timestamp. */
void fsd_get_time( struct timespec *ts );

/** Add delta to timestamp. */
void fsd_ts_add( struct timespec *a, const struct timespec *b );

/**
 * Compares two timestamps.
 * @return Negative integer when a < b (a represents earlier timestamp),
 *   positive integer when a > b or 0 when timestamps are equal.
 */
int fsd_ts_cmp( const struct timespec *a, const struct timespec *b );

/**
 * Reads file contents.
 * @param filename  Path to the file.
 * @param must_exist  Controls behaviour when file not exist
 *   (or is not readable).  If set to \c true DRMAA_ERRNO_INTERNAL_ERROR
 *   is raised on such occasion.  If set to \c false only
 *   \a content is set to \c NULL and no error is raised.
 * @param content  Filled with pointer to buffer with file contents
 *   or \c NULL when error is encoureged.  Caller is responsible
 *   for free()'ing it.
 * @param length  Filled with length of \a content buffer.
 */
void
fsd_read_file(
		const char *filename, bool must_exist,
		char **content, size_t *length
		);

/**
 * Gets path to current working directory.
 * @return  Path to process's current working directory
 *   in malloc'ed '\0' terminated buffer
 *   or \c NULL in case of error.
 */
char *
fsd_getcwd(void);

/**
 * Returns signal name.
 * @param signum  Valid signal number.
 */
const char *
fsd_strsignal( int signum );

#endif /* __DRMAA_UTILS__UTIL_H */
