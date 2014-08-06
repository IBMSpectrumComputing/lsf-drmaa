/* $Id: template.c 2 2009-10-12 09:51:22Z mamonski $ */
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

#include <drmaa_utils/compat.h>
#include <drmaa_utils/template.h>
#include <drmaa_utils/util.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: template.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


static const char*
fsd_template_get_attr( const fsd_template_t *self, const char *name )
{
	const fsd_attribute_t *attr = NULL;
	if( name == NULL )
		fsd_exc_raise_code( FSD_ERRNO_INVALID_ARGUMENT );
	attr = self->by_name( self, name );
	if( attr == NULL  ||  attr->is_vector )
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_ARGUMENT,
				"invalid scalar attribute name: %s", name
				);
	return (const char*) self->attributes[ attr->code ];
}


static void
fsd_template_set_attr( fsd_template_t *self,
		const char *name, const char *value )
{
	const fsd_attribute_t *attr = NULL;
	fsd_log_enter(("(%s=%s)", name, value));
	if( name == NULL )
		fsd_exc_raise_code( FSD_ERRNO_INVALID_ARGUMENT );
	attr = self->by_name( self, name );
	if( attr == NULL  ||  attr->is_vector )
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_ARGUMENT,
				"invalid scalar attribute name: %s", name
				);
	if( value != NULL )
		self->attributes[ attr->code ] = fsd_strdup( value );
	else
		self->attributes[ attr->code ] = NULL;
}


static const char* const*
fsd_template_get_v_attr( const fsd_template_t *self, const char *name )
{
	const fsd_attribute_t *attr = NULL;
	if( name == NULL )
		fsd_exc_raise_code( FSD_ERRNO_INVALID_ARGUMENT );
	attr = self->by_name( self, name );
	if( attr == NULL  ||  ! attr->is_vector )
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_ARGUMENT,
				"invalid vector attribute name: %s", name
				);
	return (const char* const*) self->attributes[ attr->code ];
}


static void
fsd_template_set_v_attr( fsd_template_t *self,
		const char *name, const char **value )
{
	const fsd_attribute_t *attr = NULL;
	char **volatile v = NULL;

	if( name == NULL )
		fsd_exc_raise_code( FSD_ERRNO_INVALID_ARGUMENT );
	attr = self->by_name( self, name );
	if( attr == NULL  ||  ! attr->is_vector )
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_ARGUMENT,
				"invalid vector attribute name: %s", name
				);

	TRY
	 {
		int code = attr->code;
		if( value != NULL )
			v = fsd_copy_vector( value );
		if( self->attributes[code] != NULL )
			fsd_free_vector( self->attributes[code] );
		self->attributes[code] = v;  v = NULL;
	 }
	FINALLY
	 { fsd_free_vector( v ); }
	END_TRY
}


static void
fsd_template_destroy( fsd_template_t *self )
{
	unsigned i;
	for( i = 0;  i < self->n_attributes;  i++ )
		if( self->attributes[i] != NULL )
		 {
			const fsd_attribute_t *attr;
			attr = self->by_code( self, i );
			if( attr )
			 {
				if( attr->is_vector )
					fsd_free_vector( self->attributes[i] );
				else
					fsd_free( self->attributes[i] );
			 }
		 }
	fsd_free( self->attributes );
	fsd_free( self );
}


fsd_template_t *
fsd_template_new(
		fsd_template_by_name_method *by_name_method,
		fsd_template_by_code_method *by_code_method,
		unsigned n_attributes
		)
{
	fsd_template_t *volatile self = NULL;
	TRY
	 {
		fsd_malloc( self, fsd_template_t );
		self->attributes = NULL;
		self->n_attributes = 0;
		self->get_attr = fsd_template_get_attr;
		self->set_attr = fsd_template_set_attr;
		self->get_v_attr = fsd_template_get_v_attr;
		self->set_v_attr = fsd_template_set_v_attr;
		self->by_name = by_name_method;
		self->by_code = by_code_method;
		self->destroy = fsd_template_destroy;

		fsd_calloc( self->attributes, n_attributes, void* );
		self->n_attributes = n_attributes;
	 }
	EXCEPT_DEFAULT
	 {
		if( self )
			self->destroy( self );
		fsd_exc_reraise();
	 }
	END_TRY
	return self;
}

