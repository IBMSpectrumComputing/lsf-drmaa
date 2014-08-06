/* $Id: fsd_job.c 185 2010-05-13 17:10:00Z mmatloka $ */
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

#include <string.h>

#include <drmaa_utils/drmaa.h>
#include <drmaa_utils/iter.h>
#include <drmaa_utils/job.h>
#include <drmaa_utils/lookup3.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: fsd_job.c 185 2010-05-13 17:10:00Z mmatloka $";
#endif


static void fsd_job_release( fsd_job_t *self );
static void fsd_job_destroy( fsd_job_t *self );
static void fsd_job_control( fsd_job_t *self, int action );
static void fsd_job_update_status( fsd_job_t *self );
static void fsd_job_get_termination_status( fsd_job_t *self,
			int *status, fsd_iter_t **rusage_out );
static void fsd_job_on_missing( fsd_job_t *self );

fsd_job_t *
fsd_job_new( char *job_id )
{
	fsd_job_t *volatile self = NULL;
	fsd_log_enter(( "(%s)", job_id ));
	TRY
	 {
		fsd_malloc( self, fsd_job_t );
		self->release = fsd_job_release;
		self->destroy = fsd_job_destroy;
		self->control = fsd_job_control;
		self->update_status = fsd_job_update_status;
		self->get_termination_status = fsd_job_get_termination_status;
		self->on_missing = fsd_job_on_missing;
		self->next              = NULL;
		self->ref_cnt           = 1;
		self->job_id            = job_id;
		self->session           = NULL;
		self->last_update_time  = 0;
		self->flags             = 0;
		self->state             = DRMAA_PS_UNDETERMINED;
		self->exit_status       = 0;
		self->submit_time       = 0;
		self->start_time        = 0;
		self->end_time          = 0;
		self->cpu_usage         = 0;
		self->mem_usage         = 0;
		self->vmem_usage        = 0;
		self->walltime          = 0;
		self->n_execution_hosts = 0;
		self->execution_hosts   = NULL;
		fsd_mutex_init( &self->mutex );
		fsd_cond_init( &self->status_cond );
		fsd_cond_init( &self->destroy_cond );
		fsd_mutex_lock( &self->mutex );
	 }
	EXCEPT_DEFAULT
	 {
		if( self )
			self->destroy( self );
		else
			fsd_free( job_id );
		fsd_exc_reraise();
	 }
	END_TRY
	fsd_log_return(( "=%p: ref_cnt=%d [lock %s]",
				(void*)self, self->ref_cnt, self->job_id ));
	return self;
}


void
fsd_job_release( fsd_job_t *self )
{
	bool destroy;
	fsd_log_enter(( "(%p={job_id=%s, ref_cnt=%d}) [unlock %s]",
				(void*)self, self->job_id, self->ref_cnt, self->job_id ));
	fsd_assert( self->ref_cnt > 0 );
	destroy = ( --(self->ref_cnt) == 0 );
	fsd_mutex_unlock( &self->mutex );
	if( destroy )
		self->destroy( self );
	fsd_log_return(( "" ));
}


void
fsd_job_destroy( fsd_job_t *self )
{
	fsd_log_enter(( "(%p={job_id=%s})", (void*)self, self->job_id ));
	fsd_cond_destroy( &self->status_cond );
	fsd_cond_destroy( &self->destroy_cond );
	fsd_mutex_destroy( &self->mutex );
	fsd_free( self->job_id );
	fsd_free( self );
	fsd_log_return(( "" ));
}


void
fsd_job_control( fsd_job_t *self, int action )
{
	fsd_exc_raise_code( FSD_ERRNO_NOT_IMPLEMENTED );
}

void
fsd_job_update_status( fsd_job_t *self )
{
	fsd_exc_raise_code( FSD_ERRNO_NOT_IMPLEMENTED );
}

void
fsd_job_get_termination_status( fsd_job_t *self,
			int *status, fsd_iter_t **rusage_out )
{
	fsd_iter_t* volatile rusage = NULL;

	TRY
	 {
		if( rusage_out )
		 {
			rusage = fsd_iter_new( NULL, 0 );
			rusage->append( rusage, fsd_asprintf(
						"submission_time=%ld", (long)self->submit_time ) );
			rusage->append( rusage, fsd_asprintf(
						"start_time=%ld", (long)self->start_time ) );
			rusage->append( rusage, fsd_asprintf(
						"end_time=%ld", (long)self->end_time ) );
			rusage->append( rusage, fsd_asprintf(
						"cpu=%ld", self->cpu_usage ) );
			rusage->append( rusage, fsd_asprintf(
						"mem=%ld", self->mem_usage ) );
			rusage->append( rusage, fsd_asprintf(
						"vmem=%ld", self->vmem_usage ) );
			rusage->append( rusage, fsd_asprintf(
						"walltime=%ld", self->walltime ) );
			rusage->append( rusage, fsd_asprintf(
						"n_hosts=%d", self->n_execution_hosts ) );
			rusage->append( rusage, fsd_asprintf(
						"hosts=%s", self->execution_hosts ) );
		 }
	 }
	EXCEPT_DEFAULT
	 {
		if( rusage )
			rusage->destroy( rusage );
		if( rusage_out )
			*rusage_out = NULL;
		fsd_exc_reraise();
	 }
	ELSE
	 {
		if( status )
			*status = self->exit_status;
		if( rusage_out )
			*rusage_out = rusage;
	 }
	END_TRY
}

void
fsd_job_on_missing( fsd_job_t *self )
{
	fsd_log_warning(( "job %s missing from DRM queue", self->job_id ));
}


static void
fsd_job_set_destroy( fsd_job_set_t *self );
static void
fsd_job_set_add( fsd_job_set_t *self, fsd_job_t *job );
static void
fsd_job_set_remove( fsd_job_set_t *self, fsd_job_t *job );
static fsd_job_t *
fsd_job_set_get( fsd_job_set_t *self, const char *job_id );
static bool
fsd_job_set_empty( fsd_job_set_t *self );
static fsd_job_t *
fsd_job_set_find_terminated( fsd_job_set_t *self );
static char **
fsd_job_set_get_all_job_ids( fsd_job_set_t *self );
static void fsd_job_set_signal_all( fsd_job_set_t *self );


fsd_job_set_t *
fsd_job_set_new(void)
{
	fsd_job_set_t *volatile self = NULL;
	const size_t initial_size = 1024;

	fsd_log_enter(( "()" ));
	TRY
	 {
		fsd_malloc( self, fsd_job_set_t );
		self->destroy = fsd_job_set_destroy;
		self->add = fsd_job_set_add;
		self->remove = fsd_job_set_remove;
		self->get = fsd_job_set_get;
		self->empty = fsd_job_set_empty;
		self->find_terminated = fsd_job_set_find_terminated;
		self->get_all_job_ids = fsd_job_set_get_all_job_ids;
		self->signal_all = fsd_job_set_signal_all;
		self->tab = NULL;
		self->n_jobs = 0;
		fsd_calloc( self->tab, initial_size, fsd_job_t* );
		self->tab_size = initial_size;
		self->tab_mask = self->tab_size - 1;
		fsd_mutex_init( &self->mutex );
	 }
	EXCEPT_DEFAULT
	 {
		if( self )
		 {
			fsd_free( self->tab );
			fsd_free( self );
		 }
		fsd_exc_reraise();
	 }
	END_TRY

	fsd_log_return(( " =%p", (void*)self ));
	return self;
}


void
fsd_job_set_destroy( fsd_job_set_t *self )
{
	unsigned i;
	fsd_job_t *j;

	fsd_log_enter(( "()" ));
	for( i = 0;  i < self->tab_size;  i++ )
		for( j = self->tab[i];  j != NULL;  )
		 {
			fsd_job_t *job = j;
			j = j->next;
			fsd_mutex_lock( &job->mutex );
			job->release( job );
		 }
	fsd_free( self->tab );
	fsd_free( self );
	fsd_log_return(( "" ));
}


void
fsd_job_set_add( fsd_job_set_t *self, fsd_job_t *job )
{
	uint32_t h;
	fsd_log_enter(( "(job=%p, job_id=%s)", (void*)job, job->job_id ));
	fsd_mutex_lock( &self->mutex );
	h = hashstr( job->job_id, strlen(job->job_id), 0 );
	h &= self->tab_mask;
	job->next = self->tab[ h ];
	self->tab[ h ] = job;
	self->n_jobs++;
	job->ref_cnt++;
	fsd_mutex_unlock( &self->mutex );
	fsd_log_return(( ": job->ref_cnt=%d", job->ref_cnt ));
}


void
fsd_job_set_remove( fsd_job_set_t *self, fsd_job_t *job )
{
	fsd_job_t **pjob = NULL;
	uint32_t h;

	fsd_log_enter(( "(job_id=%s)", job->job_id ));
	fsd_mutex_lock( &self->mutex );
	TRY
	 {
		h = hashstr( job->job_id, strlen(job->job_id), 0 );
		h &= self->tab_mask;
		for( pjob = &self->tab[ h ];  *pjob;  pjob = &(*pjob)->next )
		 {
			if( *pjob == job )
				break;
		 }
		if( *pjob )
		 {
			*pjob = (*pjob)->next;
			job->next = NULL;
			self->n_jobs--;
			job->ref_cnt--;
		 }
		else
			fsd_exc_raise_code( FSD_DRMAA_ERRNO_INVALID_JOB );
	 }
	FINALLY
	 { fsd_mutex_unlock( &self->mutex ); }
	END_TRY
	fsd_log_return(( ": job->ref_cnt=%d", job->ref_cnt ));
}


fsd_job_t *
fsd_job_set_get( fsd_job_set_t *self, const char *job_id )
{
	uint32_t h;
	fsd_job_t *job = NULL;

	fsd_log_enter(( "(job_id=%s)", job_id ));
	fsd_mutex_lock( &self->mutex );
	h = hashstr( job_id, strlen(job_id), 0 );
	h &= self->tab_mask;
	for( job = self->tab[ h ];  job;  job = job->next )
		if( !strcmp( job->job_id, job_id ) )
			break;
	if( job )
	 {
		fsd_mutex_lock( &job->mutex );
		fsd_assert( !(job->flags & FSD_JOB_DISPOSED) );
		job->ref_cnt ++;
	 }
	fsd_mutex_unlock( &self->mutex );
	if( job )
		fsd_log_return(( "(job_id=%s) =%p: ref_cnt=%d [lock %s]",
					job_id, (void*)job, job->ref_cnt, job->job_id ));
	else
		fsd_log_return(( "(job_id=%s) =NULL", job_id ));
	return job;
}


bool
fsd_job_set_empty( fsd_job_set_t *self )
{
	return self->n_jobs == 0;
}


fsd_job_t *
fsd_job_set_find_terminated( fsd_job_set_t *self )
{
	fsd_job_t *job = NULL;
	size_t i;
	fsd_mutex_t* volatile mutex = & self->mutex;

	fsd_log_enter(( "()" ));
	fsd_mutex_lock( mutex );
	TRY
	 {
		for( i = 0;  i < self->tab_size;  i++ )
			for( job = self->tab[ i ];  job;  job = job->next )
				if( job->state >= DRMAA_PS_DONE )
					goto found;
found:
		if( job )
		 {
			fsd_mutex_lock( &job->mutex );
			fsd_assert( !(job->flags & FSD_JOB_DISPOSED) );
			job->ref_cnt ++;
		 }
	 }
	FINALLY
	 { fsd_mutex_unlock( mutex ); }
	END_TRY
	if( job )
		fsd_log_return(( "() =%p: job_id=%s, ref_cnt=%d [lock %s]",
					(void*)job, job->job_id, job->ref_cnt, job->job_id ));
	else
		fsd_log_return(( "() =%p", (void*)job ));
	return job;
}


char **
fsd_job_set_get_all_job_ids( fsd_job_set_t *self )
{
	fsd_job_t *job = NULL;
	char** volatile job_ids = NULL;
	/* size_t n_jobs = 0, capacity = 0; */
	size_t i;
	unsigned j = 0;
	fsd_mutex_t* volatile mutex = & self->mutex;

	fsd_log_enter(( "" ));
	fsd_mutex_lock( mutex );
	TRY
	 {
		fsd_calloc( job_ids, self->n_jobs+1, char* );
		for( i = 0;  i < self->tab_size;  i++ )
			for( job = self->tab[ i ];  job;  job = job->next )
				job_ids[ j++ ] = fsd_strdup( job->job_id );
		fsd_realloc( job_ids, j+1, char* );
	 }
	FINALLY
	 {
		fsd_mutex_unlock( mutex );
		if( fsd_exc_get() )
			fsd_free_vector( job_ids );
	 }
	END_TRY

	fsd_log_return(( " =%p", (void*)job_ids ));
	return job_ids;
}


void
fsd_job_set_signal_all( fsd_job_set_t *self )
{
	fsd_job_t *volatile job = NULL;
	fsd_mutex_t *volatile mutex = & self->mutex;

	fsd_log_enter(( "" ));
	fsd_mutex_lock( mutex );
	TRY
	 {
		volatile size_t i;
		for( i = 0;  i < self->tab_size;  i++ )
			for( job = self->tab[ i ];  job;  job = job->next )
			 {
				fsd_mutex_lock( &job->mutex );
				TRY{ fsd_cond_broadcast( &job->status_cond ); }
				FINALLY{ fsd_mutex_unlock( &job->mutex ); }
				END_TRY
			 }
	 }
	FINALLY
	 { fsd_mutex_unlock( mutex ); }
	END_TRY

	fsd_log_return(( "" ));
}

