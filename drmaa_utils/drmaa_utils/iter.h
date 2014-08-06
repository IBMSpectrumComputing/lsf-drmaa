/* $Id: iter.h 2 2009-10-12 09:51:22Z mamonski $ */
/*
 * FedStage DRMAA utilities library
 * Copyright (C) 2006-2008  FedStage Systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DRMAA_UTILS__ITER_H
#define __DRMAA_UTILS__ITER_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/common.h>

fsd_iter_t *
fsd_iter_new( char **list, int length );

fsd_iter_t *
fsd_iter_new_const( const char *const *list, int length );

struct fsd_iter_s {
	const char* (*next)( fsd_iter_t *self );
	void (*reset)( fsd_iter_t *self );
	int (*len)( fsd_iter_t *self );
	void (*append)( fsd_iter_t *self, char *string );
	void (*destroy)( fsd_iter_t *self );
	
	char **_list;
	int _position;
	int _length;
	bool _own_list;
};

#endif /* __DRMAA_UTILS__ITER_H */

