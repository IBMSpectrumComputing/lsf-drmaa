/* $Id: conf_tab.y 2172 2009-02-28 14:26:23Z lukasz $ */
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

%{
#ifdef HAVE_MALLOC_H
#       include <malloc.h>
#endif
#include <drmaa_utils/conf_impl.h>
#include <drmaa_utils/conf_tab.h>
%}

%pure-parser
%locations
%name-prefix="fsd_conf_"
%parse-param { fsd_conf_parser_t *parser }
%parse-param { fsd_conf_lexer_t *lexer }
%lex-param { fsd_conf_lexer_t *lexer }


%union {
	int integer;
	char *string;
	fsd_conf_option_t *option;
	fsd_conf_dict_t *dictionary;
	fsd_conf_pair_t pair;
}

%type<option> value
%destructor { fsd_conf_option_destroy($$); } value
%type<dictionary> start conf dict dict_body pair_list
%destructor { fsd_conf_dict_destroy($$); } conf dict dict_body pair_list
%type<pair> pair
%token<integer> INTEGER
%token<string> STRING
%destructor { free($$); } STRING
%token LEXER_ERROR


%%

start
	: conf { parser->result = $1;  $$ = NULL; }
	;

conf
	: dict
	| dict_body
	;

dict
	: '{' dict_body '}' { $$ = $2; }
	;

dict_body
	: pair_list  { $$ = $1; }
	| pair_list ','  { $$ = $1; }
	| {
		$$ = fsd_conf_dict_create_noraise();
		if( $$ == NULL )  YYABORT;
	}
	;

pair_list
	: pair
		 {
			fsd_conf_dict_t *dict = NULL;
			int rc;
			dict = fsd_conf_dict_create_noraise();
			if( dict == NULL )
				YYABORT;
			rc = fsd_conf_dict_set_noraise( dict, $1.key, $1.value );
			if( rc )
			 {
				fsd_conf_dict_destroy( dict );
				YYABORT;
			 }
			$$ = dict;
		 }
	| pair_list ',' pair
		 {
			int rc;
			rc = fsd_conf_dict_set_noraise( $1, $3.key, $3.value );
			if( rc )
				YYABORT;
			$$ = $1; /* explicit no op */
		 }
	;

pair
	: STRING ':' value { $$.key = $1;  $$.value = $3; }
	;

value
	: INTEGER
		 {
			fsd_conf_option_t *o;
			o = fsd_conf_option_create_noraise( FSD_CONF_INTEGER, &$1 );
			if( o )
				$$ = o;
			else
				YYABORT;
		 }
	| STRING
		 {
			fsd_conf_option_t *o;
			o = fsd_conf_option_create_noraise( FSD_CONF_STRING, $1 );
			if( o )
				$$ = o;
			else
				YYABORT;
		 }
	| dict
		 {
			fsd_conf_option_t *o;
			o = fsd_conf_option_create_noraise( FSD_CONF_DICT, $1 );
			if( o )
				$$ = o;
			else
				YYABORT;
		 }
	;

%%
