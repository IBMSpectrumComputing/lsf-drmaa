/* $Id: job.c 142 2010-05-01 20:18:54Z mamonski $ */
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
#include <stdlib.h>
#include <unistd.h>


#include <lsf/lsbatch.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/conf.h>
#include <drmaa_utils/datetime.h>
#include <drmaa_utils/drmaa.h>
#include <drmaa_utils/drmaa_util.h>
#include <drmaa_utils/environ.h>
#include <drmaa_utils/template.h>

#include <lsf_drmaa/job.h>
#include <lsf_drmaa/native.h>
#include <lsf_drmaa/session.h>
#include <lsf_drmaa/util.h>


#define max( x, y )  ( (x) > (y) ? (x) : (y) )


static void
lsfdrmaa_job_set_req(
		fsd_drmaa_session_t *session,
		fsd_expand_drmaa_ph_t *expand,
		const fsd_template_t *jt,
		struct submit *req,
		fsd_environ_t **envp
		);


static LS_LONG_INT
lsfdrmaa_job_id_atoi( const char *job_id_str )
{
	int job_id;
	int job_array_idx;
	const char *s = job_id_str;
	char *end;

	job_id = strtol( s, &end, 10 );
	if( s == end )
		goto error;
	s = end;
	switch( *s )
	 {
		case '\0':
			job_array_idx = 0;
			break;
		case '[':
			s++;
			job_array_idx = strtol( s, &end, 10 );
			if( s == end )
				goto error;
			s = end;
			if( *s++ == ']'  &&  *s++ == '\0' )
			 {}
			else
				goto error;
			break;
		default:
			goto error;
	 }
	return LSB_JOBID( job_id, job_array_idx );

error:
	fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_JOB,
		"invalid LSF job id: %s", job_id_str );
}


static void
lsfdrmaa_job_control( fsd_job_t *self, int action )
{
	/*
	 * XXX: waiting for job state change was removed
	 * since it is not required for drmaa_control
	 * to return after change completes.
	 */
	lsfdrmaa_job_t *lsf_self = (lsfdrmaa_job_t*)self;
	LS_LONG_INT job_id;
	int signal;

	fsd_log_enter(( "({job_id=%s}, action=%d)", self->job_id, action ));
	job_id = lsf_self->int_job_id;
	switch( action )
	 {
		case DRMAA_CONTROL_SUSPEND:
		case DRMAA_CONTROL_HOLD:
			signal = SIGSTOP;
			break;
		case DRMAA_CONTROL_RESUME:
		case DRMAA_CONTROL_RELEASE:
			signal = SIGCONT;
			break;
		case DRMAA_CONTROL_TERMINATE:
			/* TODO: sending SIGTERM (configurable)? */
			signal = SIGKILL;
			break;
		default:
			fsd_exc_raise_fmt(
					FSD_ERRNO_INVALID_ARGUMENT,
					"job::control: unknown action %d", action );
	 }

	fsd_mutex_lock( &self->session->drm_connection_mutex );
	TRY
	 {
		int rc = lsb_signaljob( lsf_self->int_job_id, signal );
		fsd_log_debug(( "lsb_signaljob( %d[%d], %d ) = %d",
					LSB_ARRAY_JOBID(lsf_self->int_job_id),
					LSB_ARRAY_IDX(lsf_self->int_job_id),
					signal, rc ));
		if( rc < 0 )
			fsd_exc_raise_fmt(
					FSD_ERRNO_INTERNAL_ERROR,
					"job::control: could not send %s to job %s",
					fsd_strsignal( signal ), self->job_id
					);
	 }
	FINALLY
	 {
		fsd_mutex_unlock( &self->session->drm_connection_mutex );
	 }
	END_TRY

	fsd_log_return(( "" ));
}

#define LSFDRMAA_MAX_QUEUE_SKEW (500)

static void
lsfdrmaa_job_update_status( fsd_job_t *self )
{
	lsfdrmaa_job_t *lsf_self = (lsfdrmaa_job_t*)self;
	struct jobInfoEnt *volatile job_info = NULL;
	bool job_in_queue;
	const bool lately_submitted =
		(self->flags & FSD_JOB_CURRENT_SESSION)
		&& self->state == DRMAA_PS_UNDETERMINED
		&& (time(NULL) - self->submit_time < LSFDRMAA_MAX_QUEUE_SKEW);

	fsd_log_enter(( "({job_id=%s, time_delta=%d})", self->job_id, time(NULL) - self->submit_time ));
	do {
		fsd_mutex_lock( &self->session->drm_connection_mutex );
		TRY
		 {
			int n_records;
			int more;
			fsd_log_debug(( "drm connection locked" ));

			n_records = lsb_openjobinfo( lsf_self->int_job_id,
						NULL, NULL, NULL, NULL, ALL_JOB );
			fsd_log_debug((
						"lsb_openjobinfo( %d[%d], NULL, NULL, NULL, NULL, ALL_JOB ) =%d",
						LSB_ARRAY_JOBID(lsf_self->int_job_id),
						LSB_ARRAY_IDX(lsf_self->int_job_id),
						n_records ));
			job_in_queue = n_records > 0;
			if( !job_in_queue  &&  !lately_submitted )
			 {
				if( n_records < 0 ) {
					if ((self->flags & FSD_JOB_CURRENT_SESSION) && (self->state == DRMAA_PS_UNDETERMINED)) {
						/*synchronize/wait called after CLEAN PERIOD */
						/* lsfdrmaa_openjobinfo_from_log(); */
						fsd_exc_raise_lsf( "lsb_openjobinfo" );
					} else {
						fsd_exc_raise_lsf( "lsb_openjobinfo" );
					}
				} else
					fsd_exc_raise_code( FSD_DRMAA_ERRNO_INVALID_JOB );
			 }

			if( job_in_queue )
			 {
				job_info = lsb_readjobinfo( &more );
				fsd_log_debug(( "lsb_readjobinfo(...) =%p: more=%d",
							(void*)job_info, more ));
				if( job_info == NULL )
					fsd_exc_raise_lsf( "lsb_readjobinfo" );
				lsf_self->read_job_info( self, job_info );
			 }else{
                             fsd_exc_raise_code( FSD_DRMAA_ERRNO_INVALID_JOB );
                         }
		 }
		FINALLY
		 {
			/* lsfdrmaa_free_job_info( job_info ); */
			lsb_closejobinfo();
			fsd_log_debug(( "lsb_closejobinfo()" ));
			fsd_mutex_unlock( &self->session->drm_connection_mutex );
		 }
		END_TRY

	} while( !job_in_queue );
	fsd_log_return(( "" ));
}


static void
lsfdrmaa_job_read_job_info( fsd_job_t *self, struct jobInfoEnt *job_info )
{
	int status, flags;

	fsd_log_enter(( "" ));
#ifdef DEBUGGING
	 {
		int i;
		fsd_log_debug(( "job status of %s updated from %d[%d]",
					self->job_id,
					LSB_ARRAY_JOBID(job_info->jobId),
					LSB_ARRAY_IDX(job_info->jobId) ));
		fsd_log_debug(( "\n  status: 0x%x", job_info->status ));
		fsd_log_debug(( "\n  submitTime: %ld", job_info->submitTime ));
		fsd_log_debug(( "\n  startTime: %ld", job_info->startTime ));
		fsd_log_debug(( "\n  endTime: %ld", job_info->startTime ));
		fsd_log_debug(( "\n  duration: %d", job_info->duration ));
		fsd_log_debug(( "\n  cpuTime: %f", job_info->cpuTime ));
		fsd_log_debug(( "\n  cwd: %s", job_info->cwd ));
		fsd_log_debug(( "\n  fromHost: %s", job_info->fromHost ));
		fsd_log_debug(( "\n  numExHosts: %d", job_info->numExHosts ));
		for( i = 0;  i < job_info->numExHosts;  i++ )
			fsd_log_debug(( "\n  exHosts[%d]: %s", i, job_info->exHosts[i] ));
		fsd_log_debug(( "\n  exitStatus: %d", job_info->exitStatus ));
		fsd_log_debug(( "\n  execCwd: %s", job_info->execCwd ));
		fsd_log_debug(( "\n  runRusage.mem: %d", job_info->runRusage.mem ));
		fsd_log_debug(( "\n  runRusage.swap: %d", job_info->runRusage.swap ));
		fsd_log_debug(( "\n  runRusage.utime: %d", job_info->runRusage.utime ));
		fsd_log_debug(( "\n  runRusage.stime: %d", job_info->runRusage.stime ));
		fsd_log_debug(( "\n  jName: %s", job_info->jName ));
		/* fsd_log_debug(( "\n  execRusage: %s", job_info->execRusage )); */
	 }
#endif /* DEBUGGING */

	status = job_info->status;

	flags = 0;
	if( status & (JOB_STAT_PEND | JOB_STAT_PSUSP) )
		flags |= FSD_JOB_QUEUED;
	if( status & JOB_STAT_PSUSP )
		flags |= FSD_JOB_HOLD;
	if( status & (JOB_STAT_RUN | JOB_STAT_USUSP | JOB_STAT_SSUSP) )
		flags |= FSD_JOB_RUNNING;
	if( status & (JOB_STAT_USUSP | JOB_STAT_SSUSP) )
		flags |= FSD_JOB_SUSPENDED;
	if( status & (JOB_STAT_DONE | JOB_STAT_EXIT) )
		flags |= FSD_JOB_TERMINATED;
	if( status & (JOB_STAT_EXIT | JOB_STAT_PERR) )
		flags |= FSD_JOB_ABORTED;
	self->flags &= ~(FSD_JOB_STATE_MASK | FSD_JOB_ABORTED);
	self->flags |= flags;

	if( status & (JOB_STAT_WAIT | JOB_STAT_PEND) )
		self->state = DRMAA_PS_QUEUED_ACTIVE;
	else if( status & JOB_STAT_PSUSP )
		self->state = DRMAA_PS_USER_ON_HOLD;
	else if( status & JOB_STAT_RUN )
		self->state = DRMAA_PS_RUNNING;
	else if( status & JOB_STAT_SSUSP )
		self->state = DRMAA_PS_SYSTEM_SUSPENDED;
	else if( status & JOB_STAT_USUSP )
		self->state = DRMAA_PS_USER_SUSPENDED;
	else if( status & JOB_STAT_DONE )
		self->state = DRMAA_PS_DONE;
	else if( status & JOB_STAT_EXIT )
		self->state = DRMAA_PS_FAILED;
	else if( status & JOB_STAT_PDONE )
		self->state = DRMAA_PS_DONE;
	else if( status & JOB_STAT_PERR )
		self->state = DRMAA_PS_FAILED;
	else if( status & JOB_STAT_UNKWN )
		self->state = DRMAA_PS_UNDETERMINED;
	else
		self->state = DRMAA_PS_FAILED;

	self->exit_status = job_info->exitStatus & ~0xff;
	if( (self->exit_status >> 8) == 0  &&  (job_info->status & JOB_STAT_EXIT) )
		self->exit_status |= 0x01;
	self->start_time = job_info->startTime;
	self->end_time = job_info->endTime;
	self->cpu_usage = job_info->cpuTime;
	self->mem_usage = max( self->mem_usage, 1024*job_info->runRusage.mem );
	self->vmem_usage = max( self->vmem_usage, 1024*job_info->runRusage.swap );
	self->walltime = 60*job_info->duration;
	self->n_execution_hosts = job_info->numExHosts;
	if( self->execution_hosts == NULL  &&  job_info->exHosts != NULL )
		self->execution_hosts
			= fsd_explode( (const char*const*)job_info->exHosts, ' ',
					job_info->numExHosts );
	self->last_update_time = time(NULL);
	if( self->state >= DRMAA_PS_DONE )
		fsd_cond_broadcast( &self->status_cond );
	fsd_log_return(( "" ));
}


fsd_job_t *
lsfdrmaa_job_new( char *job_id )
{
	lsfdrmaa_job_t *self = NULL;
	LS_LONG_INT int_job_id;
	int_job_id = lsfdrmaa_job_id_atoi( job_id );
	self = (lsfdrmaa_job_t*)fsd_job_new( job_id );
	fsd_realloc( self, 1, lsfdrmaa_job_t );
	self->super.control = lsfdrmaa_job_control;
	self->super.update_status = lsfdrmaa_job_update_status;
	self->read_job_info = lsfdrmaa_job_read_job_info;
	self->int_job_id = int_job_id;
	return (fsd_job_t*)self;
}


struct submit *
lsfdrmaa_job_create_req(
		fsd_drmaa_session_t *session,
		const fsd_template_t *jt, fsd_environ_t **envp
		)
{
	struct submit *volatile req = NULL;
	fsd_expand_drmaa_ph_t *volatile expand = NULL;
	TRY
	 {
		fsd_malloc( req, struct submit );
		memset( req, 0, sizeof(struct submit) );
		expand = fsd_expand_drmaa_ph_new( NULL, NULL, fsd_strdup("%I") );
		lsfdrmaa_job_set_req( session, expand, jt, req, envp );
	 }
	EXCEPT_DEFAULT
	 {
		lsfdrmaa_free_submit_req( req, true );
		fsd_exc_reraise();
	 }
	FINALLY
	 {
		if( expand )
			expand->destroy( expand );
	 }
	END_TRY
	return req;
}


static char *
internal_map_file( fsd_expand_drmaa_ph_t *expand, const char *path,
		bool *host_given, const char *name )
{
	const char *p;
	for( p = path;  *p != ':';  p++ )
		if( *p == '\0' )
			fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
							"invalid format of drmaa_%s_path: missing colon", name );
	if( host_given )
		*host_given = ( p != path );
	p++;
	return expand->expand( expand, fsd_strdup(p),
			FSD_DRMAA_PH_HD | FSD_DRMAA_PH_WD | FSD_DRMAA_PH_INCR );
}


static char *
lsfdrmaa_job_quote_command( const char *const *argv )
{
	char *volatile result = NULL;
	TRY
	 {
		size_t size = 0;
		const char *const *i;
		const char *j;
		char *s;
		for( i = argv;  *i;  i++ )
		 {
			if( i != argv )
				size++;
			size += 2;
			for( j = *i;  *j;  j++ )
				switch( *j )
				 {
					case '"':  case '$':  case '\\':  case '`':
						size ++;
					default:
						size ++;
						break;
				 }
		 }
		fsd_calloc( result, size+1, char );
		s = result;
		for( i = argv;  *i;  i++ )
		 {
			if( i != argv )
				*s++ = ' ';
			*s++ = '"';
			for( j = *i;  *j;  j++ )
				switch( *j )
				 {
					case '"':  case '$':  case '\\':  case '`':
						*s++ = '\\';
					default:
						*s++ = *j;
						break;
				 }
			*s++ = '"';
		 }
		*s++ = '\0';
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( result );
		fsd_exc_reraise();
	 }
	END_TRY
	return result;
}



void
lsfdrmaa_job_set_req(
		fsd_drmaa_session_t *session,
		fsd_expand_drmaa_ph_t *expand,
		const fsd_template_t *jt,
		struct submit *req,
		fsd_environ_t **envp
		)
{
	const char *input_path_orig = NULL;
	const char *output_path_orig = NULL;
	const char *error_path_orig = NULL;
	char *volatile input_path = NULL;
	char *volatile output_path = NULL;
	char *volatile error_path = NULL;
	bool input_host = false;
	bool output_host = false;
	bool error_host = false;
	bool join_files = false;
	bool transfer_input = false;
	bool transfer_output = false;
	bool transfer_error = false;
	const char *job_category = "default";
	char **volatile argv = NULL;

	const char *value;
	const char *const *vector;

	/* set default lsf configs */
	 {
		int i = 0;
		req->options = 0;
		req->options2 = 0;
		for( i = 0;  i < LSF_RLIM_NLIMITS;  i++ )
			req->rLimits[i] = DEFAULT_RLIMIT;
		req->beginTime = 0;
		req->termTime = 0;
	 }

	/* job category */
	value = jt->get_attr( jt, DRMAA_JOB_CATEGORY );
	if( value )
		job_category = value;

	 {
		fsd_conf_option_t *category_value = NULL;
		category_value = fsd_conf_dict_get( session->job_categories, job_category );
		if( category_value != NULL )
		 {
			if( category_value->type != FSD_CONF_STRING )
				fsd_exc_raise_fmt(
						FSD_ERRNO_INTERNAL_ERROR,
						"configuration error: job category should be string"
						);
			lsfdrmaa_native_parse( category_value->val.string, req );
		 }
		else
		 {
			if( value != NULL )
				fsd_exc_raise_fmt(
						FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
						"invalid job category: %s", job_category
						);
		 }
	 }

	/* job working directory */
	value = jt->get_attr( jt, DRMAA_WD );
	if( value )
	 {
		char *cwd = NULL;
		cwd = expand->expand( expand, fsd_strdup(value),
				FSD_DRMAA_PH_HD | FSD_DRMAA_PH_INCR );
		expand->set( expand, FSD_DRMAA_PH_WD, cwd );
#ifdef SUB3_CWD
		req->cwd = fsd_strdup( cwd );
		req->options3 |= SUB3_CWD;
#else
		fsd_exc_raise_fmt(FSD_ERRNO_INTERNAL_ERROR, "DRMAA_WD attribute is not supported in this version of LSF.");
#endif
	 }

	TRY
	 {
		const char *command = NULL;
		unsigned n_args = 0;
		const char *const *i;
		int j;

		/* remote command */
		command = jt->get_attr( jt, DRMAA_REMOTE_COMMAND );
		if( command == NULL )
			fsd_exc_raise_msg(
					FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES,
					"drmaa_remote_command not set for job template"
					);

		/* arguments list */
		vector = jt->get_v_attr( jt, DRMAA_V_ARGV );
		if( vector )
		 {
			for( i = vector;  *i;  i++ )
				n_args++;
		 }
		fsd_calloc( argv, n_args+3, char* );
		argv[0] = fsd_strdup("exec");
		argv[1] = expand->expand( expand, fsd_strdup(command),
				FSD_DRMAA_PH_HD | FSD_DRMAA_PH_WD );
		if( vector )
		 {
			for( i = vector, j = 2;  *i;  i++, j++ )
				argv[j] = expand->expand( expand, fsd_strdup(*i),
					FSD_DRMAA_PH_HD | FSD_DRMAA_PH_WD );
		 }

		req->command = lsfdrmaa_job_quote_command( (const char*const*)argv );
	 }
	FINALLY
	 {
		fsd_free_vector( argv );
	 }
	END_TRY

	/* job name */
	value = jt->get_attr( jt, DRMAA_JOB_NAME );
	if( value )
	 {
		req->jobName = fsd_strdup(value);
		req->options |= SUB_JOB_NAME;
	 }

	/* job state at submit */
	value = jt->get_attr( jt, DRMAA_JS_STATE );
	if( value )
	 {
		if( 0 == strcmp( value, DRMAA_SUBMISSION_STATE_ACTIVE ) )
			req->options2 &= !SUB2_HOLD;
		else if( 0 == strcmp( value, DRMAA_SUBMISSION_STATE_HOLD ) )
			req->options2 |= SUB2_HOLD;
		else
			fsd_exc_raise_msg(
					FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
					"invalid value of drmaa_js_state attribute" );
	 }

	/* environment */
	vector = jt->get_v_attr( jt, DRMAA_V_ENV );
	if( vector )
	 {
		fsd_environ_t *env;
		*envp = env = fsd_environ_new( NULL );
		env->update( env, vector );
	 }

	/* start time */
	value = jt->get_attr( jt, DRMAA_START_TIME );
	if( value )
	 {
		req->beginTime = fsd_datetime_parse( value );
		fsd_log_debug(( "\n  drmaa_start_time: %s -> %ld",
					value, (long)req->beginTime ));
	 }

	TRY
	 {
		/* input path */
		input_path_orig = jt->get_attr( jt, DRMAA_INPUT_PATH );
		if( input_path_orig )
		 {
			input_path = internal_map_file( expand, input_path_orig, &input_host,
							"input" );
			fsd_log_debug(( "\n  drmaa_input_path: %s -> %s",
						input_path_orig, input_path ));
		 }

		/* output path */
		output_path_orig = jt->get_attr( jt, DRMAA_OUTPUT_PATH );
		if( output_path_orig )
		 {
			output_path = internal_map_file( expand, output_path_orig, &output_host,
							"output" );
			fsd_log_debug(( "\n  drmaa_output_path: %s -> %s",
						output_path_orig, output_path ));
		 }

		/* error path */
		error_path_orig = jt->get_attr( jt, DRMAA_ERROR_PATH );
		if( error_path_orig )
		 {
			error_path = internal_map_file( expand, error_path_orig, &error_host,
							"error" );
			fsd_log_debug(( "\n  drmaa_error_path: %s -> %s",
						error_path_orig, error_path ));
		 }

		/* join files */
		value = jt->get_attr( jt, DRMAA_JOIN_FILES );
		if( value )
		 {
			if( (value[0] == 'y' || value[0] == 'Y')  &&  value[1] == '\0' )
				join_files = true;
			else if( (value[0] == 'n' || value[0] == 'N')  &&  value[1] == '\0' )
				join_files = false;
			else
				fsd_exc_raise_msg(
						FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
						"invalid value of drmaa_join_files attribute" );
		 }

		if( join_files )
		 {
			/*
			 * LSF by default joins output and error streams
			 * when error file is not set.
			 */
			if( error_path )
			 {
				if( output_path == NULL )
					fsd_exc_raise_msg(
							FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES,
							"drmaa_join_files is set and output file is not given" );
				if( 0 != strcmp( output_path, error_path ) )
					fsd_log_warning(( "Error file was given but will be ignored "
								"since drmaa_join_files was set." ));
				fsd_free( error_path );  error_path = NULL;
			 }
		 }
		else
		 {
			/*
			 * If error path is not set, we must set it to /dev/null
			 * to prevent joining files.
			 */
			if( error_path == NULL  &&  output_path )
				error_path = fsd_strdup( "/dev/null" );
			if( output_path == NULL  &&  error_path )
				output_path = fsd_strdup( "/dev/null" );
			if( req->errFile == NULL )
			 {
				req->errFile = fsd_strdup( "/dev/null" );
				req->options |= SUB_ERR_FILE;
	#ifdef SUB2_OVERWRITE_ERR_FILE
				req->options2 &= ~SUB2_OVERWRITE_ERR_FILE;
	#endif
			 }
		 }

		/* transfer files */
		value = jt->get_attr( jt, DRMAA_TRANSFER_FILES );
		if( value )
		 {
			const char *i;
			for( i = value;  *i;  i++ )
			 {
				switch( *i )
				 {
					case 'i':  transfer_input = true;  break;
					case 'o':  transfer_output = true;  break;
					case 'e':  transfer_error = true;  break;
					default:
						fsd_exc_raise_fmt(
								FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
								"invalid character '%c' in drmaa_transfer_files: %s",
								*i, value
								);
				 }
			 }
		 }

#	if 0
		 {
			/*
			 * Input file is transfered by LSF from submission host whenever
			 * it isn't found on execution host regardless of explicit file transfers.
			 * When drmaa_transfer_files contains ``i`` input file is send
			 * explicitly because it may be outdated or otherwise differ.
			 */
			static const char *name[3]
				= {"input", "output", "error"};
			static const int options[3]
				= { XF_OP_SUB2EXEC, XF_OP_EXEC2SUB, XF_OP_EXEC2SUB };
			const char *path[3];
			bool host[3], transfer[3];
			int i;

			path[i=0] = input_path;
			path[++i] = output_path;
			path[++i] = error_path;
			host[i=0] = input_host;
			host[++i] = output_host;
			host[++i] = error_host;
			transfer[i=0] = transfer_input;
			transfer[++i] = transfer_output;
			transfer[++i] = transfer_error;

			for( i = 0;  i < 3;  i++ )
			 {
				struct xFile *t;
				if( !(transfer[i]  &&  path[i] != NULL) )
					continue;
				if( 0 == strcmp( path[i], "/dev/null" ) )
					continue;
				if( host[i] )
					fsd_log_warning((
								"hostname in drmaa_%s_path ignored", name[i] ));
				fsd_log_debug(( "setting transfer of %s file (%s) "
							"to execution host", name[i], path[i] ));
				fsd_realloc( req->xf, req->nxf+1, struct xFile );
				t = &req->xf[ req->nxf++ ];
				memset( t, 0, sizeof(struct xFile) );
				if( sizeof(t->subFn) == MAXFILENAMELEN )
				 { /* LSF 6 */
					strlcpy( t->subFn, path[i], MAXFILENAMELEN );
					strlcpy( t->execFn, path[i], MAXFILENAMELEN );
				 }
				else
				 { /* LSF 7 */
					*(char**)&t->subFn = fsd_strdup( path[i] );
					*(char**)&t->execFn = fsd_strdup( path[i] );
				 }
				t->options = options[i];
			 }

			if( req->nxf > 0 )
				req->options |= SUB_OTHER_FILES;
		 }
#	endif /* transfer files */

		/* email addresses to send notifications */
		vector = jt->get_v_attr( jt, DRMAA_V_EMAIL );
		if( vector  &&  vector[0] )
		 {
			/* only to one email address message may be send */
			req->mailUser = fsd_strdup( vector[0] );
			req->options |= SUB_MAIL_USER | SUB_NOTIFY_END;
	#if 0
			if( vector[1] != NULL )
				fsd_log_warning(( "LSF only supports one e-mail "
							"notification address" ));
	#endif
		 }

		/* block email */
		value = jt->get_attr( jt, DRMAA_BLOCK_EMAIL );
		if( value )
		 {
			bool block;
			if( strcmp(value, "1") == 0 )
				block = true;
			else if( strcmp(value, "0") == 0 )
				block = false;
			else
				fsd_exc_raise_msg(
						FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
						"invalid value of drmaa_block_email attribute" );
			if( block )
			 {
				if( output_path == NULL )
				 {
					fsd_log_debug(( "output path not set and we want to block e-mail, "
								"set to /dev/null" ));
					output_path = fsd_strdup( "/dev/null" );
				 }
				req->options &= ~SUB_NOTIFY_END;
			 }
			else
			 {
				/* SUB_NOTIFY_END should force sending e-mail even if outfile is set */
				req->options |= SUB_NOTIFY_END;
			 }
		 }

		if( !((lsfdrmaa_session_t*)session)->prepand_report_to_output
				&&  (req->options & SUB_NOTIFY_END) == 0
				&&  output_path != NULL )
		 {
			req->options |= SUB_MAIL_USER | SUB_NOTIFY_END;
			fsd_free( req->mailUser );  /* when email was set
				but notification was blocked */
			req->mailUser = fsd_strdup( "notexistent" );
		 }

		if( input_path )
		 {
			req->inFile = input_path;
			req->options |= SUB_IN_FILE;
			input_path = NULL;
		 }

		if( output_path )
		 {
			req->outFile = output_path;
			req->options |= SUB_OUT_FILE;
	#ifdef SUB2_OVERWRITE_OUT_FILE
			if( 0 != strcmp( output_path, "/dev/null" ) )
				req->options2 |= SUB2_OVERWRITE_OUT_FILE;
	#endif
			output_path = NULL;
		 }

		if( error_path )
		 {
			req->errFile = error_path;
			req->options |= SUB_ERR_FILE;
	#ifdef SUB2_OVERWRITE_ERR_FILE
			if( 0 != strcmp( error_path, "/dev/null" ) )
				req->options2 |= SUB2_OVERWRITE_ERR_FILE;
	#endif
			error_path = NULL;
		 }
	 }
	FINALLY
	 {
		fsd_free( input_path );
		fsd_free( output_path );
		fsd_free( error_path );
	 }
	END_TRY


	/* deadline time */
	value = jt->get_attr( jt, DRMAA_DEADLINE_TIME );
	if( value )
		req->termTime = fsd_datetime_parse( value );

	/* wall clock time hard limit */
	value = jt->get_attr( jt, DRMAA_WCT_HLIMIT );
	if( value )
		req->rLimits[ LSF_RLIMIT_RUN ] = fsd_parse_timedelta( value );

	/* wall clock time soft limit */
#ifdef SUB3_RUNTIME_ESTIMATION
	value = jt->get_attr( jt, DRMAA_WCT_SLIMIT );
	if( value )
	 {
		req->options3 = SUB3_RUNTIME_ESTIMATION;
		req->runtimeEstimation = fsd_parse_timedelta( value );
	 }
#endif

	/* duration hard limit */
	value = jt->get_attr( jt, DRMAA_DURATION_HLIMIT );
	if( value )
		req->rLimits[ LSF_RLIMIT_CPU ] = fsd_parse_timedelta( value );

	/* native specification */
	value = jt->get_attr( jt, DRMAA_NATIVE_SPECIFICATION );
	if( value )
		lsfdrmaa_native_parse( value, req );

	lsfdrmaa_dump_submit_req(req);
}

