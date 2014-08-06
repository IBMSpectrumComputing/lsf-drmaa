/* $Id: util.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __LSF_DRMAA__UTIL_H
#define __LSF_DRMAA__UTIL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <lsf/lsbatch.h>

struct jobInfoEnt *
lsfdrmaa_copy_job_info( const struct jobInfoEnt *job_info );
void lsfdrmaa_free_job_info( struct jobInfoEnt *job_info );
void lsfdrmaa_free_submit_req( struct submit *req, bool free_req );
void lsfdrmaa_dump_submit_req( struct submit *req );

void
fsd_exc_raise_lsf( const char *function )
	__attribute__(( noreturn ));

int lsfdrmaa_map_lsberrno( int _lsberrno );
int lsfdrmaa_map_lserrno( int _lserrno );

#endif /* __LSF_DRMAA__UTIL_H */
