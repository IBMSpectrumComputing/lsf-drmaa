/* $Id: datetime_tab.y 1450 2008-10-01 22:22:59Z lukasz $ */
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
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
#include <drmaa_utils/datetime_impl.h>
%}

/* those are Bison extensions */
%glr-parser
%pure-parser
%name-prefix="fsd_dt_"
%parse-param { fsd_dt_parser_t *parser }
%parse-param { fsd_dt_lexer_t *lexer }
%lex-param { fsd_dt_lexer_t *lexer }

%union {
	fsd_datetime_t datetime;
	int              integer;
}

%type<datetime> datetime date time timezone
%type<integer>  sign
%token<integer> NUM
%token LEXER_ERROR

%%

start : datetime { parser->result = $1; }
	;

datetime
	: time timezone {
		$$.mask = $1.mask | $2.mask;
		$$.year = 0;  $$.month = 0;  $$.day = 0;
		$$.hour   = $1.hour;
		$$.minute = $1.minute;
		$$.second = $1.second;
		$$.tz_delta = $2.tz_delta;
	}
	| date opt_sep time timezone {
		$$.mask = $1.mask | $3.mask | $4.mask;
		$$.year   = $1.year;
		$$.month  = $1.month;
		$$.day    = $1.day;
		$$.hour   = $3.hour;
		$$.minute = $3.minute;
		$$.second = $3.second;
		$$.tz_delta = $4.tz_delta;
	}
	;

opt_sep
	:
	| 'T'
	;

date
	: NUM '-' NUM '-' NUM  { $$.year=$1;  $$.month=$3;  $$.day=$5;
	                         $$.mask=FSD_DT_YEAR|FSD_DT_MONTH|FSD_DT_DAY; }
	| NUM '-' NUM          { $$.year=0;   $$.month=$1;  $$.day=$3;
	                         $$.mask=FSD_DT_MONTH|FSD_DT_DAY; }
	| NUM '/' NUM '/' NUM  { $$.year=$1;  $$.month=$3;  $$.day=$5;
	                         $$.mask=FSD_DT_YEAR|FSD_DT_MONTH|FSD_DT_DAY; }
	| NUM '/' NUM          { $$.year=0;   $$.month=$1;  $$.day=$3;
	                         $$.mask=FSD_DT_MONTH|FSD_DT_DAY; }
	| NUM                  { $$.year=0;   $$.month=0;   $$.day=$1;
	                         $$.mask=FSD_DT_DAY; }
	;


time
	: NUM ':' NUM          { $$.hour=$1;  $$.minute=$3;  $$.second=0;
	                         $$.mask=FSD_DT_HOUR|FSD_DT_MINUTE; }
	| NUM ':' NUM ':' NUM  { $$.hour=$1;  $$.minute=$3;  $$.second=$5;
	                         $$.mask=FSD_DT_HOUR|FSD_DT_MINUTE|FSD_DT_SECOND; }
	;

timezone
	:                   { $$.tz_delta=0;  $$.mask=0; }
	| sign NUM          { $$.tz_delta=$1*3600*$2;  $$.mask=FSD_DT_TZ_DELTA; }
	| sign NUM ':' NUM  { $$.tz_delta=$1*60*(60*$2+$4);
	                      $$.mask=FSD_DT_TZ_DELTA; }
	| 'Z'               { $$.tz_delta=0;  $$.mask=FSD_DT_TZ_DELTA; }
	;

sign
	:      { $$ = +1; }
	| '+'  { $$ = +1; }
	| '-'  { $$ = -1; }
	;

