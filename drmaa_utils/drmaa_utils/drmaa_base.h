/* $Id: drmaa_base.h 2 2009-10-12 09:51:22Z mamonski $ */
/*
 * FedStage DRMAA utilities library
 * Copyright (C) 2006-2008  FedStage Systems
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

#ifndef __DRMAA_UTILS__DRMAA_BASE_H
#define __DRMAA_UTILS__DRMAA_BASE_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/common.h>
#include <drmaa_utils/thread.h>

struct fsd_drmaa_singletone_s {
	/** Global session object */
	fsd_drmaa_session_t *session;
	fsd_mutex_t session_mutex;

	fsd_drmaa_session_t *(*new_session)(
			fsd_drmaa_singletone_t *self,
			const char *contact
			);

	fsd_template_t *(*new_job_template)(
			fsd_drmaa_singletone_t *self
			);

	const char* (*get_contact)( fsd_drmaa_singletone_t *self );
	void (*get_version)(
			fsd_drmaa_singletone_t *self,
			unsigned *major, unsigned *minor
			);
	const char* (*get_DRM_system)( fsd_drmaa_singletone_t *self );
	const char* (*get_DRMAA_implementation)( fsd_drmaa_singletone_t *self );

	fsd_iter_t * (*
	get_attribute_names)(
		fsd_drmaa_singletone_t *self );

	fsd_iter_t * (*
	get_vector_attribute_names)(
		fsd_drmaa_singletone_t *self );

	int (*wifexited)( int *exited, int stat,
		char *error_diagnosis, size_t error_diag_len );
	int (*wexitstatus)( int *exit_status, int stat,
		char *error_diagnosis, size_t error_diag_len );
	int (*wifsignaled)( int *signaled, int stat,
		char *error_diagnosis, size_t error_diag_len );
	int (*wtermsig)( char *signal, size_t signal_len, int stat,
		char *error_diagnosis, size_t error_diag_len );
	int (*wcoredump)( int *core_dumped, int stat,
		char *error_diagnosis, size_t error_diag_len );
	int (*wifaborted)( int *aborted, int stat,
		char *error_diagnosis, size_t error_diag_len );
};

extern fsd_drmaa_singletone_t _fsd_drmaa_singletone;

#endif /* __DRMAA_UTILS__DRMAA_BASE_H */
