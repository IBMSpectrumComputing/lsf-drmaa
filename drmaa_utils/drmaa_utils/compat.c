/* $Id: compat.c 138 2010-04-29 17:13:31Z mamonski $ */
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <drmaa_utils/compat.h>


#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: compat.c 138 2010-04-29 17:13:31Z mamonski $";
#endif


#ifndef HAVE_STRLCPY
size_t
strlcpy( char *dest, const char *src, size_t size )
{
	size_t result = 0;
	if( size == 0 )
		return 0;
	while( *src  &&  --size > 0 )
	 {
		*dest++ = *src++;
		result++;
	 }
	*dest++ = '\0';
	return result;
}
#endif /* ! HAVE_STRLCPY */


#ifndef HAVE_STRNDUP
char *
strndup( const char *s, size_t n )
{
	char *result;
	if( s == NULL )
		return NULL;
	result = calloc( n + 1, sizeof(char) );

	if( result == NULL )
	 {
		errno = ENOMEM;
		return NULL;
	 }

	strlcpy( result, s, n + 1 );
	return result;
}
#endif /* ! HAVE_STRNDUP */


#ifndef HAVE_ASPRINTF
int
asprintf( char **strp, const char *fmt, ... )
{
	va_list args;
	int result;
	va_start( args, fmt );
	result = vasprintf( strp, fmt, args );
	va_end( args );
	return result;
}
#endif /* ! HAVE_ASPRINTF */


#ifndef HAVE_VASPRINTF
int
vasprintf( char **strp, const char *format, va_list ap )
{
	int size, check_size;
	char *buf = NULL;

#ifdef HAVE_VA_COPY
	va_list aq;

	va_copy(aq, ap);
#else
#  ifdef HAVE___VA_COPY
	va_list aq;

	__va_copy(aq, ap);
#  endif
#endif

	*strp = NULL;

#ifndef HAVE_C99_VSNPRINTF
	{
		int res;
		char *tmp;

		size = 128;
		do {
			size *= 2;
			if (!(tmp = realloc(buf, size))) {
				if (buf)
					free(buf);
				return -1;
			}
			buf = tmp;
			/* XXX we're assuming here there's no va_copy on this system */
			res = vsnprintf(buf, size, format, ap);
		} while (res == -1);
	}
#else
	{
		char tmp[2];

		/* on Solaris vsnprintf fails if buf is empty, so use a small one */
		size = vsnprintf(tmp, sizeof(tmp), format, ap);
		if (!(buf = malloc(size + 1)))
			return -1;
	}
#endif

#ifdef HAVE_VA_COPY
	check_size = vsnprintf(buf, size + 1, format, aq);
	va_end(aq);
#else
#  ifdef HAVE___VA_COPY
	check_size = vsnprintf(buf, size + 1, format, aq);
	va_end(aq);
#  else
	check_size = vsnprintf(buf, size + 1, format, ap);
#  endif
#endif
	assert(check_size <= size);
	*strp = buf;
	return 0;
}
#endif

