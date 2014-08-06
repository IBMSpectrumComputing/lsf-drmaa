/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf --readonly-tables --output-file=drmaa_attrib.c drmaa_attrib.gperf  */
/* Computed positions: -k'11,16' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "drmaa_attrib.gperf"

/* $Id: drmaa_attrib.gperf 1409 2008-09-25 08:59:45Z lukasz $ */
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

#include <drmaa_utils/common.h>
#include <drmaa_utils/drmaa_attrib.h>
#include <drmaa_utils/template.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: drmaa_attrib.gperf 1409 2008-09-25 08:59:45Z lukasz $";
#endif

extern const fsd_attribute_t drmaa_attributes[];
#define t(code) \
	( & drmaa_attributes[ code ] )

#line 48 "drmaa_attrib.gperf"
struct drmaa_attrib { int name; const fsd_attribute_t *attr; };
#include <string.h>

#define TOTAL_KEYWORDS 21
#define MIN_WORD_LENGTH 8
#define MAX_WORD_LENGTH 26
#define MIN_HASH_VALUE 8
#define MAX_HASH_VALUE 46
/* maximum key range = 39, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 18, 47,  0, 47,  0,
      47, 47, 20,  0, 13, 10,  5,  0,  5, 47,
       5, 47,  0, 47, 15,  0,  0,  0,  0, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[15]+1];
      /*FALLTHROUGH*/
      case 15:
      case 14:
      case 13:
      case 12:
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
      case 9:
      case 8:
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str8[sizeof("drmaa_wd")];
    char stringpool_str11[sizeof("drmaa_v_env")];
    char stringpool_str12[sizeof("drmaa_v_argv")];
    char stringpool_str13[sizeof("drmaa_v_email")];
    char stringpool_str14[sizeof("drmaa_js_state")];
    char stringpool_str16[sizeof("drmaa_wct_slimit")];
    char stringpool_str17[sizeof("drmaa_output_path")];
    char stringpool_str18[sizeof("drmaa_job_category")];
    char stringpool_str19[sizeof("drmaa_job_name")];
    char stringpool_str20[sizeof("drmaa_transfer_files")];
    char stringpool_str21[sizeof("drmaa_duration_slimit")];
    char stringpool_str22[sizeof("drmaa_block_email")];
    char stringpool_str24[sizeof("drmaa_deadline_time")];
    char stringpool_str25[sizeof("drmaa_remote_command")];
    char stringpool_str26[sizeof("drmaa_input_path")];
    char stringpool_str29[sizeof("drmaa_wct_hlimit")];
    char stringpool_str31[sizeof("drmaa_duration_hlimit")];
    char stringpool_str34[sizeof("drmaa_join_files")];
    char stringpool_str36[sizeof("drmaa_start_time")];
    char stringpool_str41[sizeof("drmaa_error_path")];
    char stringpool_str46[sizeof("drmaa_native_specification")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "drmaa_wd",
    "drmaa_v_env",
    "drmaa_v_argv",
    "drmaa_v_email",
    "drmaa_js_state",
    "drmaa_wct_slimit",
    "drmaa_output_path",
    "drmaa_job_category",
    "drmaa_job_name",
    "drmaa_transfer_files",
    "drmaa_duration_slimit",
    "drmaa_block_email",
    "drmaa_deadline_time",
    "drmaa_remote_command",
    "drmaa_input_path",
    "drmaa_wct_hlimit",
    "drmaa_duration_hlimit",
    "drmaa_join_files",
    "drmaa_start_time",
    "drmaa_error_path",
    "drmaa_native_specification"
  };
#define stringpool ((const char *) &stringpool_contents)
#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct drmaa_attrib *
drmaa_attrib_lookup (register const char *str, register unsigned int len)
{
  static const struct drmaa_attrib wordlist[] =
    {
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 55 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, t(DRMAA_ATTR_WORKING_DIRECTORY)},
      {-1}, {-1},
#line 54 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, t(DRMAA_ATTR_JOB_ENVIRONMENT)},
#line 52 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, t(DRMAA_ATTR_ARGS)},
#line 58 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, t(DRMAA_ATTR_EMAIL)},
#line 53 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, t(DRMAA_ATTR_JOB_SUBMISSION_STATE)},
      {-1},
#line 69 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, t(DRMAA_ATTR_SOFT_WALL_CLOCK_TIME_LIMIT)},
#line 63 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, t(DRMAA_ATTR_OUTPUT_PATH)},
#line 56 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, t(DRMAA_ATTR_JOB_CATEGORY)},
#line 61 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, t(DRMAA_ATTR_JOB_NAME)},
#line 66 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, t(DRMAA_ATTR_TRANSFER_FILES)},
#line 71 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, t(DRMAA_ATTR_SOFT_RUN_DURATION_LIMIT)},
#line 59 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, t(DRMAA_ATTR_BLOCK_EMAIL)},
      {-1},
#line 67 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, t(DRMAA_ATTR_DEADLINE_TIME)},
#line 51 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, t(DRMAA_ATTR_REMOTE_COMMAND)},
#line 62 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, t(DRMAA_ATTR_INPUT_PATH)},
      {-1}, {-1},
#line 68 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, t(DRMAA_ATTR_HARD_WALL_CLOCK_TIME_LIMIT)},
      {-1},
#line 70 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, t(DRMAA_ATTR_HARD_RUN_DURATION_LIMIT)},
      {-1}, {-1},
#line 65 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, t(DRMAA_ATTR_JOIN_FILES)},
      {-1},
#line 60 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, t(DRMAA_ATTR_START_TIME)},
      {-1}, {-1}, {-1}, {-1},
#line 64 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, t(DRMAA_ATTR_ERROR_PATH)},
      {-1}, {-1}, {-1}, {-1},
#line 57 "drmaa_attrib.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, t(DRMAA_ATTR_NATIVE_SPECIFICATION)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + stringpool;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[key];
            }
        }
    }
  return 0;
}
#line 72 "drmaa_attrib.gperf"


#undef t

static const fsd_attribute_t *
drmaa_template_by_name( const fsd_template_t *self, const char *name )
{
	const struct drmaa_attrib *found;
	found = drmaa_attrib_lookup( name, strlen(name) );
	if( found != NULL )
		return found->attr;
	else
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_ARGUMENT,
				"invalid DRMAA attribute name: %s", name
				);
}


static const fsd_attribute_t *
drmaa_template_by_code( const fsd_template_t *self, int code )
{
	if( 0 <= code  &&  code < DRMAA_N_ATTRIBUTES )
		return & drmaa_attributes[ code ];
	else
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_ARGUMENT,
				"invalid attribute code: %d", code
				);
}


fsd_template_t *
drmaa_template_new(void)
{
	return fsd_template_new(
			drmaa_template_by_name,
			drmaa_template_by_code,
			DRMAA_N_ATTRIBUTES
			);
}


const fsd_attribute_t drmaa_attributes[ DRMAA_N_ATTRIBUTES ] = {
	 { "drmaa_remote_command", DRMAA_ATTR_REMOTE_COMMAND, false },
	 { "drmaa_v_argv", DRMAA_ATTR_ARGS, true },
	 { "drmaa_js_state", DRMAA_ATTR_JOB_SUBMISSION_STATE, false },
	 { "drmaa_v_env", DRMAA_ATTR_JOB_ENVIRONMENT, true },
	 { "drmaa_wd", DRMAA_ATTR_WORKING_DIRECTORY, false },
	 { "drmaa_job_category", DRMAA_ATTR_JOB_CATEGORY, false },
	 { "drmaa_native_specification", DRMAA_ATTR_NATIVE_SPECIFICATION, false },
	 { "drmaa_v_email", DRMAA_ATTR_EMAIL, true },
	 { "drmaa_block_email", DRMAA_ATTR_BLOCK_EMAIL, false },
	 { "drmaa_start_time", DRMAA_ATTR_START_TIME, false },
	 { "drmaa_job_name", DRMAA_ATTR_JOB_NAME, false },
	 { "drmaa_input_path", DRMAA_ATTR_INPUT_PATH, false },
	 { "drmaa_output_path", DRMAA_ATTR_OUTPUT_PATH, false },
	 { "drmaa_error_path", DRMAA_ATTR_ERROR_PATH, false },
	 { "drmaa_join_files", DRMAA_ATTR_JOIN_FILES, false },
	 { "drmaa_transfer_files", DRMAA_ATTR_TRANSFER_FILES, false },
	 { "drmaa_deadline_time", DRMAA_ATTR_DEADLINE_TIME, false },
	 { "drmaa_wct_hlimit", DRMAA_ATTR_HARD_WALL_CLOCK_TIME_LIMIT, false },
	 { "drmaa_wct_slimit", DRMAA_ATTR_SOFT_WALL_CLOCK_TIME_LIMIT, false },
	 { "drmaa_duration_hlimit", DRMAA_ATTR_HARD_RUN_DURATION_LIMIT, false },
	 { "drmaa_duration_slimit", DRMAA_ATTR_SOFT_RUN_DURATION_LIMIT, false }
};

/* vim: set ft=c: */
