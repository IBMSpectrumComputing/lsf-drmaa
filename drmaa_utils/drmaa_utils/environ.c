/* $Id: environ.c 2 2009-10-12 09:51:22Z mamonski $ */
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

#include <string.h>
#include <stdlib.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/environ.h>
#include <drmaa_utils/lookup3.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: environ.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


static void
fsd_environ_destroy( fsd_environ_t *self )
{
	unsigned i;
	fsd_environ_item_t *j;
	if( self->_table )
	 {
		for( i = 0;  i < self->_table_size;  i++ )
			for( j = self->_table[i];  j != NULL; )
			 {
				fsd_environ_item_t *to_delete = j;
				j = j->next;
				fsd_free( to_delete->name );
				fsd_free( to_delete->value );
				fsd_free( to_delete );
			 }
		fsd_free( self->_table );
	 }
	fsd_free( self );
}

static const char *
fsd_environ_get( fsd_environ_t *self, const char *name )
{
	uint32_t hash;
	const fsd_environ_item_t *i;
	hash = hashstr( name, strlen(name), 0 );
	hash &= self->_table_size - 1;
	for( i = self->_table[hash];  i != NULL;  i = i->next )
		if( strcmp( i->name, name ) == 0 )
			return i->value;
	return NULL;
}

static void
fsd_environ_set( fsd_environ_t *self, char *name, char *value )
{
	uint32_t hash;
	fsd_environ_item_t *i;
	fsd_environ_item_t *item = NULL;
	bool replaced = false;

	TRY
	 {
		hash = hashstr( name, strlen(name), 0 );
		hash &= self->_table_size - 1;
		for( i = self->_table[hash];  i != NULL;  i = i->next )
		 {
			if( strcmp( i->name, name ) == 0 )
			 {
				fsd_free( i->value );
				i->value = value;
				replaced = true;
				break;
			 }
		 }

		if( !replaced )
		 {
			fsd_malloc( item, fsd_environ_item_t );
			item->next = self->_table[hash];
			item->name = name;
			item->value = value;
			self->_table[hash] = item;
		 }
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( name );
		fsd_free( value );
		fsd_exc_reraise();
	 }
	END_TRY
}

static void
fsd_environ_update( fsd_environ_t *self, const char *const envp[] )
{
	const char *const *i;
	for( i = envp;  *i;  i++ )
	 {
		const char *eq;
		char *volatile name = NULL;
		char *volatile value = NULL;
		eq = strchr( *i, '=' );
		if( eq == NULL )
			fsd_exc_raise_code( FSD_ERRNO_INVALID_VALUE_FORMAT );
		name = fsd_strndup( *i, eq-*i );
		TRY
		 { value = fsd_strdup( eq+1 ); }
		EXCEPT_DEFAULT
		 { fsd_free(name); fsd_exc_reraise(); }
		END_TRY
		self->set( self, name, value );
	 }
}

static char **
fsd_environ_list( fsd_environ_t *self )
{
	uint32_t i;
	const fsd_environ_item_t *j;
	char **volatile result = NULL;
	unsigned items = 0;

	TRY
	 {
		for( i = 0;  i < self->_table_size;  i++ )
			for( j = self->_table[i];  j != NULL;  j = j->next )
			 {
				items++;
				fsd_realloc( result, items+1, char* );
				result[items] = NULL;
				result[items-1] = fsd_asprintf( "%s=%s", j->name, j->value );
			 }
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free_vector( result );
		fsd_exc_reraise();
	 }
	END_TRY

	i = 0;
	while (result[i])
	 {
		fsd_log_debug(("enc[%d]=%s",i, result[i]));
		i++;
	 }

	return result;
}

static fsd_environ_t *
fsd_environ_apply( fsd_environ_t *self )
{
	uint32_t i;
	const fsd_environ_item_t *j;
	fsd_environ_t *volatile saved_state =NULL;
	TRY
	 {
		saved_state = fsd_environ_new( NULL );
		for( i = 0;  i < self->_table_size;  i++ )
			for( j = self->_table[i];  j != NULL;  j = j->next )
			 {
				const char *value = getenv( j->name );
				if( value )
					saved_state->set( saved_state,
							fsd_strdup(j->name), fsd_strdup(value) );
				setenv( j->name, j->value, 1 );
			 }
	 }
	EXCEPT_DEFAULT
	 {
		if( saved_state )
			saved_state->destroy( saved_state );
		fsd_exc_reraise();
	 }
	END_TRY
	return saved_state;
}

static void
fsd_environ_restore( fsd_environ_t *self, fsd_environ_t *saved_state )
{
	uint32_t i;
	const fsd_environ_item_t *j;
	for( i = 0;  i < self->_table_size;  i++ )
		for( j = self->_table[i];  j != NULL;  j = j->next )
		 {
			const char *value = saved_state->get( saved_state, j->name );
			if( value )
				setenv( j->name, value, 1 );
			else
				unsetenv( j->name );
		 }
}

fsd_environ_t *
fsd_environ_new( const char *const envp[] )
{
	fsd_environ_t *volatile self = NULL;
	TRY
	 {
		fsd_malloc( self, fsd_environ_t );
		self->destroy = fsd_environ_destroy;
		self->get = fsd_environ_get;
		self->set = fsd_environ_set;
		self->update = fsd_environ_update;
		self->list = fsd_environ_list;
		self->apply = fsd_environ_apply;
		self->restore = fsd_environ_restore;
		self->_table_size = 256;
		fsd_calloc( self->_table, self->_table_size, fsd_environ_item_t* );
		if( envp )
			self->update( self, envp );
	 }
	EXCEPT_DEFAULT
	 {
		if( self )
			self->destroy( self );
	 }
	END_TRY
	return self;
}

