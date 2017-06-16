
#line 1 "timedelta.rl"
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
	
	#line 52 "timedelta.rl"
	
	
	
	static const char _fsd_timedelta_actions[] = {
		0, 1, 0, 1, 1, 1, 3, 2,
		2, 3, 0
	};
	
	static const char _fsd_timedelta_key_offsets[] = {
		0, 0, 2, 5, 7, 9, 12, 0
	};
	
	static const char _fsd_timedelta_trans_keys[] = {
		48, 57, 58, 48, 57, 48, 57, 48,
		57, 58, 48, 57, 48, 57, 0
	};
	
	static const char _fsd_timedelta_single_lengths[] = {
		0, 0, 1, 0, 0, 1, 0, 0
	};
	
	static const char _fsd_timedelta_range_lengths[] = {
		0, 1, 1, 1, 1, 1, 1, 0
	};
	
	static const char _fsd_timedelta_index_offsets[] = {
		0, 0, 2, 5, 7, 9, 12, 0
	};
	
	static const char _fsd_timedelta_trans_cond_spaces[] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, 0
	};
	
	static const char _fsd_timedelta_trans_offsets[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 0
	};
	
	static const char _fsd_timedelta_trans_lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 0
	};
	
	static const char _fsd_timedelta_cond_keys[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0
	};
	
	static const char _fsd_timedelta_cond_targs[] = {
		2, 0, 3, 2, 0, 5, 0, 6,
		0, 4, 5, 0, 6, 0, 0
	};
	
	static const char _fsd_timedelta_cond_actions[] = {
		7, 1, 3, 5, 1, 7, 1, 7,
		1, 3, 5, 1, 5, 1, 0
	};
	
	static const char _fsd_timedelta_eof_actions[] = {
		0, 1, 1, 1, 1, 3, 3, 0
	};
	
	static const char _fsd_timedelta_nfa_targs[] = {
		0, 0
	};
	
	static const char _fsd_timedelta_nfa_offsets[] = {
		0, 0, 0, 0, 0, 0, 0, 0
	};
	
	static const char _fsd_timedelta_nfa_push_actions[] = {
		0, 0
	};
	
	static const char _fsd_timedelta_nfa_pop_trans[] = {
		0, 0
	};
	
	static const int fsd_timedelta_start = 1;
	static const int fsd_timedelta_first_final = 5;
	static const int fsd_timedelta_error = 0;
	
	static const int fsd_timedelta_en_main = 1;
	
	
	#line 55 "timedelta.rl"
	
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
	
	{
		cs = (int)fsd_timedelta_start;
	}
	
	#line 68 "timedelta.rl"
	
	
	{
		int _klen;
		unsigned int _trans = 0;
		unsigned int _cond = 0;
		const char *_acts;
		unsigned int _nacts;
		const char *_keys;
		const char *_ckeys;
		int _cpc;
		{
			
			if ( p == pe )
			goto _test_eof;
			if ( cs == 0 )
			goto _out;
			_resume:  {
				_keys = ( _fsd_timedelta_trans_keys + (_fsd_timedelta_key_offsets[cs]));
				_trans = (unsigned int)_fsd_timedelta_index_offsets[cs];
				
				_klen = (int)_fsd_timedelta_single_lengths[cs];
				if ( _klen > 0 ) {
					const char *_lower;
					const char *_mid;
					const char *_upper;
					_lower = _keys;
					_upper = _keys + _klen - 1;
					while ( 1 ) {
						if ( _upper < _lower )
						break;
						
						_mid = _lower + ((_upper-_lower) >> 1);
						if ( ( (*( p))) < (*( _mid)) )
						_upper = _mid - 1;
						else if ( ( (*( p))) > (*( _mid)) )
						_lower = _mid + 1;
						else {
							_trans += (unsigned int)(_mid - _keys);
							goto _match;
						}
					}
					_keys += _klen;
					_trans += (unsigned int)_klen;
				}
				
				_klen = (int)_fsd_timedelta_range_lengths[cs];
				if ( _klen > 0 ) {
					const char *_lower;
					const char *_mid;
					const char *_upper;
					_lower = _keys;
					_upper = _keys + (_klen<<1) - 2;
					while ( 1 ) {
						if ( _upper < _lower )
						break;
						
						_mid = _lower + (((_upper-_lower) >> 1) & ~1);
						if ( ( (*( p))) < (*( _mid)) )
						_upper = _mid - 2;
						else if ( ( (*( p))) > (*( _mid + 1)) )
						_lower = _mid + 2;
						else {
							_trans += (unsigned int)((_mid - _keys)>>1);
							goto _match;
						}
					}
					_trans += (unsigned int)_klen;
				}
				
			}
			_match:  {
				_ckeys = ( _fsd_timedelta_cond_keys + (_fsd_timedelta_trans_offsets[_trans]));
				_klen = (int)_fsd_timedelta_trans_lengths[_trans];
				_cond = (unsigned int)_fsd_timedelta_trans_offsets[_trans];
				
				_cpc = 0;
				{
					const char *_lower;
					const char *_mid;
					const char *_upper;
					_lower = _ckeys;
					_upper = _ckeys + _klen - 1;
					while ( 1 ) {
						if ( _upper < _lower )
						break;
						
						_mid = _lower + ((_upper-_lower) >> 1);
						if ( _cpc < (int)(*( _mid)) )
						_upper = _mid - 1;
						else if ( _cpc > (int)(*( _mid)) )
						_lower = _mid + 1;
						else {
							_cond += (unsigned int)(_mid - _ckeys);
							goto _match_cond;
						}
					}
					cs = 0;
					goto _again;
				}
			}
			_match_cond:  {
				cs = (int)_fsd_timedelta_cond_targs[_cond];
				
				if ( _fsd_timedelta_cond_actions[_cond] == 0 )
				goto _again;
				
				_acts = ( _fsd_timedelta_actions + (_fsd_timedelta_cond_actions[_cond]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 )
				{
					switch ( (*( _acts)) )
					{
						case 0:  {
							{
								#line 41 "timedelta.rl"
								
								fsd_exc_raise_fmt(
								FSD_ERRNO_INVALID_VALUE_FORMAT,
								"time delta syntax error: %s", string );
							}
							break; }
						case 1:  {
							{
								#line 46 "timedelta.rl"
								
								fields[ n_fields++ ] = v;
							}
							break; }
						case 2:  {
							{
								#line 49 "timedelta.rl"
								v = 0; }
							break; }
						case 3:  {
							{
								#line 49 "timedelta.rl"
								v *= 10;  v += (( (*( p)))) - '0'; }
							break; }
					}
					_nacts -= 1;
					_acts += 1;
				}
				
				
			}
			_again:  {
				if ( cs == 0 )
				goto _out;
				p += 1;
				if ( p != pe )
				goto _resume;
			}
			_test_eof:  { {}
				if ( p == eof )
				{
					const char *__acts;
					unsigned int __nacts;
					__acts = ( _fsd_timedelta_actions + (_fsd_timedelta_eof_actions[cs]));
					__nacts = (unsigned int)(*( __acts));
					__acts += 1;
					while ( __nacts > 0 ) {
						switch ( (*( __acts)) ) {
							case 0:  {
								{
									#line 41 "timedelta.rl"
									
									fsd_exc_raise_fmt(
									FSD_ERRNO_INVALID_VALUE_FORMAT,
									"time delta syntax error: %s", string );
								}
								break; }
							case 1:  {
								{
									#line 46 "timedelta.rl"
									
									fields[ n_fields++ ] = v;
								}
								break; }
						}
						__nacts -= 1;
						__acts += 1;
					}
				}
				
			}
			_out:  { {}
			}
		}
	}
	
	#line 69 "timedelta.rl"
	
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
