/* $Id: template.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__TEMPLATE_H
#define __DRMAA_UTILS__TEMPLATE_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/common.h>

typedef const fsd_attribute_t *
fsd_template_by_name_method( const fsd_template_t *self, const char *name );

typedef const fsd_attribute_t *
fsd_template_by_code_method( const fsd_template_t *self, int code );


fsd_template_t *
fsd_template_new(
		fsd_template_by_name_method *by_name_method,
		fsd_template_by_code_method *by_code_method,
		unsigned n_attributes
		);

struct fsd_template_s {
	const char* (*
	get_attr)( const fsd_template_t *self, const char *name );

	void (*
	set_attr)( fsd_template_t *self, const char *name, const char *value );

	const char* const* (*
	get_v_attr)( const fsd_template_t *self, const char *name );

	void (*
	set_v_attr)( fsd_template_t *self, const char *name, const char **value );

	const fsd_attribute_t* (*
	by_name)( const fsd_template_t *self, const char *name ); 

	const fsd_attribute_t* (*
	by_code)( const fsd_template_t *self, int code );

	void (*
	destroy)( fsd_template_t *self );

	void **attributes;
	unsigned n_attributes;
};

struct fsd_attribute_s {
	const char *name;
	int code;
	bool is_vector;
};


#endif /* __DRMAA_UTILS__TEMPLATE_H */

