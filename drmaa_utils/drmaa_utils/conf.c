/* $Id: conf.c 226 2010-05-25 10:47:18Z mamonski $ */
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

#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/conf_impl.h>
#include <drmaa_utils/conf_tab.h>
#include <drmaa_utils/logging.h>
#include <drmaa_utils/util.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: conf.c 226 2010-05-25 10:47:18Z mamonski $";
#endif


fsd_conf_dict_t *
fsd_conf_read(
	        fsd_conf_dict_t  * volatile configuration,
		const char *filename, bool must_exist,
		const char *content, size_t content_len
		)
{
	fsd_conf_dict_t *volatile result = NULL;
	fsd_conf_parser_t *parser = NULL;
	fsd_conf_lexer_t *lexer = NULL;
	char *file_content = NULL;
	size_t file_content_len = 0;

	fsd_log_enter((
		"(filename=%s, must_exist=%s, content=%s)",
		filename, must_exist ? "true" : "false", content
		));

	TRY
	 {
		fsd_malloc( parser, fsd_conf_parser_t );
		fsd_malloc( lexer, fsd_conf_lexer_t );

		parser->lexer = lexer;
		parser->result = NULL;
		parser->n_errors = 0;
		parser->errors = NULL;

		lexer->parser = parser;
		lexer->filename = filename;
		lexer->buffer = NULL;
		lexer->buflen = 0;
		lexer->pos = NULL;
		lexer->lineno = 0;
		lexer->cline = NULL;

		if( filename )
			TRY
			 {
				fsd_read_file(
						filename, must_exist,
						&file_content, &file_content_len
						);
			 }
			EXCEPT_DEFAULT
			 {
				const fsd_exc_t *e = fsd_exc_get();
				if( must_exist )
					fsd_exc_raise_fmt( e->code(e), "%s: %s", filename, e->message(e) );
				else
				 {
					fsd_log_warning(( "%s: %s", filename, e->message(e) ));
				 }
			 }
			END_TRY

		if( file_content )
		 {
			lexer->buffer = (const uchar*)file_content;
			lexer->buflen = file_content_len;
			fsd_log_trace(( "content from file" ));
		 }
		else if( content )
		 {
			lexer->buffer = (const uchar*)content;
			lexer->buflen = content_len;
			fsd_log_trace(( "content from memory" ));
		 }

		if( lexer->buffer )
		 {
			lexer->pos = lexer->cline = lexer->buffer;
			lexer->lineno = 1;

			fsd_conf_parse( parser, lexer );
			result = parser->result;
		 }

		if( parser->n_errors > 0 )
		 {
			fsd_exc_raise(
					fsd_exc_new(
							FSD_ERRNO_INTERNAL_ERROR,
							fsd_explode( (const char*const*)parser->errors, '\n',
								parser->n_errors ),
							true
							)
					);
		 }
	 }
	ELSE
	 {
		configuration = fsd_conf_dict_merge( configuration, result );
		result = NULL;
	 }
	FINALLY
	 {
		if( parser )
		 {
			if( parser->errors )
			 {
				int i;
				for( i=0;  i < parser->n_errors;  i++ )
					fsd_free( parser->errors[i] );
				fsd_free( parser->errors );
			 }
			fsd_free( parser );
		 }
		fsd_free( lexer );
		fsd_free( file_content );
		fsd_conf_dict_destroy( result );
	 }
	END_TRY

	fsd_log_return(( "" ));
	return configuration;
}



int
fsd_conf_lex(
		union YYSTYPE *lvalp, struct YYLTYPE *locp,
		fsd_conf_lexer_t *lexer
		)
{
	const uchar *c = lexer->pos;
	const uchar *end = lexer->buffer + lexer->buflen;
	const char *error = NULL;
	int result;

	while( c<end )
		switch( *c )
		 {
			case '#':  /* a comment */
				while( c<end && *c != '\n' )
					c++;
			case '\n':  /* no break */
				lexer->lineno++;
				lexer->cline = c+1;
			case ' ':  case '\t':  case '\r':  /* no break */
				c++;
				break;
			default:
				goto token_begin;
		 }

token_begin:
	locp->first_line = lexer->lineno;
	locp->first_column = c - lexer->cline + 1;

	if( c == end )
		result = 0;
	else
		switch( *c )
		 {
			case ':':  case ',':  case '{':  case '}':
				result = *c++;
				break;

			case '0':  case '1':  case '2':  case '3':  case '4':
			case '5':  case '6':  case '7':  case '8':  case '9':
			 {
				int v = 0;
				while( c < end  &&  '0' <= *c  &&  *c <= '9' )
				 {
					v *= 10;
					v += *c - '0';
					c++;
				 }
				lvalp->integer = v;
				result = INTEGER;
				break;
			 }

			case '"':  case '\'':
			 {
				uchar delimiter;
				const uchar *begin;
				delimiter = *c++;
				begin = c;
                                
				while( c < end  &&  *c != delimiter )
					c++;
				if( c == end )
				 {
					error = "expected string delimiter but EOF found";
					result = LEXER_ERROR;
				 }
				else
				 {
					lvalp->string = fsd_strndup( (const char*)begin, c-begin );
					result = STRING;
					c++;
				 }
				break;
			 }

			default:
			 {
				const uchar *begin = c;
				while( c<end  &&  !isspace(*c) )
					switch( *c )
					 {
						case ':':  case ',':  case '{':  case '}':
							goto end_of_string;
						default:
							c++;
							break;
					 }
			end_of_string:
				lvalp->string = fsd_strndup( (const char*)begin, c-begin );
				result = STRING;
				break;
			 }
		 }

	locp->last_line = lexer->lineno;
	locp->last_column = c - lexer->cline;
	if( locp->last_column < locp->first_column )
		locp->last_column = locp->first_column;
	lexer->pos = c;

	if( error )
		fsd_conf_error( locp, lexer->parser, lexer, error );

	return result;
}



void
fsd_conf_error(
		struct YYLTYPE *locp,
		fsd_conf_parser_t *parser, fsd_conf_lexer_t *lexer,
		const char *fmt, ...
		)
{
	char *volatile message = NULL;
	va_list args;

	TRY
	 {
		va_start( args, fmt );
		message = fsd_vasprintf( fmt, args );
		va_end( args );

		fsd_realloc( parser->errors, parser->n_errors+1, char* );
		parser->errors[ parser->n_errors ] = fsd_asprintf(
				"%s:%d:%d: %s",
				parser->lexer->filename, locp->first_line, locp->first_column,
				message );
		fsd_log_error(("Parser error: %s:%d:%d: %s", parser->lexer->filename, locp->first_line, locp->first_column, message ));
		parser->n_errors ++;
	 }
	FINALLY
	 { fsd_free( message ); }
	END_TRY
}



fsd_conf_option_t *
fsd_conf_option_create( fsd_conf_type_t type, void *value )
{
	fsd_conf_option_t *o = NULL;

	fsd_malloc( o, fsd_conf_option_t );
	o->type = type;
	switch( type )
	 {
		case FSD_CONF_INTEGER:
			o->val.integer = *(int*)value;
			break;
		case FSD_CONF_STRING:
			o->val.string = (char*)value;
			break;
		case FSD_CONF_DICT:
			o->val.dict = (fsd_conf_dict_t*)value;
			break;
		default:
			fsd_assert(false);
			break;
	 }
	return o;
}


void
fsd_conf_option_destroy( fsd_conf_option_t *option )
{
	if( option == NULL )
		return;
	switch( option->type )
	 {
		case FSD_CONF_INTEGER:
			break;
		case FSD_CONF_STRING:
			fsd_free( option->val.string );
			break;
		case FSD_CONF_DICT:
			fsd_conf_dict_destroy( option->val.dict );
			break;
		default:
			fsd_assert( false );
	 }
	fsd_free( option );
}


fsd_conf_option_t *
fsd_conf_option_merge( fsd_conf_option_t *lhs, fsd_conf_option_t *rhs )
{
	if( lhs->type == rhs->type  &&  rhs->type == FSD_CONF_DICT )
	 {
		lhs->val.dict = fsd_conf_dict_merge( lhs->val.dict, rhs->val.dict );
		fsd_free( rhs );
		return lhs;
	 }
	else
	 {
		fsd_conf_option_destroy( lhs );
		return rhs;
	 }
}


void
fsd_conf_option_dump( fsd_conf_option_t *option )
{
	if( option == NULL )
	 {
		printf( "(null)" );
		return;
	 }
	switch( option->type )
	 {
		case FSD_CONF_STRING:
			printf( "\"%s\"", option->val.string );
			break;
		case FSD_CONF_INTEGER:
			printf( "%d", option->val.integer );
			break;
		case FSD_CONF_DICT:
			fsd_conf_dict_dump( option->val.dict );
			break;
	 }
}



struct fsd_conf_dict_s {
	fsd_conf_dict_t *next;
	char *key;
	fsd_conf_option_t *value;
};


fsd_conf_dict_t *
fsd_conf_dict_create(void)
{
	fsd_conf_dict_t *dict = NULL;
	fsd_malloc( dict, fsd_conf_dict_t );
	dict->next = NULL;
	dict->key = NULL;
	dict->value = NULL;
	return dict;
}


void
fsd_conf_dict_destroy( fsd_conf_dict_t *dict )
{
	fsd_conf_dict_t *i;
	for( i = dict;  i != NULL;  )
	 {
		fsd_conf_dict_t *c = i;
		i = i->next;
		fsd_free( c->key );
		fsd_conf_option_destroy( c->value );
		fsd_free( c );
	 }
}


fsd_conf_option_t *
fsd_conf_dict_get(
		fsd_conf_dict_t *dict, const char *key )
{
	fsd_conf_dict_t *i;
	if( dict == NULL  ||  key == NULL )
		return NULL;
	for( i = dict->next;  i != NULL;  i = i->next )
	 {
		if( !strcmp( i->key, key ) )
			return i->value;
	 }
	return NULL;
}


void
fsd_conf_dict_set(
		fsd_conf_dict_t *dict, const char *key, fsd_conf_option_t *value
		)
{
	fsd_conf_dict_t *i;
	for( i = dict->next;  i != NULL;  i = i->next )
	 {
		if( !strcmp( i->key, key ) )
			break;
	 }

	if( i != NULL )
	 {
		fsd_conf_option_destroy( i->value );
		i->value = value;
	 }
	else
	 {
		fsd_conf_dict_t* volatile n = NULL;
		TRY
		 {
			fsd_malloc( n, fsd_conf_dict_t );
			n->key = fsd_strdup( key );
			n->value = value;
		 }
		EXCEPT_DEFAULT
		 {
			fsd_free( n );
			fsd_exc_reraise();
		 }
		ELSE
		 {
			n->next = dict->next;
			dict->next = n;
		 }
		END_TRY
	 }
}


fsd_conf_dict_t *
fsd_conf_dict_merge( fsd_conf_dict_t *lhs, fsd_conf_dict_t *rhs )
{
	fsd_conf_dict_t *i, *j;

	if( lhs == NULL )
		return rhs;

	if( rhs == NULL )
		return lhs;

	for( j = rhs->next;  j != NULL;  )
	 {
		fsd_conf_dict_t *r = j;
		j = j->next;

		for( i = lhs->next;  i != NULL;  i = i->next )
			if( !strcmp( i->key, r->key ) )
				break;

		if( i != NULL )
		 {
			i->value = fsd_conf_option_merge( i->value, r->value );
			fsd_free( r->key );
			fsd_free( r );
		 }
		else
		 {
			r->next = lhs->next;
			lhs->next = r;
		 }
	 }

	fsd_free( rhs );
	return lhs;
}


void
fsd_conf_dict_dump( fsd_conf_dict_t *dict )
{
	fsd_conf_dict_t *i;

	if( dict == NULL )
	 {
		printf( "(null)" );
		return;
	 }
	printf( "{" );
	for( i=dict->next;  i;  i = i->next )
	 {
		if( i != dict->next )
			printf( "," );
		printf( " %s=", i->key );
		fsd_conf_option_dump( i->value );
	 }
	printf( " }" );
}


fsd_conf_option_t *
fsd_conf_option_create_noraise( fsd_conf_type_t type, void *value )
{
	fsd_conf_option_t *volatile result = NULL;
	TRY{ result = fsd_conf_option_create( type, value ); }
	EXCEPT_DEFAULT{}
	END_TRY
	return result;
}

fsd_conf_dict_t *
fsd_conf_dict_create_noraise(void)
{
	fsd_conf_dict_t *volatile result = NULL;
	TRY{ result = fsd_conf_dict_create(); }
	EXCEPT_DEFAULT{}
	END_TRY
	return result;
}

int
fsd_conf_dict_set_noraise(
		fsd_conf_dict_t *dict, const char *key, fsd_conf_option_t *value )
{
	volatile int rc = FSD_ERRNO_SUCCESS;
	TRY{ fsd_conf_dict_set( dict, key, value ); }
	EXCEPT_DEFAULT
	 {
		const fsd_exc_t *e = fsd_exc_get();
		rc = e->code(e);
	 }
	END_TRY
	return rc;
}

