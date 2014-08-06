/* $Id: iter.c 2 2009-10-12 09:51:22Z mamonski $ */
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


#include <drmaa_utils/iter.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: iter.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


static const char *
fsd_iter_next( fsd_iter_t *self )
{
	if( self->_position < self->_length )
		return self->_list[ self->_position++ ];
	else
		fsd_exc_raise_code( FSD_ERRNO_STOP_ITERATION );
}

static void
fsd_iter_reset( fsd_iter_t *self )
{
	self->_length = 0;
}

static int
fsd_iter_len( fsd_iter_t *self )
{
	return self->_length;
}

static void
fsd_iter_append( fsd_iter_t *self, char *string )
{
	TRY
	 {
		if( !self->_own_list )
			fsd_exc_raise_msg(
					FSD_ERRNO_INTERNAL_ERROR,
					"iter::append called on constant list" );
		fsd_realloc( self->_list, self->_length+1, char* );
		self->_list[ self->_length++ ] = string;
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( string );
		fsd_exc_reraise();
	 }
	END_TRY
}

static void
fsd_iter_destroy( fsd_iter_t *self )
{
	int i;
	if( self->_own_list  &&  self->_list )
	 {
		for( i = 0;  i < self->_length;  i++ )
			fsd_free( self->_list[i] );
		fsd_free( self->_list );
	 }
	fsd_free( self );
}

static fsd_iter_t *
fsd_iter_new_impl( char **list, int length, bool own )
{
	fsd_iter_t *volatile self = NULL;
	TRY
	 {
		fsd_malloc( self, fsd_iter_t );
		self->next = fsd_iter_next;
		self->reset = fsd_iter_reset;
		self->len = fsd_iter_len;
		self->append = fsd_iter_append;
		self->destroy = fsd_iter_destroy;
		self->_list = list;
		self->_position = 0;
		if( list == NULL )
			self->_length = 0;
		else if( length >= 0 )
			self->_length = length;
		else
		 {
			char **i;
			int cnt = 0;
			for( i = self->_list;  *i != NULL;  i++ )
				cnt++;
			self->_length = cnt;
		 }
		self->_own_list = own;
	 }
	EXCEPT_DEFAULT
	 {
		if( own  &&  list )
		 {
			if( length >= 0 )
			 {
				int i;
				for( i = 0;  i < length;  i++ )
					fsd_free( list[i] );
				fsd_free( list );
			 }
			else
				fsd_free_vector( list );
		 }
		fsd_exc_reraise();
	 }
	END_TRY
	return self;
}

fsd_iter_t *
fsd_iter_new( char **list, int length )
{
	return fsd_iter_new_impl( list, length, true );
}

fsd_iter_t *
fsd_iter_new_const( const char *const *list, int length )
{
	return fsd_iter_new_impl( (char**)list, length, false );
}

