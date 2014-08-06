/* $Id: conf.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__CONF_H
#define __DRMAA_UTILS__CONF_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/common.h>

typedef enum {
	FSD_CONF_INTEGER,
	FSD_CONF_STRING,
	FSD_CONF_DICT
} fsd_conf_type_t;

struct fsd_conf_option_s {
	fsd_conf_type_t type;
	union {
		int integer;
		char *string;
		fsd_conf_dict_t *dict;
	} val;
};


/**
 * Read configuration.
 */
fsd_conf_dict_t *
fsd_conf_read(
		fsd_conf_dict_t *configuration,
		const char *filename, bool must_exist,
		const char *content, size_t content_len
		);


fsd_conf_dict_t *
fsd_conf_load( const char *filename );

fsd_conf_option_t *
fsd_conf_option_create( fsd_conf_type_t type, void *value );

void
fsd_conf_option_destroy( fsd_conf_option_t *option );

fsd_conf_option_t *
fsd_conf_option_merge( fsd_conf_option_t *lhs, fsd_conf_option_t *rhs );

void
fsd_conf_option_dump( fsd_conf_option_t *option );



fsd_conf_dict_t *
fsd_conf_dict_create(void);

void
fsd_conf_dict_destroy( fsd_conf_dict_t *dict );

fsd_conf_option_t *
fsd_conf_dict_get( fsd_conf_dict_t *dict, const char *key );

void
fsd_conf_dict_set(
		fsd_conf_dict_t *dict, const char *key, fsd_conf_option_t *value
		);

fsd_conf_dict_t *
fsd_conf_dict_merge( fsd_conf_dict_t *lhs, fsd_conf_dict_t *rhs );

void
fsd_conf_dict_dump( fsd_conf_dict_t *dict );


/*
 * Versions of functions above which do not raise exceptions.
 * Needed in conf_tab.y
 */

fsd_conf_option_t *
fsd_conf_option_create_noraise( fsd_conf_type_t type, void *value );

fsd_conf_dict_t *
fsd_conf_dict_create_noraise(void);

int
fsd_conf_dict_set_noraise(
		fsd_conf_dict_t *dict, const char *key, fsd_conf_option_t *value );

#endif /* __DRMAA_UTILS__CONF_H */

