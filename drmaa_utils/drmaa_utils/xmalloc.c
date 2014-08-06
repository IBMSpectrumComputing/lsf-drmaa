/* $Id: xmalloc.c 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file xmalloc.c
 * Memory allocation/deallocation routines.
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <drmaa_utils/xmalloc.h>
#include <drmaa_utils/compat.h>
#include <drmaa_utils/exception.h>
#include <drmaa_utils/logging.h>

#ifndef SIZE_T_MAX
#  define SIZE_T_MAX ((size_t) -1)
#endif

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: xmalloc.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


void *
fsd_malloc_( void **p, size_t size )
{
	void *ptr = NULL;
	if( size )
	 {
		ptr = malloc( size );
		if( ptr )
			memset( ptr, 0, size );
		else
		 {
			*p = NULL;
			fsd_exc_raise_sys( errno=ENOMEM );
		 }
	 }
	*p = ptr;
	return ptr;
}


void *
fsd_calloc_( void **p, size_t n, size_t size )
{
	void *ptr = NULL;
	if( n && size )
	 {
		if( n <= SIZE_T_MAX / size )
			ptr = calloc( n, size );
		if( !ptr )
		 {
			*p = NULL;
			fsd_exc_raise_sys( errno=ENOMEM );
		 }
	 }
	*p = ptr;
	return ptr;
}


void *
fsd_realloc_( void **p, size_t size )
{
	void *ptr = *p;

	if( size )
	 {
		if( ptr )
			ptr = realloc( ptr, size );
		else
			ptr = malloc( size );

		if( ptr != NULL )
			*p = ptr;
		else
			fsd_exc_raise_sys( errno=ENOMEM );
	 }
	else if( ptr != NULL )
	 {
		free( ptr );
		*p = ptr = NULL;
	 }

	return ptr;
}


void
fsd_free( void *p )
{
	if( p )
		free( p );
}


int
fsd_malloc_noraise_( void **p, size_t size )
{
	int result = 0;
	void *ptr = NULL;
	if( size )
	 {
		ptr = malloc( size );
		if( ptr )
			memset( ptr, 0, size );
		else
			result = ENOMEM;
	 }
	*p = ptr;
	return result;
}


int
fsd_calloc_noraise_( void **p, size_t n, size_t size )
{
	int result = 0;
	void *ptr = NULL;
	if( n && size )
	 {
		if( n <= SIZE_T_MAX / size )
			ptr = calloc( n, size );
		if( !ptr )
			result = ENOMEM;
	 }
	*p = ptr;
	return result;
}


int
fsd_realloc_noraise_( void **p, size_t size )
{
	int result = 0;
	void *ptr = *p;

	if( size )
	 {
		if( ptr )
			ptr = realloc( ptr, size );
		else
			ptr = malloc( size );

		if( ptr != NULL )
			*p = ptr;
		else
			result = ENOMEM;
	 }
	else if( ptr != NULL )
	 {
		free( ptr );
		*p = ptr = NULL;
	 }

	return result;
}

