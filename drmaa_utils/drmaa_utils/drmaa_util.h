/* $Id: drmaa_util.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__DRMAA_UTIL_H
#define __DRMAA_UTILS__DRMAA_UTIL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/common.h>

enum{
	FSD_DRMAA_PH_HD    = 1<<0,
	FSD_DRMAA_PH_WD    = 1<<1,
	FSD_DRMAA_PH_INCR  = 1<<2
};

struct fsd_expand_drmaa_ph_s {
	/**
	 * Expands placeholders in DRMAA job attribute value.
	 * @param self   Context.
	 * @param input  String to transform.  Should be malloc'ed
	 *   and ownership is passed to callee.
	 * @param set    Set of placeholders to expand.  Bitwise
	 *   `or' of any of following bits:
	 *   - FSD_DRMAA_PH_HD,
	 *   - FSD_DRMAA_PH_WD,
	 *   - FSD_DRMAA_PH_INCR
	 * @return Value with expanded placeholders or \c NULL
	 *   upon error.  Caller is responsible for free()'ing it.
	 */
	char *(*
	expand)( fsd_expand_drmaa_ph_t *self, char *input, unsigned set );

	void (*
	set)( fsd_expand_drmaa_ph_t *self, int placeholder, char *value );

	void (*destroy)( fsd_expand_drmaa_ph_t *self );

	char *home_directory;
	char *working_directory;
	char *bulk_incr;
};

fsd_expand_drmaa_ph_t *
fsd_expand_drmaa_ph_new(
		char *home_directory,
		char *working_directory,
		char *bulk_incr
		);

int fsd_parse_timedelta( const char *string );

/**
 * Return textual representation of action
 * - argument of drmaa_control().
 */
const char *drmaa_control_to_str( int action );

/**
 * Return textual representation of job status
 * - result of drmaa_job_ps().
 */
const char *drmaa_job_ps_to_str( int ps );

int fsd_drmaa_code( int fsd_errno );

int fsd_errno_from_drmaa_code( int drmaa_errno );

const char *
fsd_drmaa_strerror( int drmaa_errno );

#endif /* __DRMAA_UTILS__DRMAA_UTIL_H */

