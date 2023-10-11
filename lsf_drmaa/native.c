
#line 1 "native.rl"
/* $Id: native.rl 2298 2009-04-09 16:11:25Z lukasz $ */
/*
* FedStage DRMAA for LSF
* Copyright (C) 2007-2008  FedStage Systems
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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/datetime.h>
#include <drmaa_utils/logging.h>
#include <drmaa_utils/util.h>
#include <lsf/lsbatch.h>

#define min(a,b) ((a)<(b) ? (a) : (b))


static void
impl_native_add(
struct submit *req,
bool long_option,
const char *option, size_t optlen,
const char *argument
);

static time_t
lsfdrmaa_parse_datetime( const char *datetime_string );

static int
lsfdrmaa_parse_timedelta( const char *timedelta_string );

static void
lsfdrmaa_parse_asked_hosts(
struct submit *req,
const char *asked_hosts_string
);

static void
lsfdrmaa_parse_file_transfer(
struct xFile *transfer,
const char *transfer_string );


void
lsfdrmaa_native_parse( const char *native_spec, struct submit *req )
{
	
	#line 108 "native.rl"
	
	
	
	static const char _lsf_native_specification_actions[] = {
		0, 1, 0, 1, 1, 1, 2, 1,
		3, 1, 4, 1, 5, 1, 6, 2,
		3, 5, 0
	};
	
	static const short _lsf_native_specification_key_offsets[] = {
		0, 0, 34, 35, 36, 37, 38, 42,
		47, 48, 49, 54, 57, 61, 65, 66,
		71, 72, 76, 77, 82, 83, 87, 88,
		89, 90, 94, 95, 96, 97, 101, 102,
		103, 104, 105, 106, 107, 111, 113, 114,
		119, 123, 128, 131, 136, 0
	};
	
	static const char _lsf_native_specification_trans_keys[] = {
		45, 66, 69, 72, 73, 75, 76, 78,
		80, 85, 87, 90, 97, 98, 99, 101,
		105, 106, 107, 111, 114, 115, 119, 120,
		67, 77, 82, 83, 102, 103, 109, 110,
		113, 118, 97, 114, 105, 100, 32, 61,
		9, 13, 32, 34, 39, 9, 13, 34,
		39, 32, 34, 39, 9, 13, 32, 9,
		13, 32, 112, 9, 13, 32, 101, 9,
		13, 115, 32, 112, 114, 9, 13, 112,
		32, 119, 9, 13, 100, 32, 111, 120,
		9, 13, 116, 32, 115, 9, 13, 99,
		104, 101, 32, 115, 9, 13, 115, 100,
		108, 32, 95, 9, 13, 115, 116, 114,
		105, 99, 116, 32, 111, 9, 13, 108,
		112, 97, 32, 97, 116, 9, 13, 32,
		45, 9, 13, 32, 34, 39, 9, 13,
		32, 9, 13, 32, 112, 115, 9, 13,
		32, 110, 9, 13, 0
	};
	
	static const char _lsf_native_specification_single_lengths[] = {
		0, 24, 1, 1, 1, 1, 2, 3,
		1, 1, 3, 1, 2, 2, 1, 3,
		1, 2, 1, 3, 1, 2, 1, 1,
		1, 2, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 2, 2, 1, 3,
		2, 3, 1, 3, 2, 0
	};
	
	static const char _lsf_native_specification_range_lengths[] = {
		0, 5, 0, 0, 0, 0, 1, 1,
		0, 0, 1, 1, 1, 1, 0, 1,
		0, 1, 0, 1, 0, 1, 0, 0,
		0, 1, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 1, 0, 0, 1,
		1, 1, 1, 1, 1, 0
	};
	
	static const short _lsf_native_specification_index_offsets[] = {
		0, 0, 30, 32, 34, 36, 38, 42,
		47, 49, 51, 56, 59, 63, 67, 69,
		74, 76, 80, 82, 87, 89, 93, 95,
		97, 99, 103, 105, 107, 109, 113, 115,
		117, 119, 121, 123, 125, 129, 132, 134,
		139, 143, 148, 151, 156, 0
	};
	
	static const char _lsf_native_specification_trans_cond_spaces[] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		0
	};
	
	static const short _lsf_native_specification_trans_offsets[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127,
		128, 129, 130, 131, 132, 133, 134, 135,
		136, 137, 138, 139, 140, 141, 142, 143,
		144, 145, 146, 147, 148, 149, 150, 151,
		152, 153, 154, 155, 156, 157, 158, 159,
		0
	};
	
	static const char _lsf_native_specification_trans_lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		0
	};
	
	static const char _lsf_native_specification_cond_keys[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0
	};
	
	static const char _lsf_native_specification_cond_targs[] = {
		2, 42, 12, 42, 43, 42, 12, 42,
		11, 11, 13, 14, 15, 11, 17, 19,
		25, 26, 11, 36, 44, 37, 39, 42,
		11, 11, 11, 11, 11, 0, 3, 0,
		4, 0, 5, 0, 6, 0, 7, 10,
		7, 0, 7, 8, 9, 7, 41, 41,
		8, 41, 9, 0, 8, 9, 0, 41,
		7, 7, 0, 7, 11, 7, 0, 7,
		11, 7, 0, 42, 0, 7, 16, 11,
		7, 0, 11, 0, 7, 18, 7, 0,
		11, 0, 7, 11, 20, 7, 0, 21,
		0, 7, 22, 7, 0, 23, 0, 24,
		0, 18, 0, 7, 11, 7, 0, 27,
		0, 28, 0, 29, 0, 7, 30, 7,
		0, 31, 0, 32, 0, 33, 0, 34,
		0, 35, 0, 11, 0, 7, 11, 7,
		0, 38, 11, 0, 11, 0, 7, 11,
		11, 7, 0, 40, 1, 40, 0, 40,
		8, 9, 40, 41, 40, 40, 0, 40,
		42, 42, 40, 0, 40, 42, 40, 0,
		0
	};
	
	static const char _lsf_native_specification_cond_actions[] = {
		3, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 13, 5, 13,
		0, 13, 0, 13, 0, 13, 7, 7,
		7, 13, 0, 0, 0, 0, 9, 0,
		9, 0, 9, 13, 0, 0, 13, 9,
		7, 7, 13, 7, 0, 7, 13, 7,
		0, 7, 13, 0, 13, 7, 0, 0,
		7, 13, 0, 13, 7, 0, 7, 13,
		0, 13, 7, 0, 0, 7, 13, 0,
		13, 7, 0, 7, 13, 0, 13, 0,
		13, 0, 13, 7, 0, 7, 13, 0,
		13, 0, 13, 0, 13, 7, 0, 7,
		13, 0, 13, 0, 13, 0, 13, 0,
		13, 0, 13, 0, 13, 7, 0, 7,
		13, 0, 0, 13, 0, 13, 7, 0,
		0, 7, 13, 0, 1, 0, 13, 11,
		0, 0, 11, 9, 15, 15, 13, 15,
		0, 0, 15, 13, 15, 0, 15, 13,
		0
	};
	
	static const char _lsf_native_specification_eof_actions[] = {
		0, 13, 13, 13, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 13,
		0, 11, 15, 15, 15, 0
	};
	
	static const char _lsf_native_specification_nfa_targs[] = {
		0, 0
	};
	
	static const char _lsf_native_specification_nfa_offsets[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0
	};
	
	static const char _lsf_native_specification_nfa_push_actions[] = {
		0, 0
	};
	
	static const char _lsf_native_specification_nfa_pop_trans[] = {
		0, 0
	};
	
	static const int lsf_native_specification_start = 40;
	static const int lsf_native_specification_first_final = 40;
	static const int lsf_native_specification_error = 0;
	
	static const int lsf_native_specification_en_main = 40;
	
	
	#line 111 "native.rl"
	
	char *volatile arg = NULL;
	size_t arglen = 0;
	/* size_t argsize = 0; */
	enum{ OPT_SHORT, OPT_LONG } opt_type;
	const char *opt_begin;
	const char *opt_end;
	
	const char *p = native_spec;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;
	
	TRY
	{
		fsd_calloc( arg, strlen(native_spec)+1, char );
		
		{
			cs = (int)lsf_native_specification_start;
		}
		
		#line 127 "native.rl"
		
		
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
					_keys = ( _lsf_native_specification_trans_keys + (_lsf_native_specification_key_offsets[cs]));
					_trans = (unsigned int)_lsf_native_specification_index_offsets[cs];
					
					_klen = (int)_lsf_native_specification_single_lengths[cs];
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
					
					_klen = (int)_lsf_native_specification_range_lengths[cs];
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
					_ckeys = ( _lsf_native_specification_cond_keys + (_lsf_native_specification_trans_offsets[_trans]));
					_klen = (int)_lsf_native_specification_trans_lengths[_trans];
					_cond = (unsigned int)_lsf_native_specification_trans_offsets[_trans];
					
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
					cs = (int)_lsf_native_specification_cond_targs[_cond];
					
					if ( _lsf_native_specification_cond_actions[_cond] == 0 )
					goto _again;
					
					_acts = ( _lsf_native_specification_actions + (_lsf_native_specification_cond_actions[_cond]));
					_nacts = (unsigned int)(*( _acts));
					_acts += 1;
					while ( _nacts > 0 )
					{
						switch ( (*( _acts)) )
						{
							case 0:  {
								{
									#line 66 "native.rl"
									opt_type = OPT_SHORT; }
								break; }
							case 1:  {
								{
									#line 67 "native.rl"
									opt_type = OPT_LONG; }
								break; }
							case 2:  {
								{
									#line 68 "native.rl"
									opt_begin = p; }
								break; }
							case 3:  {
								{
									#line 69 "native.rl"
									opt_end = p; }
								break; }
							case 4:  {
								{
									#line 70 "native.rl"
									arg[arglen++] = (( (*( p)))); }
								break; }
							case 5:  {
								{
									#line 71 "native.rl"
									
									arg[arglen] = '\0';
									impl_native_add(
									req, (opt_type == OPT_LONG),
									opt_begin, opt_end-opt_begin,
									arg
									);
									fsd_log_debug(( "native parse: %s%.*s '%.*s'",
									(opt_type == OPT_SHORT ? "-" : "--"),
									(int)(opt_end - opt_begin), opt_begin,
									(int)arglen, arg
									));
									arglen = 0;
								}
								break; }
							case 6:  {
								{
									#line 85 "native.rl"
									
									fsd_exc_raise_fmt(
									FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
									"Error in native specification: %s",
									native_spec );
								}
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
						__acts = ( _lsf_native_specification_actions + (_lsf_native_specification_eof_actions[cs]));
						__nacts = (unsigned int)(*( __acts));
						__acts += 1;
						while ( __nacts > 0 ) {
							switch ( (*( __acts)) ) {
								case 3:  {
									{
										#line 69 "native.rl"
										opt_end = p; }
									break; }
								case 5:  {
									{
										#line 71 "native.rl"
										
										arg[arglen] = '\0';
										impl_native_add(
										req, (opt_type == OPT_LONG),
										opt_begin, opt_end-opt_begin,
										arg
										);
										fsd_log_debug(( "native parse: %s%.*s '%.*s'",
										(opt_type == OPT_SHORT ? "-" : "--"),
										(int)(opt_end - opt_begin), opt_begin,
										(int)arglen, arg
										));
										arglen = 0;
									}
									break; }
								case 6:  {
									{
										#line 85 "native.rl"
										
										fsd_exc_raise_fmt(
										FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
										"Error in native specification: %s",
										native_spec );
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
		
		#line 128 "native.rl"
		
	}
	FINALLY
	{
		fsd_free( arg );
	}
	END_TRY
	
	fsd_log_debug(( "characters parsed=%ld, left=%ld\n",
	(long)(p-native_spec), (long)(pe-p) ));
}


static void
impl_native_add(
struct submit *req,
bool long_option,
const char *option, size_t optlen,
const char *argument
)
{
	#define ASSIGN_ARG( dest ) \
	do { \
		if( dest != NULL )  fsd_free(dest);  \
		(dest) = fsd_strdup(argument); \
	} while(0)
	#define ASSIGN_INTARG( dest ) \
	do { \
		(dest) = fsd_atoi(argument); \
	} while(0)
	
	if( long_option )
	{
		if( optlen==4  &&  0==strncmp(option, "arid", 4) )
		goto reservation_id;
		else
		goto unimplemented_option;
	}
	else
	{
		switch( option[0] )
		{
			case 'B':  /* -B: Sends mail to you when the job
			is dispatched and begins execution. */
			req->options |= SUB_NOTIFY_BEGIN;
			break;
			
			case 'H':  /* -H: Holds the job in the PSUSP state
			when the job is submitted. */
			req->options2 |= SUB2_HOLD;
			break;
			
			case 'I':  /* -I[ps]: Interactive job */
			goto unimplemented_option;
			
			case 'K':  /* -K: Submits a batch job and waits
			for the job to complete. */
			goto unimplemented_option;
			
			case 'N':  /* -N: Sends the job report to you by mail
			when the job finishes. */
			req->options |= SUB_NOTIFY_END;
			break;
			
			case 'r':  /* -r -rn: Job is (not) rerunnable. */
			if( optlen == 1 ) /* -r: rerunnable */
			{
				req->options |= SUB_RERUNNABLE;
				#ifdef SUB3_NOT_RERUNNABLE
				req->options3 &= ~SUB3_NOT_RERUNNABLE;
				#endif
			}
			else /* -rn: not rerunnable */
			{
				req->options &= ~SUB_RERUNNABLE;
				#ifdef SUB3_NOT_RERUNNABLE
				req->options3 |= SUB3_NOT_RERUNNABLE;
				#else
				goto unimplemented_option;
				#endif
			}
			break;
			
			case 'x': /* -x: Puts the host running your job
			into exclusive execution mode. */
			req->options |= SUB_EXCLUSIVE;
			break;
			
			case 'a': /* -a esub_application
			-app application_profile_name
			-ar reservation_id (SGE like)
			*/
			switch( optlen )
			{
				case 1: /* -a */
				goto unimplemented_option; /* ?? TODO */
				case 2: /* -ar */
				goto reservation_id;
				case 3: /* -app */
				#ifdef SUB3_APP
				req->options3 |= SUB3_APP;
				ASSIGN_ARG( req->app );
				#else
				goto unimplemented_option;
				#endif
				break;
			}
			break;
			
			case 'b':
			/*
			* -b [[month:]day:]hour:minute
			*   Dispatches the job for execution on or after
			*   the specified date and time.
			*/
			req->beginTime = lsfdrmaa_parse_datetime( argument );
			break;
			
			case 'C':  /* -C core_limit: Core limit in KiB */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_CORE] );
			break;
			
			case 'c':
			/*
			* -c [hour:]minute[/host_name | /host_model]
			*   cpu time limit
			* -cwd working_directory
			*/
			if( optlen == 1 )
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_CPU] );
			else if( optlen == 3 )
			{
				#ifdef SUB3_CWD
				req->options3 |= SUB3_CWD;
				ASSIGN_ARG( req->cwd );
				#else
				goto unimplemented_option;
				#endif
			}
			break;
			
			case 'D':  /* -D data_limit */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_DATA] );
			break;
			
			case 'E':  /* -E pre_exec_command
			-Ep post_exec_command */
			if( optlen == 1 )
			{
				req->options |= SUB_PRE_EXEC;
				ASSIGN_ARG( req->preExecCmd );
			}
			else
			{
				#ifdef SUB3_POST_EXEC
				req->options3 |= SUB3_POST_EXEC;
				ASSIGN_ARG( req->postExecCmd );
				#else
				goto unimplemented_option;
				#endif
			}
			break;
			
			case 'e':  /* -e[o] error_file
			-ext[sched] external_scheaduler_options */
			if( optlen <= 2 )
			{
				ASSIGN_ARG( req->errFile );
				req->options |= SUB_ERR_FILE;
				if( optlen == 2 )
				{
					#ifdef SUB2_OVERWRITE_OUT_FILE
					req->options2 |= SUB2_OVERWRITE_ERR_FILE;
					#else
					goto unimplemented_option;
					#endif
				}
			}
			else
			{
				req->options2 |= SUB2_EXTSCHED;
				ASSIGN_ARG( req->extsched );
			}
			break;
			
			case 'F':  /* -F file_limit */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_FSIZE] );
			break;
			
			case 'f':  /* -f local_file operator [remote_file] */
			{
				struct xFile *t;
				fsd_realloc( req->xf, req->nxf+1, struct xFile );
				t = &req->xf[ req->nxf++ ];
				lsfdrmaa_parse_file_transfer( t, argument );
				break;
			}
			
			case 'G':  /* -G user_group */
			req->options |= SUB_USER_GROUP;
			ASSIGN_ARG( req->userGroup );
			break;
			
			case 'g':  /* -g job_group_name */
			req->options |= SUB2_JOB_GROUP;
			ASSIGN_ARG( req->jobGroup );
			break;
			
			case 'i':  /* -i[s] input_file */
			req->options |= SUB_IN_FILE;
			ASSIGN_ARG( req->inFile );
			if( optlen == 2 )
			req->options2 |= SUB2_IN_FILE_SPOOL;
			break;
			
			case 'J':  /* -J job_name */
			req->options |= SUB_JOB_NAME;
			ASSIGN_ARG( req->jobName );
			break;
			
			case 'j':  /* -jsdl[_strict] file_name */
			#if LSF_PRODUCT_MAJOR_VERSION >= 7  /* FIXME */
			ASSIGN_ARG( req->jsdlDoc );
			if( optlen == 4 )
			req->jsdlFlag = 1;
			else
			req->jsdlFlag = 0;
			#else
			goto unimplemented_option;
			#endif
			break;
			
			case 'k':  /* -k checkpoint_dir */
			req->options |= SUB_CHKPNT_DIR;
			ASSIGN_ARG( req->chkpntDir );
			break;
			
			case 'L':  /* -L login_shell
			-Lp ls_project_name */
			if( optlen == 1 )
			{
				req->options |= SUB_LOGIN_SHELL;
				ASSIGN_ARG( req->loginShell );
			}
			else
			{
				#ifdef SUB2_LICENSE_PROJECT
				req->options2 |= SUB2_LICENSE_PROJECT;
				ASSIGN_ARG( req->licenseProject );
				#else
				goto unimplemented_option;
				#endif
			}
			break;
			
			case 'M':  /* -M memory_limit: Memory limit in KiB*/
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_RSS] );
			break;
			
			case 'm':  /* -m host_name...: runs job on specified hosts */
			{
				lsfdrmaa_parse_asked_hosts( req, argument );
				break;
			}
			
			case 'n':  /* -n min_proc[,max_proc] */
			{
				const char *p = argument, *end;
				int value;
				value = strtol( p, (char**)&end, 10 );
				if( p < end  &&  (end[0] == '\0' || end[0] == ',') )
				{
					req->numProcessors = value;
					p = end;
					if( p[0] == ',' )
					p++;
				}
				else
				fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
				"Invalid argument to -n option in native specification: %s",
				argument );
				
				value = strtol( p, (char**)&end, 10 );
				if( p < end  &&  end[0] == '\0' )
				req->maxNumProcessors = value;
				else if( p[-1] != ','  &&  end[0] == '\0' )
				req->maxNumProcessors = req->numProcessors;
				else
				fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
				"Invalid argument to -n option in native specification: %s",
				argument );
				break;
			}
			
			case 'o':  /* -o[o] output_file */
			req->options |= SUB_OUT_FILE;
			ASSIGN_ARG( req->outFile );
			if( optlen==2 )
			{
				#ifdef SUB2_OVERWRITE_ERR_FILE
				req->options2 |= SUB2_OVERWRITE_OUT_FILE;
				#else
				goto unimplemented_option;
				#endif
			}
			break;
			
			case 'P':  /* -P project_name */
			req->options |= SUB_PROJECT_NAME;
			ASSIGN_ARG( req->projectName );
			break;
			
			case 'p':  /* -p process_limit */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_PROCESS] );
			break;
			
			case 'q':  /* -q queue_name */
			req->options |= SUB_QUEUE;
			ASSIGN_ARG( req->queue );
			break;
			
			case 'R':  /* -R res_req */
			req->options |= SUB_RES_REQ;
			ASSIGN_ARG( req->resReq );
			break;
			
			case 'S':  /* -S stack_limit: stack limit in KiB */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_STACK] );
			break;
			
			case 's':
			/* -s signal: sends signal when queue run window closes
			* -sla service_class_name
			* -sp priority
			*/
			if( optlen == 1 )
			{
				req->sigValue = SIGTERM; /* TODO */
				req->options = SUB_WINDOW_SIG;
			}
			else if( optlen == 3 )
			{
				req->options2 |= SUB2_SLA;
				ASSIGN_ARG( req->sla );
			}
			else if( optlen == 2 ) /* -sp */
			{
				req->options2 |= SUB2_JOB_PRIORITY;
				ASSIGN_INTARG( req->userPriority );
			}
			break;
			
			case 'T':  /* -T thread_limit */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_THREAD] );
			break;
			
			case 't':  /* -t [[month:]day:]hour:minute
			Specifies the job termination deadline. */
			{
				req->termTime = lsfdrmaa_parse_datetime( argument );
				break;
			}
			
			case 'U':  /* -U reservation_id */
			reservation_id:
			req->options2 |= SUB2_USE_RSV;
			ASSIGN_ARG( req->rsvId );
			break;
			
			case 'u':
			/* -ul
			*   Passes the current operating system user shell
			*   limits for the job submission user to the
			*   execution host.
			* -u mail_user
			*/
			if( optlen == 2 )
			{
				#ifdef SUB3_USER_SHELL_LIMITS
				req->options3 |= SUB3_USER_SHELL_LIMITS;
				#else
				goto unimplemented_option;
				#endif
			}
			else
			{
				req->options |= SUB_MAIL_USER;
				ASSIGN_ARG( req->mailUser );
			}
			break;
			
			case 'v':  /* -v swap_limt: Swap limit in KiB */
			ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_SWAP] );
			break;
			
			case 'W':
			/*
			* -W [hour:]minute[/host_name | /host_model]
			*   Sets the runtime limit of the batch job.
			* -We [hour:]minute[/host_name | /host_model]
			*   Specifies an estimated run time for the job.
			*/
			if( optlen==1 )
			req->rLimits[LSF_RLIMIT_RUN] = lsfdrmaa_parse_timedelta( argument );
			else
			{
				#ifdef SUB3_RUNTIME_ESTIMATION
				req->options3 = SUB3_RUNTIME_ESTIMATION;
				req->runtimeEstimation = lsfdrmaa_parse_timedelta( argument );
				#else
				goto unimplemented_option;
				#endif
			}
			break;
			
			case 'w':
			/*
			* -w 'dependency_expression'
			* -wa ’[signal | command | CHKPNT]’
			*   Specifies the job action to be taken before a job control
			*   action occurs.
			* -wt ’[hour:]minute’
			*   Specifies the amount of time before a job control action
			*   occurs that a job warning action is to be taken.
			*/
			if( optlen == 1 )
			{
				req->options |= SUB_DEPEND_COND;
				ASSIGN_ARG( req->dependCond );
			}
			else if( option[1] == 'a' )
			{
				req->options2 |= SUB2_WARNING_ACTION;
				ASSIGN_ARG( req->warningAction );
			}
			else
			{
				req->options2 |= SUB2_WARNING_TIME_PERIOD;
				req->warningTimePeriod = lsfdrmaa_parse_timedelta( argument );
			}
			break;
			
			case 'Z':
			/*
			* -Zs
			*   Spools a job command file to the directory specified by the
			*   JOB_SPOOL_DIR parameter in lsb.params, and uses the spooled
			*   file as the command file for the job.
			*/
			req->options2 |= SUB2_JOB_CMD_SPOOL;
			break;
			
			default:
			unimplemented_option:
			break;
		}
	}
	#undef ASSIGN_ARG
	#undef ASSIGN_INTARG
}


time_t
lsfdrmaa_parse_datetime( const char *datetime_string )
{
	
	#line 605 "native.rl"
	
	
	
	static const char _lsfdrmaa_datetime_actions[] = {
		0, 1, 0, 1, 1, 1, 3, 2,
		2, 3, 0
	};
	
	static const char _lsfdrmaa_datetime_key_offsets[] = {
		0, 0, 2, 5, 7, 9, 11, 14,
		17, 0
	};
	
	static const char _lsfdrmaa_datetime_trans_keys[] = {
		48, 57, 58, 48, 57, 48, 57, 48,
		57, 48, 57, 58, 48, 57, 58, 48,
		57, 48, 57, 0
	};
	
	static const char _lsfdrmaa_datetime_single_lengths[] = {
		0, 0, 1, 0, 0, 0, 1, 1,
		0, 0
	};
	
	static const char _lsfdrmaa_datetime_range_lengths[] = {
		0, 1, 1, 1, 1, 1, 1, 1,
		1, 0
	};
	
	static const char _lsfdrmaa_datetime_index_offsets[] = {
		0, 0, 2, 5, 7, 9, 11, 14,
		17, 0
	};
	
	static const char _lsfdrmaa_datetime_trans_cond_spaces[] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, 0
	};
	
	static const char _lsfdrmaa_datetime_trans_offsets[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 0
	};
	
	static const char _lsfdrmaa_datetime_trans_lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 0
	};
	
	static const char _lsfdrmaa_datetime_cond_keys[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0
	};
	
	static const char _lsfdrmaa_datetime_cond_targs[] = {
		2, 0, 3, 2, 0, 6, 0, 7,
		0, 8, 0, 4, 6, 0, 5, 7,
		0, 8, 0, 0
	};
	
	static const char _lsfdrmaa_datetime_cond_actions[] = {
		7, 1, 3, 5, 1, 7, 1, 7,
		1, 7, 1, 3, 5, 1, 3, 5,
		1, 5, 1, 0
	};
	
	static const char _lsfdrmaa_datetime_eof_actions[] = {
		0, 1, 1, 1, 1, 1, 3, 3,
		3, 0
	};
	
	static const char _lsfdrmaa_datetime_nfa_targs[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_datetime_nfa_offsets[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0
	};
	
	static const char _lsfdrmaa_datetime_nfa_push_actions[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_datetime_nfa_pop_trans[] = {
		0, 0
	};
	
	static const int lsfdrmaa_datetime_start = 1;
	static const int lsfdrmaa_datetime_first_final = 6;
	static const int lsfdrmaa_datetime_error = 0;
	
	static const int lsfdrmaa_datetime_en_main = 1;
	
	
	#line 608 "native.rl"
	
	fsd_datetime_t dt;
	int fields[4];
	int n_fields = 0;
	const char *i;
	int v = 0;
	time_t result;
	
	const char *p = datetime_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;
	
	fsd_log_enter(( "(%s)", datetime_string ));
	
	{
		cs = (int)lsfdrmaa_datetime_start;
	}
	
	#line 622 "native.rl"
	
	
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
				_keys = ( _lsfdrmaa_datetime_trans_keys + (_lsfdrmaa_datetime_key_offsets[cs]));
				_trans = (unsigned int)_lsfdrmaa_datetime_index_offsets[cs];
				
				_klen = (int)_lsfdrmaa_datetime_single_lengths[cs];
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
				
				_klen = (int)_lsfdrmaa_datetime_range_lengths[cs];
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
				_ckeys = ( _lsfdrmaa_datetime_cond_keys + (_lsfdrmaa_datetime_trans_offsets[_trans]));
				_klen = (int)_lsfdrmaa_datetime_trans_lengths[_trans];
				_cond = (unsigned int)_lsfdrmaa_datetime_trans_offsets[_trans];
				
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
				cs = (int)_lsfdrmaa_datetime_cond_targs[_cond];
				
				if ( _lsfdrmaa_datetime_cond_actions[_cond] == 0 )
				goto _again;
				
				_acts = ( _lsfdrmaa_datetime_actions + (_lsfdrmaa_datetime_cond_actions[_cond]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 )
				{
					switch ( (*( _acts)) )
					{
						case 0:  {
							{
								#line 594 "native.rl"
								
								fsd_exc_raise_fmt(
								FSD_ERRNO_INVALID_VALUE_FORMAT,
								"date/time syntax error: %s", datetime_string );
							}
							break; }
						case 1:  {
							{
								#line 599 "native.rl"
								
								fields[ n_fields++ ] = v;
							}
							break; }
						case 2:  {
							{
								#line 602 "native.rl"
								v = 0; }
							break; }
						case 3:  {
							{
								#line 602 "native.rl"
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
					__acts = ( _lsfdrmaa_datetime_actions + (_lsfdrmaa_datetime_eof_actions[cs]));
					__nacts = (unsigned int)(*( __acts));
					__acts += 1;
					while ( __nacts > 0 ) {
						switch ( (*( __acts)) ) {
							case 0:  {
								{
									#line 594 "native.rl"
									
									fsd_exc_raise_fmt(
									FSD_ERRNO_INVALID_VALUE_FORMAT,
									"date/time syntax error: %s", datetime_string );
								}
								break; }
							case 1:  {
								{
									#line 599 "native.rl"
									
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
	
	#line 623 "native.rl"
	
	
	fsd_assert( 2 <= n_fields  &&  n_fields <= 4 );
	dt.minute = fields[--n_fields];
	dt.hour = fields[--n_fields];
	dt.mask = FSD_DT_MINUTE | FSD_DT_HOUR;
	if( n_fields > 0 )
	{
		dt.day = fields[--n_fields];
		dt.mask |= FSD_DT_DAY;
	}
	if( n_fields > 0 )
	{
		dt.month = fields[--n_fields];
		dt.mask |= FSD_DT_MONTH;
	}
	fsd_assert( n_fields == 0 );
	
	result = fsd_datetime_after( &dt, time(NULL) );
	
	fsd_log_return(( " =%ld", (long)result ));
	return result;
}


int
lsfdrmaa_parse_timedelta( const char *timedelta_string )
{
	
	#line 661 "native.rl"
	
	
	
	static const char _lsfdrmaa_timedelta_actions[] = {
		0, 1, 0, 1, 2, 1, 3, 2,
		1, 2, 2, 4, 5, 0
	};
	
	static const char _lsfdrmaa_timedelta_key_offsets[] = {
		0, 0, 2, 5, 7, 0
	};
	
	static const char _lsfdrmaa_timedelta_trans_keys[] = {
		48, 57, 58, 48, 57, 48, 57, 48,
		57, 0
	};
	
	static const char _lsfdrmaa_timedelta_single_lengths[] = {
		0, 0, 1, 0, 0, 0
	};
	
	static const char _lsfdrmaa_timedelta_range_lengths[] = {
		0, 1, 1, 1, 1, 0
	};
	
	static const char _lsfdrmaa_timedelta_index_offsets[] = {
		0, 0, 2, 5, 7, 0
	};
	
	static const char _lsfdrmaa_timedelta_trans_cond_spaces[] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, 0
	};
	
	static const char _lsfdrmaa_timedelta_trans_offsets[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 0
	};
	
	static const char _lsfdrmaa_timedelta_trans_lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 0
	};
	
	static const char _lsfdrmaa_timedelta_cond_keys[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0
	};
	
	static const char _lsfdrmaa_timedelta_cond_targs[] = {
		2, 0, 3, 2, 0, 4, 0, 4,
		0, 0
	};
	
	static const char _lsfdrmaa_timedelta_cond_actions[] = {
		7, 1, 5, 3, 1, 7, 1, 3,
		1, 0
	};
	
	static const char _lsfdrmaa_timedelta_eof_actions[] = {
		0, 1, 1, 1, 10, 0
	};
	
	static const char _lsfdrmaa_timedelta_nfa_targs[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_timedelta_nfa_offsets[] = {
		0, 0, 0, 0, 0, 0
	};
	
	static const char _lsfdrmaa_timedelta_nfa_push_actions[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_timedelta_nfa_pop_trans[] = {
		0, 0
	};
	
	static const int lsfdrmaa_timedelta_start = 1;
	static const int lsfdrmaa_timedelta_first_final = 4;
	static const int lsfdrmaa_timedelta_error = 0;
	
	static const int lsfdrmaa_timedelta_en_main = 1;
	
	
	#line 664 "native.rl"
	
	int v;
	int hours, minutes;
	int timedelta;
	const char *p = timedelta_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;
	
	fsd_log_enter(( "(%s)", timedelta_string ));
	
	{
		cs = (int)lsfdrmaa_timedelta_start;
	}
	
	#line 674 "native.rl"
	
	
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
				_keys = ( _lsfdrmaa_timedelta_trans_keys + (_lsfdrmaa_timedelta_key_offsets[cs]));
				_trans = (unsigned int)_lsfdrmaa_timedelta_index_offsets[cs];
				
				_klen = (int)_lsfdrmaa_timedelta_single_lengths[cs];
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
				
				_klen = (int)_lsfdrmaa_timedelta_range_lengths[cs];
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
				_ckeys = ( _lsfdrmaa_timedelta_cond_keys + (_lsfdrmaa_timedelta_trans_offsets[_trans]));
				_klen = (int)_lsfdrmaa_timedelta_trans_lengths[_trans];
				_cond = (unsigned int)_lsfdrmaa_timedelta_trans_offsets[_trans];
				
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
				cs = (int)_lsfdrmaa_timedelta_cond_targs[_cond];
				
				if ( _lsfdrmaa_timedelta_cond_actions[_cond] == 0 )
				goto _again;
				
				_acts = ( _lsfdrmaa_timedelta_actions + (_lsfdrmaa_timedelta_cond_actions[_cond]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 )
				{
					switch ( (*( _acts)) )
					{
						case 0:  {
							{
								#line 652 "native.rl"
								
								fsd_exc_raise_fmt(
								FSD_ERRNO_INVALID_VALUE_FORMAT,
								"time delta syntax error: %s", timedelta_string );
							}
							break; }
						case 1:  {
							{
								#line 657 "native.rl"
								v = 0; }
							break; }
						case 2:  {
							{
								#line 657 "native.rl"
								v *= 10;  v += (( (*( p)))) - '0'; }
							break; }
						case 3:  {
							{
								#line 658 "native.rl"
								hours = v; }
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
					__acts = ( _lsfdrmaa_timedelta_actions + (_lsfdrmaa_timedelta_eof_actions[cs]));
					__nacts = (unsigned int)(*( __acts));
					__acts += 1;
					while ( __nacts > 0 ) {
						switch ( (*( __acts)) ) {
							case 0:  {
								{
									#line 652 "native.rl"
									
									fsd_exc_raise_fmt(
									FSD_ERRNO_INVALID_VALUE_FORMAT,
									"time delta syntax error: %s", timedelta_string );
								}
								break; }
							case 4:  {
								{
									#line 658 "native.rl"
									minutes = v; }
								break; }
							case 5:  {
								{
									#line 659 "native.rl"
									timedelta = (60*hours + minutes)*60; }
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
	
	#line 675 "native.rl"
	
	fsd_log_return(( " =%d", timedelta ));
	return timedelta;
}


void
lsfdrmaa_parse_asked_hosts(
struct submit *req,
const char *asked_hosts_string
)
{
	
	#line 707 "native.rl"
	
	
	
	static const char _lsfdrmaa_asked_hosts_actions[] = {
		0, 1, 1, 1, 2, 2, 3, 0,
		0
	};
	
	static const char _lsfdrmaa_asked_hosts_key_offsets[] = {
		0, 2, 4, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_trans_keys[] = {
		9, 32, 9, 32, 9, 32, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_single_lengths[] = {
		2, 2, 2, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_range_lengths[] = {
		0, 0, 0, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_index_offsets[] = {
		0, 3, 6, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_trans_cond_spaces[] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_trans_offsets[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_trans_lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_cond_keys[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_cond_targs[] = {
		0, 0, 1, 2, 2, 1, 2, 2,
		1, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_cond_actions[] = {
		0, 0, 3, 5, 5, 0, 0, 0,
		3, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_eof_actions[] = {
		1, 5, 0, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_nfa_targs[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_nfa_offsets[] = {
		0, 0, 0, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_nfa_push_actions[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_asked_hosts_nfa_pop_trans[] = {
		0, 0
	};
	
	static const int lsfdrmaa_asked_hosts_start = 0;
	static const int lsfdrmaa_asked_hosts_first_final = 1;
	static const int lsfdrmaa_asked_hosts_error = -1;
	
	static const int lsfdrmaa_asked_hosts_en_main = 0;
	
	
	#line 710 "native.rl"
	
	const char *name;
	size_t name_len;
	
	const char *p = asked_hosts_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;
	
	fsd_log_enter(( "(%s)", asked_hosts_string ));
	
	{
		cs = (int)lsfdrmaa_asked_hosts_start;
	}
	
	#line 720 "native.rl"
	
	
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
			_resume:  {
				_keys = ( _lsfdrmaa_asked_hosts_trans_keys + (_lsfdrmaa_asked_hosts_key_offsets[cs]));
				_trans = (unsigned int)_lsfdrmaa_asked_hosts_index_offsets[cs];
				
				_klen = (int)_lsfdrmaa_asked_hosts_single_lengths[cs];
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
				
				_klen = (int)_lsfdrmaa_asked_hosts_range_lengths[cs];
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
				_ckeys = ( _lsfdrmaa_asked_hosts_cond_keys + (_lsfdrmaa_asked_hosts_trans_offsets[_trans]));
				_klen = (int)_lsfdrmaa_asked_hosts_trans_lengths[_trans];
				_cond = (unsigned int)_lsfdrmaa_asked_hosts_trans_offsets[_trans];
				
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
					cs = -1;
					goto _again;
				}
			}
			_match_cond:  {
				cs = (int)_lsfdrmaa_asked_hosts_cond_targs[_cond];
				
				if ( _lsfdrmaa_asked_hosts_cond_actions[_cond] == 0 )
				goto _again;
				
				_acts = ( _lsfdrmaa_asked_hosts_actions + (_lsfdrmaa_asked_hosts_cond_actions[_cond]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 )
				{
					switch ( (*( _acts)) )
					{
						case 0:  {
							{
								#line 688 "native.rl"
								
								fsd_log_debug(( "-m %.*s",(int)name_len, name ));
								fsd_realloc( req->askedHosts, req->numAskedHosts+1, char* );
								req->askedHosts[ req->numAskedHosts++ ] = NULL;
								req->askedHosts[ req->numAskedHosts-1 ]
								= fsd_strndup( name, name_len );
								req->options |= SUB_HOST; 
							}
							break; }
						case 2:  {
							{
								#line 703 "native.rl"
								name = p; }
							break; }
						case 3:  {
							{
								#line 703 "native.rl"
								name_len = p - name; }
							break; }
					}
					_nacts -= 1;
					_acts += 1;
				}
				
				
			}
			_again:  {
				p += 1;
				if ( p != pe )
				goto _resume;
			}
			_test_eof:  { {}
				if ( p == eof )
				{
					const char *__acts;
					unsigned int __nacts;
					__acts = ( _lsfdrmaa_asked_hosts_actions + (_lsfdrmaa_asked_hosts_eof_actions[cs]));
					__nacts = (unsigned int)(*( __acts));
					__acts += 1;
					while ( __nacts > 0 ) {
						switch ( (*( __acts)) ) {
							case 0:  {
								{
									#line 688 "native.rl"
									
									fsd_log_debug(( "-m %.*s",(int)name_len, name ));
									fsd_realloc( req->askedHosts, req->numAskedHosts+1, char* );
									req->askedHosts[ req->numAskedHosts++ ] = NULL;
									req->askedHosts[ req->numAskedHosts-1 ]
									= fsd_strndup( name, name_len );
									req->options |= SUB_HOST; 
								}
								break; }
							case 1:  {
								{
									#line 696 "native.rl"
									
									fsd_exc_raise_fmt(
									FSD_ERRNO_INVALID_VALUE_FORMAT,
									"-m: asked hosts syntax error: %s",
									asked_hosts_string );
								}
								break; }
							case 3:  {
								{
									#line 703 "native.rl"
									name_len = p - name; }
								break; }
						}
						__nacts -= 1;
						__acts += 1;
					}
				}
				
			}
		}
	}
	
	#line 721 "native.rl"
	
	fsd_log_return(( "" ));
}


void
lsfdrmaa_parse_file_transfer(
struct xFile *transfer,
const char *transfer_string )
{
	
	#line 749 "native.rl"
	
	
	
	static const char _lsfdrmaa_transfer_files_actions[] = {
		0, 1, 0, 1, 1, 1, 2, 1,
		3, 1, 4, 1, 5, 1, 6, 2,
		2, 3, 2, 4, 5, 0
	};
	
	static const char _lsfdrmaa_transfer_files_key_offsets[] = {
		0, 0, 4, 8, 12, 16, 20, 22,
		26, 0
	};
	
	static const char _lsfdrmaa_transfer_files_trans_keys[] = {
		9, 32, 60, 62, 9, 32, 60, 62,
		9, 32, 60, 62, 9, 32, 60, 62,
		9, 32, 60, 62, 9, 32, 9, 32,
		60, 62, 9, 32, 60, 62, 0
	};
	
	static const char _lsfdrmaa_transfer_files_single_lengths[] = {
		0, 4, 4, 4, 4, 4, 2, 4,
		4, 0
	};
	
	static const char _lsfdrmaa_transfer_files_range_lengths[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0
	};
	
	static const char _lsfdrmaa_transfer_files_index_offsets[] = {
		0, 0, 5, 10, 15, 20, 25, 28,
		33, 0
	};
	
	static const char _lsfdrmaa_transfer_files_trans_cond_spaces[] = {
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, 0
	};
	
	static const char _lsfdrmaa_transfer_files_trans_offsets[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 0
	};
	
	static const char _lsfdrmaa_transfer_files_trans_lengths[] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 0
	};
	
	static const char _lsfdrmaa_transfer_files_cond_keys[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0
	};
	
	static const char _lsfdrmaa_transfer_files_cond_targs[] = {
		1, 1, 0, 0, 2, 3, 3, 4,
		4, 2, 3, 3, 4, 4, 0, 7,
		7, 8, 8, 5, 6, 6, 0, 0,
		5, 6, 6, 0, 7, 7, 0, 0,
		5, 7, 7, 0, 0, 5, 0
	};
	
	static const char _lsfdrmaa_transfer_files_cond_actions[] = {
		0, 0, 1, 1, 3, 5, 5, 15,
		15, 0, 0, 0, 7, 7, 1, 9,
		9, 0, 0, 18, 13, 13, 1, 1,
		0, 0, 0, 1, 0, 0, 1, 1,
		11, 9, 9, 1, 1, 18, 0
	};
	
	static const char _lsfdrmaa_transfer_files_eof_actions[] = {
		0, 1, 1, 1, 9, 13, 0, 0,
		9, 0
	};
	
	static const char _lsfdrmaa_transfer_files_nfa_targs[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_transfer_files_nfa_offsets[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0
	};
	
	static const char _lsfdrmaa_transfer_files_nfa_push_actions[] = {
		0, 0
	};
	
	static const char _lsfdrmaa_transfer_files_nfa_pop_trans[] = {
		0, 0
	};
	
	static const int lsfdrmaa_transfer_files_start = 1;
	static const int lsfdrmaa_transfer_files_first_final = 4;
	static const int lsfdrmaa_transfer_files_error = 0;
	
	static const int lsfdrmaa_transfer_files_en_main = 1;
	
	
	#line 752 "native.rl"
	
	const char *local_file = NULL;
	size_t local_file_len;
	const char *remote_file = NULL;
	size_t remote_file_len;
	const char *op = NULL;
	size_t op_len;
	unsigned i;
	
	const char *p = transfer_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;
	
	fsd_log_enter(( "(%s)", transfer_string ));
	transfer->options = 0;
	if( sizeof(transfer->subFn) != MAXFILENAMELEN )
	{ /* LSF 7 */
		*(char**)&transfer->subFn = NULL;
		*(char**)&transfer->execFn = NULL;
	}
	
	{
		cs = (int)lsfdrmaa_transfer_files_start;
	}
	
	#line 773 "native.rl"
	
	
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
				_keys = ( _lsfdrmaa_transfer_files_trans_keys + (_lsfdrmaa_transfer_files_key_offsets[cs]));
				_trans = (unsigned int)_lsfdrmaa_transfer_files_index_offsets[cs];
				
				_klen = (int)_lsfdrmaa_transfer_files_single_lengths[cs];
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
				
				_klen = (int)_lsfdrmaa_transfer_files_range_lengths[cs];
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
				_ckeys = ( _lsfdrmaa_transfer_files_cond_keys + (_lsfdrmaa_transfer_files_trans_offsets[_trans]));
				_klen = (int)_lsfdrmaa_transfer_files_trans_lengths[_trans];
				_cond = (unsigned int)_lsfdrmaa_transfer_files_trans_offsets[_trans];
				
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
				cs = (int)_lsfdrmaa_transfer_files_cond_targs[_cond];
				
				if ( _lsfdrmaa_transfer_files_cond_actions[_cond] == 0 )
				goto _again;
				
				_acts = ( _lsfdrmaa_transfer_files_actions + (_lsfdrmaa_transfer_files_cond_actions[_cond]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 )
				{
					switch ( (*( _acts)) )
					{
						case 0:  {
							{
								#line 732 "native.rl"
								
								fsd_exc_raise_fmt(
								FSD_ERRNO_INVALID_VALUE_FORMAT,
								"-f: file transfer syntax error: %s",
								transfer_string );
							}
							break; }
						case 1:  {
							{
								#line 742 "native.rl"
								local_file = p; }
							break; }
						case 2:  {
							{
								#line 743 "native.rl"
								local_file_len = p - local_file; }
							break; }
						case 3:  {
							{
								#line 744 "native.rl"
								op = p; }
							break; }
						case 4:  {
							{
								#line 744 "native.rl"
								op_len = p - op; }
							break; }
						case 5:  {
							{
								#line 745 "native.rl"
								remote_file = p; }
							break; }
						case 6:  {
							{
								#line 746 "native.rl"
								remote_file_len = p - remote_file; }
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
					__acts = ( _lsfdrmaa_transfer_files_actions + (_lsfdrmaa_transfer_files_eof_actions[cs]));
					__nacts = (unsigned int)(*( __acts));
					__acts += 1;
					while ( __nacts > 0 ) {
						switch ( (*( __acts)) ) {
							case 0:  {
								{
									#line 732 "native.rl"
									
									fsd_exc_raise_fmt(
									FSD_ERRNO_INVALID_VALUE_FORMAT,
									"-f: file transfer syntax error: %s",
									transfer_string );
								}
								break; }
							case 4:  {
								{
									#line 744 "native.rl"
									op_len = p - op; }
								break; }
							case 6:  {
								{
									#line 746 "native.rl"
									remote_file_len = p - remote_file; }
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
	
	#line 774 "native.rl"
	
	
	if( op_len > 2 )
	fsd_exc_raise_fmt(
	FSD_ERRNO_INVALID_VALUE_FORMAT,
	"-f: invalid operator: %.*s", (int)op_len, op );
	for( i = 0;  i < op_len;  i++ )
	if( op[i] == '>' )
	transfer->options |= XF_OP_SUB2EXEC;
	else
	transfer->options |= XF_OP_EXEC2SUB;
	if( op_len==2  &&  op[0] == op[1] )
	{
		if( op[0] == '>' )
		transfer->options |= XF_OP_SUB2EXEC_APPEND;
		else
		transfer->options |= XF_OP_EXEC2SUB_APPEND;
	}
	
	if( remote_file == NULL )
	{
		remote_file = local_file;
		remote_file_len = local_file_len;
	}
	if( sizeof(transfer->subFn) == MAXFILENAMELEN )
	{ /* LSF 6 */
		strlcpy( transfer->subFn, local_file,
		min(local_file_len+1, MAXFILENAMELEN) );
		strlcpy( transfer->execFn, remote_file,
		min(remote_file_len+1, MAXFILENAMELEN) );
	}
	else
	{ /* LSF 7 */
		*(char**)&transfer->subFn = fsd_strndup( local_file, local_file_len );
		*(char**)&transfer->execFn = fsd_strndup( remote_file, remote_file_len );
	}
	fsd_log_return(( ": %.*s %.*s %.*s",
	(int)local_file_len, local_file, (int)op_len, op,
	(int)remote_file_len, remote_file ));
}


/* vim: set ft=ragel: */
