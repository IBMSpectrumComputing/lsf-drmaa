/* $Id: session.c 142 2010-05-01 20:18:54Z mamonski $ */
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

#include <string.h>
#include <unistd.h>

extern char **environ;

#include <lsf/lsbatch.h>

#include <drmaa_utils/iter.h>
#include <drmaa_utils/conf.h>
#include <lsf_drmaa/job.h>
#include <lsf_drmaa/session.h>
#include <lsf_drmaa/util.h>

static char *
lsfdrmaa_session_run_job(
		fsd_drmaa_session_t *self,
		const fsd_template_t *jt
		);

static fsd_iter_t *
lsfdrmaa_session_run_bulk(
		fsd_drmaa_session_t *self,
		const fsd_template_t *jt,
		int start, int end, int incr );

static fsd_job_t *
lsfdrmaa_session_new_job( fsd_drmaa_session_t *self, const char *job_id );

static void
lsfdrmaa_session_update_all_jobs_status( fsd_drmaa_session_t *self );

static void
lsfdrmaa_session_apply_configuration( fsd_drmaa_session_t *self );


fsd_drmaa_session_t *
lsfdrmaa_session_new( const char *contact )
{
	lsfdrmaa_session_t *volatile self = NULL;
	TRY
	 {
		if( lsb_init( "lsf_drmaa" ) < 0 )
			fsd_exc_raise_lsf( "lsb_init" );
		self = (lsfdrmaa_session_t*)fsd_drmaa_session_new(contact);
		fsd_realloc( self, 1, lsfdrmaa_session_t );
		self->super.run_job = lsfdrmaa_session_run_job;
		self->super.run_bulk = lsfdrmaa_session_run_bulk;
		self->super.new_job = lsfdrmaa_session_new_job;
		self->super.update_all_jobs_status
				= lsfdrmaa_session_update_all_jobs_status;
		self->super_apply_configuration = self->super.apply_configuration;
		self->super.apply_configuration = lsfdrmaa_session_apply_configuration;
		self->prepand_report_to_output = false;
		self->super.load_configuration( &self->super, "lsf_drmaa" );
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( self );
		fsd_exc_reraise();
	 }
	END_TRY
	return (fsd_drmaa_session_t*)self;
}


char *
lsfdrmaa_session_run_job(
		fsd_drmaa_session_t *self,
		const fsd_template_t *jt
		)
{
	char *job_id = NULL;
	fsd_iter_t *volatile job_ids = NULL;
	TRY
	 {
		job_ids = self->run_bulk( self, jt, 0, 0, 0 );
		job_id = fsd_strdup( job_ids->next( job_ids ) );
	 }
	FINALLY
	 {
		if( job_ids )
			job_ids->destroy( job_ids );
	 }
	END_TRY
	return job_id;
}


fsd_iter_t *
lsfdrmaa_session_run_bulk(
		fsd_drmaa_session_t *self,
		const fsd_template_t *jt,
		int start, int end, int incr )
{
	struct submit *volatile req = NULL;
	struct submitReply reply = { NULL, 0, NULL, 0 };
	fsd_environ_t *volatile env = NULL;
	fsd_environ_t *volatile saved_env_state = NULL;
	fsd_job_t *volatile job = NULL;
	char **volatile job_ids = NULL;
	const bool bulk = !(start == 0 && end == 0 && incr == 0);
	unsigned n_jobs;
	volatile bool connection_lock = false;

	TRY
	 {
		LS_LONG_INT int_job_id;
		const char *basename;

		req = lsfdrmaa_job_create_req( self, jt, (fsd_environ_t**)&env );

		if( req->jobName )
			basename = req->jobName;
		else
			basename = req->command;
		if( strchr( basename, '[' ) )
			fsd_exc_raise_fmt(
					FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
					"Invalid job name '%s': job name can not contain "
					"array indices use drmaa_run_bulk_jobs instead.",
					basename );

		if( bulk )
		 {
			char *array_name;
			array_name = fsd_asprintf( "%s[%d-%d:%d]",
					basename, start, end, incr );
			if( req->jobName )
				fsd_free( req->jobName );
			else
				req->options |= SUB_JOB_NAME;
			req->jobName = array_name;
		 }

		connection_lock = fsd_mutex_lock( &self->drm_connection_mutex );
		if( env )
			saved_env_state = env->apply( env );
		int_job_id = lsb_submit( req, &reply );
		fsd_log_debug((
					"lsb_submit( %p, %p ) = %d[%d]",
					(void*)req, (void*)&reply,
					LSB_ARRAY_JOBID(int_job_id), LSB_ARRAY_IDX(int_job_id) ));
		if( int_job_id == -1 )
		 {
			lsb_perror("lsb_submit:");
			fsd_exc_raise_fmt(
					FSD_ERRNO_DENIED_BY_DRM,
					"Submission failed: '%s': ",
					lsb_sysmsg());
		 }
		if( env && saved_env_state )
		 {
			env->restore( env, saved_env_state );
			saved_env_state->destroy( saved_env_state );
			saved_env_state = NULL;
		 }
		connection_lock = fsd_mutex_unlock( &self->drm_connection_mutex );

		fsd_assert( LSB_ARRAY_IDX(int_job_id) == 0 );
		int_job_id = LSB_ARRAY_JOBID(int_job_id);

		if( bulk )
		 {
			unsigned idx, i;
			n_jobs = (end - start) / incr + 1;
			fsd_calloc( job_ids, n_jobs+1, char* );
			for( idx = start, i = 0;  idx <= (unsigned)end;  idx += incr, i++ )
			 {
				job_ids[i] = fsd_asprintf( "%u[%u]", (unsigned)int_job_id, idx );
				job = lsfdrmaa_job_new( fsd_strdup(job_ids[i]) );
				job->session = self;
				job->submit_time = time(NULL);
				job->flags |= FSD_JOB_CURRENT_SESSION;
				self->jobs->add( self->jobs, job );
				job->release( job );  job = NULL;
			 }
			fsd_assert( i == n_jobs );
		 }
		else /* ! bulk */
		 {
			n_jobs = 1;
			fsd_calloc( job_ids, n_jobs+1, char* );
			job_ids[0] = fsd_asprintf( "%u", (unsigned)int_job_id );
			job = lsfdrmaa_job_new( fsd_strdup(job_ids[0]) );
			job->session = self;
			job->submit_time = time(NULL);
			job->flags |= FSD_JOB_CURRENT_SESSION;
			self->jobs->add( self->jobs, job );
			job->release( job );  job = NULL;
		 }
	 }
	FINALLY
	 {
		if( saved_env_state )
		 {
			env->restore( env, saved_env_state );
			saved_env_state->destroy( saved_env_state );
		 }
		if( connection_lock )
			fsd_mutex_unlock( &self->drm_connection_mutex );
		if( env )
			env->destroy( env );
		lsfdrmaa_free_submit_req( req, true );
		if( job )
			job->release( job );
		if( fsd_exc_get() != NULL )
			fsd_free_vector( job_ids );
	 }
	END_TRY

	return fsd_iter_new( job_ids, n_jobs );
}


fsd_job_t *
lsfdrmaa_session_new_job( fsd_drmaa_session_t *self, const char *job_id )
{
	fsd_job_t *job;
	job = lsfdrmaa_job_new( fsd_strdup(job_id) );
	job->session = self;
	return job;
}


void
lsfdrmaa_session_update_all_jobs_status( fsd_drmaa_session_t *self )
{
	char **volatile all_job_ids = NULL;
	volatile bool conn_lock = false;
	struct jobInfoEnt **volatile job_info_array = NULL;
	volatile int n_records;

	fsd_log_enter(( "" ));
	TRY
	 {
		const char **job_iter;
		int i;
		fsd_job_t *volatile job = NULL;
		char job_id_str[32];

		all_job_ids = self->get_submited_job_ids( self );

		for( job_iter = (const char **)all_job_ids;  *job_iter;  job_iter++ )
		 {
			job = self->get_job( self, *job_iter );
			job->flags |= FSD_JOB_MISSING;
			job->release( job );
		 }

		conn_lock = fsd_mutex_lock( &self->drm_connection_mutex );
		n_records = lsb_openjobinfo( 0, NULL, NULL, NULL, NULL, ALL_JOB );
		if(n_records > 0)
			fsd_calloc( job_info_array, n_records, struct jobInfoEnt* );
		for( i = 0;  i < n_records;  i++ )
		 {
			int more;
			struct jobInfoEnt *job_info;
			job_info = lsb_readjobinfo( &more );
			if( job_info == NULL )
				fsd_exc_raise_lsf( "lsb_readjobinfo" );
			job_info_array[i] = lsfdrmaa_copy_job_info( job_info );
			if( i+1+more != n_records )
				fsd_log_warning(( "lsb_readjobinfo: invalid job count" ));
		 }
		lsb_closejobinfo();
		conn_lock = fsd_mutex_unlock( &self->drm_connection_mutex );

		for( i = 0;  i < n_records;  i++ )
		 {
			struct jobInfoEnt *job_info = job_info_array[i];
			int job_id, job_array_idx;
			job_id = LSB_ARRAY_JOBID( job_info->jobId );
			job_array_idx = LSB_ARRAY_IDX( job_info->jobId );
			if( job_array_idx )
				sprintf( job_id_str, "%d[%d]", job_id, job_array_idx );
			else
				sprintf( job_id_str, "%d", job_id );
			job = self->get_job( self, job_id_str );
			if( job != NULL )
			 {
				job->flags &= ~FSD_JOB_MISSING;
				TRY
				 {
					((lsfdrmaa_job_t*)job)->read_job_info( job, job_info );
				 }
				FINALLY{ job->release( job ); }
				END_TRY
			 }
		 }

		for( job_iter = (const char **)all_job_ids;  *job_iter;  job_iter++ )
		 {
			job = self->get_job( self, *job_iter );
			if( job->flags & FSD_JOB_MISSING )
			 {
				fsd_log_warning(( "job %s missing from DRM queue",
							job->job_id ));
			 }
			job->release( job );
		 }
		 fsd_free_vector( all_job_ids );
	 }
	FINALLY
	 {
		int i;
		if( conn_lock )
		 {
			lsb_closejobinfo();
			fsd_mutex_unlock( &self->drm_connection_mutex );
		 }
		if( job_info_array )
		 {
			for( i = 0;  i < n_records;  i++ )
				lsfdrmaa_free_job_info( job_info_array[i] );
			fsd_free( job_info_array );
		 }
		
	 }
	END_TRY

	fsd_log_return(( "" ));
}


void
lsfdrmaa_session_apply_configuration( fsd_drmaa_session_t *self )
{
	lsfdrmaa_session_t *lsfself;
	fsd_conf_option_t *prepand_report;
	lsfself = (lsfdrmaa_session_t*)self;
	lsfself->super_apply_configuration(self);

	prepand_report = fsd_conf_dict_get(
			self->configuration, "prepand_report" );
	if( prepand_report )
	 {
		if( prepand_report->type == FSD_CONF_INTEGER )
			lsfself->prepand_report_to_output = (prepand_report->val.integer != 0 );
		else
			fsd_exc_raise_msg(
					FSD_ERRNO_INTERNAL_ERROR,
					"configuration: 'prepand_report' should be 0 or 1"
					);
	 }
}
