/* $Id: environ.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__ENVIRON_H
#define __DRMAA_UTILS__ENVIRON_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/compat.h>

typedef struct fsd_environ_s fsd_environ_t;
typedef struct fsd_environ_item_s fsd_environ_item_t;

struct fsd_environ_s {
	void (*
	destroy)( fsd_environ_t *self );

	const char* (*
	get)( fsd_environ_t *self, const char *name );

	void (*
	set)( fsd_environ_t *self, char *name, char *value );

	void (*
	update)( fsd_environ_t *self, const char *const envp[] );

	char ** (*
	list)( fsd_environ_t *self );

	/**
	 * Modifies process's environment overwriting variables
	 * set in environ object.
	 * @return  Dictionary of overwritten values.
	 * @see #restore
	 */
	fsd_environ_t * (*
	apply)( fsd_environ_t *self );

	/**
	 * Restore process's environment to original state.
	 * All variables from \a self gets overwritten with values
	 * from \a saved_state or unset if variable doesn't exist
	 * in \a saved_state.
	 * @see #apply
	 */
	void (*
	restore)( fsd_environ_t *self, fsd_environ_t *saved_state );

	fsd_environ_item_t **_table;
	unsigned _table_size;
};

fsd_environ_t *
fsd_environ_new( const char *const envp[] );

struct fsd_environ_item_s {
	fsd_environ_item_t *next;
	char *name;
	char *value;
};

#endif /* __DRMAA_UTILS__ENVIRON_H */

