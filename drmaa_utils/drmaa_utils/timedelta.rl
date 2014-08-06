/* $Id: timedelta.rl 1452 2008-10-02 12:09:09Z lukasz $ */
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

#include <string.h>

#include <drmaa_utils/common.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: timedelta.rl 1452 2008-10-02 12:09:09Z lukasz $";
#endif

int
fsd_parse_timedelta( const char *string )
{
	%%{
		machine fsd_timedelta;
		action error {
			fsd_exc_raise_fmt(
					FSD_ERRNO_INVALID_VALUE_FORMAT,
					"time delta syntax error: %s", string );
		}
		action next_field {
			fields[ n_fields++ ] = v;
		}
		integer = [0-9]+ >{ v = 0; } ${ v *= 10;  v += fc - '0'; };
		timedelta = integer %next_field (':' integer %next_field){1,2};
		main := timedelta @eof(error) $err(error);
	}%%

	%%write data;
	int v = 0;
	int fields[3];
	int n_fields = 0;
	int timedelta;
	int i;

	const char *p = string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;

	fsd_log_enter(( "(%s)", string ));
	%%write init;
	%%write exec;
	timedelta = 0;
	for( i = 0;  i < n_fields;  i++ )
	 {
		timedelta *= 60;
		timedelta += fields[i];
	 }
	fsd_log_return(( " =%d", timedelta ));
	return timedelta;
}

/* vim: set ft=ragel: */
