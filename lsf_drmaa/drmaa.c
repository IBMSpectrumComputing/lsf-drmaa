/* $Id: drmaa.c 2 2009-10-12 09:51:22Z mamonski $ */
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

#include <lsf/lsbatch.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/drmaa.h>
#include <drmaa_utils/drmaa_base.h>
#include <drmaa_utils/iter.h>
#include <drmaa_utils/drmaa_attrib.h>

#include <lsf_drmaa/session.h>


static fsd_drmaa_session_t *
lsfdrmaa_new_session( fsd_drmaa_singletone_t *self, const char *contact )
{
	return lsfdrmaa_session_new( contact );
}

static fsd_template_t *
lsfdrmaa_new_job_template( fsd_drmaa_singletone_t *self )
{
	return drmaa_template_new();
}

static const char *
lsfdrmaa_get_contact( fsd_drmaa_singletone_t *self )
{
	return "";
}

static void
lsfdrmaa_get_version( fsd_drmaa_singletone_t *self,
		unsigned *major, unsigned *minor )
{
	*major = 1;  *minor = 0;
}

static const char *
lsfdrmaa_get_DRM_system( fsd_drmaa_singletone_t *self )
{
	return _VERSION_STR_;
}

static const char *
lsfdrmaa_get_DRMAA_implementation( fsd_drmaa_singletone_t *self )
{
	return PACKAGE_STRING;
}


fsd_iter_t *
lsfdrmaa_get_attribute_names( fsd_drmaa_singletone_t *self )
{
	static const char *attribute_names[] = {
		DRMAA_REMOTE_COMMAND,
		DRMAA_JS_STATE,
		DRMAA_WD,
		DRMAA_JOB_CATEGORY,
		DRMAA_NATIVE_SPECIFICATION,
		DRMAA_BLOCK_EMAIL,
		DRMAA_START_TIME,
		DRMAA_JOB_NAME,
		DRMAA_INPUT_PATH,
		DRMAA_OUTPUT_PATH,
		DRMAA_ERROR_PATH,
		DRMAA_JOIN_FILES,
		DRMAA_TRANSFER_FILES,
		DRMAA_DEADLINE_TIME,
		DRMAA_WCT_HLIMIT,
		DRMAA_WCT_SLIMIT,
		DRMAA_DURATION_HLIMIT,
		NULL
	};
	return fsd_iter_new_const( attribute_names, -1 );
}

fsd_iter_t *
lsfdrmaa_get_vector_attribute_names( fsd_drmaa_singletone_t *self )
{
	static const char *attribute_names[] = {
		DRMAA_V_ARGV,
		DRMAA_V_ENV,
		DRMAA_V_EMAIL,
		NULL
	};
	return fsd_iter_new_const( attribute_names, -1 );
}

static int
lsfdrmaa_wifexited(
		int *exited, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	bool run;
	int exit_status;
	run = ((stat & 0xff) == 0);
	exit_status = ((stat & 0xff00) >> 8) & 0xff; 
	*exited = run && (exit_status <= 128);
	return DRMAA_ERRNO_SUCCESS;
}

static int
lsfdrmaa_wexitstatus(
		int *exit_status, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	*exit_status = ((stat & 0xff00) >> 8) & 0xff; 
	return DRMAA_ERRNO_SUCCESS;
}

static int
lsfdrmaa_wifsignaled(
		int *signaled, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	*signaled = ((stat & 0xff00) >> 8) >= 128;
	return DRMAA_ERRNO_SUCCESS;
}	

static int
lsfdrmaa_wtermsig(
		char *signal, size_t signal_len, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	int sig = ((stat & 0xff00) >> 8) - 128;
	strlcpy( signal, fsd_strsignal(sig), signal_len );
	return DRMAA_ERRNO_SUCCESS;
}

static int
lsfdrmaa_wcoredump(
		int *core_dumped, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	*core_dumped = 0;
	return DRMAA_ERRNO_SUCCESS;
}

static int
lsfdrmaa_wifaborted(
		int *aborted, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	*aborted = ((stat & 0x01) != 0);
	return DRMAA_ERRNO_SUCCESS;
}


fsd_drmaa_singletone_t _fsd_drmaa_singletone = {
	NULL,
	FSD_MUTEX_INITIALIZER,

	lsfdrmaa_new_session,
	lsfdrmaa_new_job_template,

	lsfdrmaa_get_contact,
	lsfdrmaa_get_version,
	lsfdrmaa_get_DRM_system,
	lsfdrmaa_get_DRMAA_implementation,

	lsfdrmaa_get_attribute_names,
	lsfdrmaa_get_vector_attribute_names,

	lsfdrmaa_wifexited,
	lsfdrmaa_wexitstatus,
	lsfdrmaa_wifsignaled,
	lsfdrmaa_wtermsig,
	lsfdrmaa_wcoredump,
	lsfdrmaa_wifaborted
};

