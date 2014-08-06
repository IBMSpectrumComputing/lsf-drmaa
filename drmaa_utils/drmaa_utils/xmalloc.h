/* $Id: xmalloc.h 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file xmalloc.h
 * Memory allocation/deallocation routines.
 */

#ifndef __DRMAA_UTILS__XMALLOC_H
#define __DRMAA_UTILS__XMALLOC_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/compat.h>

/**
 * @defgroup xmalloc  Memory allocation/deallocation routines.
 */
/* @{ */


/**
 * Allocates <code> sizeof(type) </code> bytes.  Address of block is stored
 * in @a p.  Upon failure FSD_ERRNO_NO_MEMORY is raised and @a p is NULL.
 */
#define fsd_malloc( p, type ) \
	fsd_malloc_( (void**)(void*)&(p), sizeof(type) )

/**
 * Allocates <code> n * sizeof(type) </code> bytes of memory and stores
 * address in \a p.  Allocated block is filled with zeros.  Upon failure
 * error FSD_ERRNO_NO_MEMORY is raised.
 * \c NULL is assigned to \a p on error or when <code> n == 0 </code>.
 */
#define fsd_calloc( p, n, type ) \
	fsd_calloc_( (void**)(void*)&(p), (n), sizeof(type) )

#define fsd_realloc( p, n, type ) \
	fsd_realloc_( (void**)(void*)&(p), (n)*sizeof(type) )

/**
 * Fress previously allocated memory pointed by @a p.
 * When <code> p == NULL </code> it does nothing.
 */
void fsd_free( void *p );

void *fsd_malloc_( void **p, size_t size );
void *fsd_calloc_( void **p, size_t n, size_t size );
void *fsd_realloc_( void **p, size_t size );


#define fsd_malloc_noraise( p, type ) \
	fsd_malloc_noraise_( (void**)(void*)&(p), sizeof(type) )

#define fsd_calloc_noraise( p, n, type ) \
	fsd_calloc_noraise_( (void**)(void*)&(p), (n), sizeof(type) )

#define fsd_realloc_noraise( p, n, type ) \
	fsd_realloc_noraise_( (void**)(void*)&(p), (n)*sizeof(type) )

#define fsd_free_noraise( p )  (fsd_free(p), 0)

int fsd_malloc_noraise_( void **p, size_t size );
int fsd_calloc_noraise_( void **p, size_t n, size_t size );
int fsd_realloc_noraise_( void **p, size_t size );

/* @} */

#endif /* __DRMAA_UTILS__XMALLOC_H */

