/* $Id: job.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __LSF_DRMAA__JOB_H
#define __LSF_DRMAA__JOB_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <lsf/lsbatch.h>

#include <drmaa_utils/job.h>
#include <drmaa_utils/environ.h>

typedef struct lsfdrmaa_job_s lsfdrmaa_job_t;

fsd_job_t *
lsfdrmaa_job_new( char *job_id );

struct lsfdrmaa_job_s {
	fsd_job_t super;

	void (*
	read_job_info)( fsd_job_t *job, struct jobInfoEnt *job_info );

	LS_LONG_INT int_job_id;
};

struct submit *
lsfdrmaa_job_create_req(
		fsd_drmaa_session_t *session,
		const fsd_template_t *jt, fsd_environ_t **envp
		);

#endif /* __LSF_DRMAA__JOB_H */

