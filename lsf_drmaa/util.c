/* $Id: util.c 64 2010-02-23 23:04:02Z mamonski $ */
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

#include <lsf/lsf.h>
#include <drmaa_utils/common.h>
#include <drmaa_utils/exception.h>
#include <lsf_drmaa/util.h>
#include <string.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: util.c 64 2010-02-23 23:04:02Z mamonski $";
#endif


#define offset( type, field ) \
		( (int)(long)&((type*)0)->field )


struct jobInfoEnt *
lsfdrmaa_copy_job_info( const struct jobInfoEnt *job_info )
{
	struct jobInfoEnt *volatile result = NULL;
	fsd_malloc( result, struct jobInfoEnt );
	memset( result, 0, sizeof(struct jobInfoEnt) );
	TRY
	 {
		memcpy( result, job_info, offset(struct jobInfoEnt, counter) );
		result->user = NULL;
		result->reasonTb = NULL;
		result->numReasons = 0;
		result->cwd = NULL;
		result->subHomeDir = NULL;
		result->fromHost = NULL;
		result->numExHosts = 0;
		result->exHosts = NULL;
		result->loadSched = NULL;
		result->loadStop = NULL;
		result->execHome = NULL;
		result->execCwd = NULL;
		result->execUsername = NULL;
		result->runRusage.npids = 0;
		result->runRusage.pidInfo = NULL;
		result->runRusage.npgids = 0;
		result->runRusage.pgid = NULL;
		result->parentGroup = NULL;
		result->jName = NULL;

		result->user = fsd_strdup( job_info->user );
		result->cwd = fsd_strdup( job_info->cwd );
		if( job_info->numExHosts > 0 )
		 {
			int i;
			fsd_calloc( result->exHosts, job_info->numExHosts, char* );
			result->numExHosts = job_info->numExHosts;
			for( i = 0;  i < job_info->numExHosts;  i++ )
				result->exHosts[i] = fsd_strdup( job_info->exHosts[i] );
		 }
		memset( &result->submit, 0, sizeof(result->submit) );
		result->execHome = fsd_strdup( job_info->execHome );
		result->execCwd = fsd_strdup( job_info->execCwd );
		result->execUsername = fsd_strdup( job_info->execUsername );
		result->jName = fsd_strdup( job_info->jName );
	 }
	EXCEPT_DEFAULT
	 {
		lsfdrmaa_free_job_info( result );
		fsd_exc_reraise();
	 }
	END_TRY
	return result;
}

void
lsfdrmaa_free_job_info( struct jobInfoEnt *job_info )
{
	static const int released_pointers[] = {
		offset( struct jobInfoEnt, user ),
		offset( struct jobInfoEnt, reasonTb ),
		offset( struct jobInfoEnt, cwd ),
		offset( struct jobInfoEnt, subHomeDir ),
		offset( struct jobInfoEnt, fromHost ),
		offset( struct jobInfoEnt, loadSched ),
		offset( struct jobInfoEnt, loadStop ),
		offset( struct jobInfoEnt, execHome ),
		offset( struct jobInfoEnt, execCwd ),
		offset( struct jobInfoEnt, execUsername ),
		offset( struct jobInfoEnt, parentGroup ),
		offset( struct jobInfoEnt, jName ),
		offset( struct jobInfoEnt, detailReason ),
		offset( struct jobInfoEnt, additionalInfo ),
		offset( struct jobInfoEnt, warningAction ),
		offset( struct jobInfoEnt, chargedSAAP ),
#if LSF_PRODUCT_MAJOR_VERSION >= 7
		offset( struct jobInfoEnt, execRusage ),
#endif
		-1 /* sentinel */
	};
	int i;
	if( job_info == NULL )
		return;
	lsfdrmaa_free_submit_req( &job_info->submit, false );
	for( i = 0;  i < job_info->numExHosts;  i++ )
		fsd_free( job_info->exHosts[i] );
	fsd_free( job_info->exHosts );
	for( i = 0;  i < job_info->numExternalMsg;  i++ )
		fsd_free( job_info->externalMsg[i] );
	fsd_free( job_info->externalMsg );
#ifdef SUB2_LICENSE_PROJECT /* FIXME: is this correct? */
	for( i = 0;  i < job_info->numLicense;  i++ )
		fsd_free( job_info->licenseNames[i] );
	fsd_free( job_info->licenseNames );
#endif
	for( i = 0;  released_pointers[i] >= 0;  i++ )
	 {
		void *ptr = *(void**)((char*)job_info + released_pointers[i]);
		if( ptr )
			fsd_free( ptr );
	 }
	fsd_free( job_info );
}

void
lsfdrmaa_free_submit_req( struct submit *req, bool free_req )
{
	static const int released_pointers[] = {
	  offset( struct submit, jobName ),
	  offset( struct submit, queue ),
	  offset( struct submit, resReq ),
	  offset( struct submit, hostSpec ),
	  offset( struct submit, dependCond ),
	  offset( struct submit, timeEvent ),
	  offset( struct submit, inFile ),
	  offset( struct submit, outFile ),
	  offset( struct submit, errFile ),
	  offset( struct submit, command ),
	  offset( struct submit, newCommand ),
	  offset( struct submit, chkpntDir ),
	  offset( struct submit, preExecCmd ),
	  offset( struct submit, mailUser ),
	  offset( struct submit, projectName ),
	  offset( struct submit, loginShell ),
	  offset( struct submit, userGroup ),
	  offset( struct submit, exceptList ),
	  offset( struct submit, rsvId ),
	  offset( struct submit, jobGroup ),
	  offset( struct submit, sla ),
	  offset( struct submit, extsched ),
	  offset( struct submit, warningAction ),
#ifdef SUB2_LICENSE_PROJECT
	  offset( struct submit, licenseProject ),
#endif
#ifdef SUB3_APP
	  offset( struct submit, app ),
		/* FIXME: is this correct: */
	  offset( struct submit, jsdlDoc ),
	  offset( struct submit, apsString ),
#endif
#ifdef SUB3_POST_EXEC
	  offset( struct submit, postExecCmd ),
#endif
#ifdef SUB3_CWD
	  offset( struct submit, cwd ),
#endif
		-1 /* sentinel */
	};
	int i;

	if( req == NULL )
		return;

	if( req->askedHosts != NULL )
	 {
		for( i = 0;  i < req->numAskedHosts;  i++ )
			fsd_free( req->askedHosts[i] );
		fsd_free( req->askedHosts );
	 }

	if( sizeof(req->xf[0].subFn) != MAXFILENAMELEN )
		for( i = 0;  i < req->nxf;  i++ )
		 {
			fsd_free( req->xf[i].subFn );
			fsd_free( req->xf[i].execFn );
		 }
	fsd_free( req->xf );

	for( i = 0;  released_pointers[i] >= 0;  i++ )
	 {
		void *ptr = *(void**)((char*)req + released_pointers[i]);
		if( ptr )
			fsd_free( ptr );
	 }

	if( free_req )
		fsd_free( req );
}


void
lsfdrmaa_dump_submit_req( struct submit *req )
{
	int options = req->options;
	int options2 = req->options2;
#ifdef SUB3_APP
	int options3 = req->options3;
#endif
	int i;

	fsd_log_debug(( "\n  command: %s", req->command ));
	if( options & SUB_JOB_NAME )
		fsd_log_debug(( "\n  jobName: %s", req->jobName ));
	if( options & SUB_QUEUE )
		fsd_log_debug(( "\n  queue: %s", req->queue ));
	for( i = 0;  i < req->numAskedHosts;  i++ )
		fsd_log_debug(( "\n  askedHosts[%d]: %s", i, req->askedHosts[i] ));
	if( options & SUB_RES_REQ )
		fsd_log_debug(( "\n  resReq: %s", req->resReq ));
	/*
	for( i = 0;  i < LSF_RLIM_NLIMITS;  i++ )
		fsd ...
	*/
	fsd_log_debug(( "\n  numProcessors: %d", req->numProcessors ));
	fsd_log_debug(( "\n  maxNumProcessors: %d", req->maxNumProcessors ));
	if( options & SUB_DEPEND_COND )
		fsd_log_debug(( "\n  dependCond: %s", req->dependCond ));
	if( req->beginTime )
		fsd_log_debug(( "\n  beginTime: %ld", (long)req->beginTime ));
	if( req->termTime )
		fsd_log_debug(( "\n  termTime: %ld", (long)req->termTime ));
	if( req->sigValue )
		fsd_log_debug(( "\n  sigValue: %d", req->sigValue ));
	if( options & SUB_IN_FILE )
		fsd_log_debug(( "\n  inFile: %s%s", req->inFile,
					(options2 & SUB2_IN_FILE_SPOOL) ? " [spool]": "" ));
	if( options & SUB_OUT_FILE )
#ifdef SUB2_OVERWRITE_OUT_FILE
		fsd_log_debug(( "\n  outFile: %s%s", req->outFile,
					(options2 & SUB2_OVERWRITE_OUT_FILE) ? " [overwrite]" : "" ));
#else
		fsd_log_debug(( "\n  outFile: %s", req->outFile ));
#endif
	if( options & SUB_ERR_FILE )
#ifdef SUB2_OVERWRITE_ERR_FILE
		fsd_log_debug(( "\n  errFile: %s%s", req->errFile,
					(options2 & SUB2_OVERWRITE_ERR_FILE) ? " [overwrite]" : "" ));
#else
		fsd_log_debug(( "\n  errFile: %s", req->errFile ));
#endif
	for( i = 0;  i < req->nxf;  i++ )
	 {
		fsd_log_debug(( "\n  xf[%d].subFn: %s", i, req->xf[i].subFn ));
		fsd_log_debug(( "\n  xf[%d].execFn: %s", i, req->xf[i].execFn ));
		fsd_log_debug(( "\n  xf[%d].options: 0x%x", i, req->xf[i].options ));
	 }
	if( options & SUB_PRE_EXEC )
		fsd_log_debug(( "\n  preExecCmd: %s", req->preExecCmd ));
#ifdef SUB3_POST_EXEC
	if( options3 & SUB3_POST_EXEC )
		fsd_log_debug(( "\n  postExecCmd: %s", req->postExecCmd ));
#endif
	if( options & SUB_MAIL_USER )
		fsd_log_debug(( "\n  mailUser: %s", req->mailUser ));
	if( options & SUB_PROJECT_NAME )
		fsd_log_debug(( "\n  projectName: %s", req->projectName ));
	if( options & SUB_LOGIN_SHELL )
		fsd_log_debug(( "\n  loginShell: %s", req->loginShell ));
	if( options & SUB_USER_GROUP )
		fsd_log_debug(( "\n  userGroup: %s", req->userGroup ));
	if( options2 & SUB2_JOB_PRIORITY )
		fsd_log_debug(( "\n  userPriority: %d", req->userPriority ));
	if( options2 & SUB2_USE_RSV )
		fsd_log_debug(( "\n  rsvId: %s", req->rsvId ));
	if( options2 & SUB2_JOB_GROUP )
		fsd_log_debug(( "\n  jobGroup: %s", req->jobGroup ));
	if( options2 & SUB2_SLA )
		fsd_log_debug(( "\n  sla: %s", req->sla ));
	if( options2 & SUB2_EXTSCHED )
		fsd_log_debug(( "\n  extsched: %s", req->extsched ));
	if( options2 & SUB2_WARNING_TIME_PERIOD )
		fsd_log_debug(( "\n  warningTimePeriod: %d", req->warningTimePeriod ));
	if( options2 & SUB2_WARNING_ACTION )
		fsd_log_debug(( "\n  warningAction: %s", req->warningAction ));
#ifdef SUB2_LICENSE_PROJECT
	if( options2 & SUB2_LICENSE_PROJECT )
		fsd_log_debug(( "\n  licenseProject: %s", req->licenseProject ));
#endif
#ifdef SUB3_APP
	if( options2 & SUB3_APP )
		fsd_log_debug(( "\n  app: %s", req->app ));
	if( req->jsdlFlag != -1 )
		fsd_log_debug(( "\n  jsdlDoc: %s%s", req->jsdlDoc,
					req->jsdlFlag ? " [strict]" : "" ));
#endif
#ifdef SUB3_CWD
	if( options3 & SUB3_CWD )
		fsd_log_debug(( "\n  cwd: %s", req->cwd ));
#endif
#if SUB3_RUNTIME_ESTIMATION
	if( options3 & SUB3_RUNTIME_ESTIMATION )
		fsd_log_debug(( "\n  runtimeEstimation: %d", req->runtimeEstimation ));
#endif
#ifdef SUB3_MIG_THRESHOLD
	if( options3 & SUB3_MIG_THRESHOLD )
		fsd_log_debug(( "\n  migThreshold: %d", req->migThreshold ));
#endif
}


void
fsd_exc_raise_lsf( const char *function )
{
	int lsf_errno;
	int fsd_errno;
	const char *message;

	lsf_errno = lsberrno;
	message = lsb_sysmsg();
	fsd_errno = lsfdrmaa_map_lsberrno( lsf_errno );
	fsd_log_error((
				"call to %s returned with error %d:%s mapped to DRMAA error %d:%s",
				function,
				lsf_errno, message,
				fsd_errno, fsd_strerror(fsd_errno)
				));
	fsd_exc_raise_fmt(
			fsd_errno,
			"%s: %s (%d)", function, message, lsf_errno
			);
}


int
lsfdrmaa_map_lsberrno( int _lsberrno )
{
	switch( _lsberrno )
	 {
		case LSBE_NO_ERROR:         /* No error at all */
			return FSD_ERRNO_SUCCESS;

		case LSBE_NO_JOB:           /* No matching job found */
		case LSBE_BAD_IDX:          /* Bad index */
		case LSBE_BIG_IDX:          /* Index too big */
		case LSBE_ARRAY_NULL:       /* Job array not exist*/
		case LSBE_BAD_JOBID:        /* Bad jobId */
			return FSD_DRMAA_ERRNO_INVALID_JOB;

		/* Job's inconsitnet state: */
		case LSBE_NOT_STARTED:      /* Job not started yet */
		case LSBE_JOB_STARTED:      /* Job already started */
		case LSBE_JOB_FINISH:       /* Job already finished */
		case LSBE_STOP_JOB:         /* Ask sbatchd to stop the wrong job */
		case LSBE_MIGRATION:        /* Job is already being migrated */
		case LSBE_J_UNCHKPNTABLE:   /* Job is not chkpntable */
		case LSBE_J_UNREPETITIVE:   /* Job is not a repetitive job */
		case LSBE_NO_OUTPUT:        /* Job has no output so far */
		case LSBE_PEND_CAL_JOB:     /* Job can not be killed in pending */
		case LSBE_RUN_CAL_JOB:      /* This Running turn is being terminated */
		case LSBE_JOB_MODIFY:       /* The job's params cannot be changed */
		case LSBE_JOB_MODIFY_ONCE:  /* The changed once parameters are not used */
			return FSD_ERRNO_INTERNAL_ERROR;

		case LSBE_DEPEND_SYNTAX:    /* Depend_cond syntax error */
			return FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT;

		case LSBE_EXCLUSIVE:        /* Queue doesn't accept EXCLUSIVE job */
		case LSBE_ROOT:             /* Root is not allowed to submit jobs */
		case LSBE_ONLY_INTERACTIVE: /* Queue only accepts bsub -I job */
		case LSBE_NO_INTERACTIVE:   /* Queue doesn't accept bsub -I job */
		case LSBE_BAD_QUEUE:        /* No such queue in the system */
		case LSBE_QUEUE_NAME:       /* Queue name should be given */
		case LSBE_QUEUE_CLOSED:     /* Queue has been closed */
		case LSBE_QUEUE_WINDOW:     /* Queue windows are closed */
		case LSBE_BAD_HOST:         /* Bad host name or host group name" */
		case LSBE_PROC_NUM:         /* Too many processors requested */
		case LSBE_BAD_HPART:        /* Bad host partition name */
		case LSBE_BAD_GROUP:        /* Bad host/user group name */
		case LSBE_QUEUE_HOST:       /* Host is not used by the queue */
		case LSBE_UJOB_LIMIT:       /* User reach UJOB_LIMIT of the queue */
		case LSBE_NO_HOST:          /* No host available for migration */
		case LSBE_PJOB_LIMIT:       /* User reach PJOB_LIMIT of the queue */
		case LSBE_NOLSF_HOST:       /* Request from non LSF host rejected */
		case LSBE_OVER_LIMIT:       /* Over hard limit of queue */
		case LSBE_USER_JLIMIT:      /* User has no enough job slots */
		case LSBE_CAL_DISABLED:     /* Calendar function is not enabled */
		case LSBE_HJOB_LIMIT:       /* User reach HJOB_LIMIT of the queue */
		case LSBE_NO_ENOUGH_HOST:   /* Not enough hosts */
		case LSBE_CAL_MODIFY:       /* CONF used calendar cannot be modified */
		case LSBE_JOB_CAL_MODIFY:   /* Job created calendar cannot be modified  */
		case LSBE_LOCK_JOB:         /* Lock the job so that it cann't be
		                               resume by sbatchd */
		case LSBE_UNSUPPORTED_MC:   /* Operation not supported for
		                               a Multicluster job */
		case LSBE_PERMISSION_MC:    /* Operation permission denied for
		                               a Multicluster job */
		case LSBE_OVER_RUSAGE:      /* Exceed q's resource reservation */
		case LSBE_CAL_USED:         /* Delete a used calendar */
		case LSBE_CAL_CYC:          /* Cyclic calednar dependence */
		case LSBE_JOB_DEP:          /* Job dependence, not deleted immed */
		case LSBE_JGRP_EXIST:       /* The job group exists */
		case LSBE_JGRP_NULL:        /* The job group doesn't exist */
		case LSBE_JGRP_HASJOB:      /* The group contains jobs */
		case LSBE_JOB_SUSP:         /* Suspended job not supported */
		case LSBE_JOB_FORW:         /* Forwarded job not suported */
		case LSBE_JOB_EXIST:        /* The job exists */
		case LSBE_JGRP_HOLD:        /* Parent group is held */
		case LSBE_MOD_JOB_NAME:     /* Cannot change job name */
			return FSD_ERRNO_DENIED_BY_DRM;

		case LSBE_NO_JOBID:         /* No jobId can be used now */
		case LSBE_NO_HPART:         /* No host partition in the system */
		case LSBE_NO_GROUP:         /* No group defined in the system */
		case LSBE_BAD_CHKLOG:       /* chklog is corrupted */
		case LSBE_NO_HOST_GROUP:        /* No host group defined in the system */
		case LSBE_NO_USER_GROUP:        /* No user group defined in the system */
		/* Error codes related to configuration lsblib call: */
		case LSBE_CONF_FATAL:        /* Fatal error in reading conf files  */
		case LSBE_CONF_WARNING:      /* Warning error in reading conf files  */
			return FSD_ERRNO_INTERNAL_ERROR;

		/* Authorization errors: */
		case LSBE_NO_USER:          /* No user defined in lsb.users file */
		case LSBE_BAD_USER:         /* Bad user name */
		case LSBE_PERMISSION:       /* User permission denied */
		case LSBE_QUEUE_USE:        /* User cannot use the queue */
		case LSBE_UGROUP_MEMBER:    /* User not in the user group */
		case LSBE_BAD_PROJECT_GROUP:    /* Invoker is not in project group */
			return FSD_ERRNO_AUTHZ_FAILURE;

		/* Error codes related to input arguments of lsblib call: */
		case LSBE_BAD_ARG:          /* Bad argument for lsblib call */
		case LSBE_BAD_TIME:         /* Bad time spec for lsblib call */
		case LSBE_BAD_LIMIT:        /* Bad CPU limit specification */
			return FSD_ERRNO_INVALID_ARGUMENT;

		case LSBE_BAD_CMD:          /* Empty job (command) */
			return FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES;

		case LSBE_START_TIME:       /* Start time is later than end time */
		case LSBE_BAD_SIGNAL:       /* Bad signal value; not supported */
		case LSBE_BAD_JOB:          /* Bad job name */
		case LSBE_QJOB_LIMIT:       /* Queue reach QJOB_LIMIT of the queue */
		case LSBE_BAD_CLUSTER:      /* Bad cluster name */
		case LSBE_BAD_RESREQ:       /* Bad resource requirement */
		case LSBE_BAD_RESOURCE:     /* Bad resource name */
		case LSBE_BAD_UGROUP:       /* Bad user group name */
		case LSBE_EXCEPT_SYNTAX:    /* Bad exception handler syntax */
		case LSBE_EXCEPT_COND:      /* Bad exception condition specification */
		case LSBE_EXCEPT_ACTION:    /* Bad or invalid action specification */
		case LSBE_BAD_HOST_SPEC:        /* bad host spec of run/cpu limits */
		/* Error codes related to calendar: */
		case LSBE_BAD_CALENDAR:     /* Bad calendar name */
		case LSBE_NOMATCH_CALENDAR: /* No calendar found */
		case LSBE_NO_CALENDAR:      /* No calendar in system */
		case LSBE_BAD_TIMEEVENT:    /* Bad calendar time events */
		case LSBE_CAL_EXIST:        /* Calendar exist already */
		case LSBE_SYNTAX_CALENDAR:  /* Calendar syntax error */
		/* Error codes related to event */
		case LSBE_BAD_EVENT:        /* Bad event name */
		case LSBE_NOMATCH_EVENT:    /* No event found */
		case LSBE_NO_EVENT:         /* No event in system */
			return FSD_ERRNO_INVALID_VALUE;

		case LSBE_UNKNOWN_EVENT:    /* Unknown event in event log file */
		case LSBE_EVENT_FORMAT:     /* bad event format in event log file */
		case LSBE_EOF:              /* End of file */

		case LSBE_MBATCHD:          /* mbatchd internal error */
		case LSBE_SBATCHD:          /* sbatchd internal error */
		case LSBE_LSBLIB:           /* lsbatch lib internal error */
		case LSBE_SYS_CALL:         /* System call fails */
		case LSBE_NO_MEM:           /* Cannot alloc memory */
		case LSBE_SERVICE:          /* lsbatch service not registered */
		case LSBE_NO_ENV:           /* LSB_SHAREDIR not defined */
		case LSBE_CHKPNT_CALL:      /* chkpnt system call fail */
		case LSBE_NO_FORK:          /* mbatchd cannot fork */
			return FSD_ERRNO_INTERNAL_ERROR;

		case LSBE_LSLIB:            /* LSLIB call fails */
			return lsfdrmaa_map_lserrno( lserrno );

		/* Error codes related to communication between mbatchd/lsblib/sbd */
		case LSBE_PROTOCOL:         /* LSBATCH protocol error */
		case LSBE_XDR:              /* XDR en/decode error */
		case LSBE_PORT:             /* No appropriate port can be bound */
		case LSBE_TIME_OUT:         /* Timeout in contacting mbatchd */
		case LSBE_CONN_TIMEOUT:     /* Timeout on connect() call */
		case LSBE_CONN_REFUSED:     /* Connection refused by server */
		case LSBE_CONN_EXIST:       /* server connection already exists */
		case LSBE_CONN_NONEXIST:    /* server is not connected */
		case LSBE_SBD_UNREACH:      /* sbd cannot be reached */
			return FSD_ERRNO_DRM_COMMUNICATION_FAILURE;

		case LSBE_OP_RETRY:         /* Operation cannot be performed right
			                             now, op. will be retried. */
			return FSD_ERRNO_TRY_LATER;

		case LSBE_INTERACTIVE_CAL:   /* Calendar not allowed for interactive job */
		case LSBE_INTERACTIVE_RERUN: /* Interactive job cannot be rerunnable */
		case LSBE_PTY_INFILE:        /* PTY and infile specified */
			return FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES;

		case LSBE_JGRP_CTRL_UNKWN:  /* The unknown group control signal */
		case LSBE_JGRP_BAD:         /* Bad Job Group name */
		case LSBE_CAL_VOID:         /* Void calendar */
		case LSBE_BAD_FRAME:        /* Bad frame expression */
		case LSBE_FRAME_BIG_IDX:    /* Frame index too long */
		case LSBE_FRAME_BAD_IDX:    /* Frame index syntax error */
		case LSBE_BAD_USER_PRIORITY: /* Bad user priority */
			return FSD_ERRNO_INVALID_VALUE;

#if 0
		case LSBE_JOB_REQUEUED:     /* Job has been killed & requeued */
		case LSBE_JOB_REQUEUE_REMOTE: /* Remote job cannot kill-requeued */
		case LSBE_NQS_NO_ARRJOB:    /* Cannot submit job array to a NQS queue */

		/* error codes for EXT_JOB_STATUS */
		case LSBE_BAD_EXT_MSGID:        /* No message available */
		case LSBE_NO_IFREG:             /* Not a regular file */
		case LSBE_BAD_ATTA_DIR:         /* MBD fail to create files in the directory*/
		case LSBE_COPY_DATA:            /* Fail to transfer data */
		case LSBE_JOB_ATTA_LIMIT:       /* exceed the limit on data transferring of a msg*/
		case LSBE_CHUNK_JOB:            /* cannot bswitch a run/wait job */

/* error code for multi-cluster: remote only queue */
case LSBE_MC_HOST:              /* cannot specify exec host */
case LSBE_MC_REPETITIVE:        /* cannot specify repetitive job */
case LSBE_MC_CHKPNT:            /* cannot be a chkpnt job */
case LSBE_MC_EXCEPTION:         /* cannot specify exception */
case LSBE_MC_TIMEEVENT:         /* cannot specify time event */
case LSBE_PROC_LESS:            /* Too few processors requested */
case LSBE_MOD_MIX_OPTS:         /* bmod pending options and running
                                   options together towards running job */
case LSBE_MOD_REMOTE:           /* cannot bmod remote running job */
case LSBE_MOD_CPULIMIT:         /* cannot bmod cpulimit without
case LSB_JOB_CPULIMIT: defined */
case LSBE_MOD_MEMLIMIT:         /* cannot bmod memlimit without
case LSB_JOB_MEMLIMIT: defined */
case LSBE_MOD_ERRFILE:          /* cannot bmod err file name */
case LSBE_LOCKED_MASTER:        /* host is locked by master LIM*/
case LSBE_WARNING_INVALID_TIME_PERIOD: /* warning time period is invalid */
case LSBE_WARNING_MISSING:      /* either warning time period or
					    warning action is not specified */
case LSBE_DEP_ARRAY_SIZE:       /* The job arrays involved in
					  * one to one dependency do not
					  *  have the same size.
					  */
case LSBE_FEWER_PROCS:          /* Not enough processors to be reserved (lsb_addreservation()) */
case LSBE_BAD_RSVID:            /* Bad reservation ID */
case LSBE_NO_RSVID:             /* No more reservation IDs can be used now */
case LSBE_NO_EXPORT_HOST:       /* No hosts are exported */
case LSBE_REMOTE_HOST_CONTROL:    /* Trying to control remote hosts*/
case LSBE_REMOTE_CLOSED:         /*Can't open a remote host closed by the remote cluster admin */
case LSBE_USER_SUSPENDED:        /* User suspended job */
case LSBE_ADMIN_SUSPENDED:       /* Admin suspended job */
case LSBE_NOT_LOCAL_HOST:        /* Not a local host name in
					   * bhost -e command
					   */
case LSBE_LEASE_INACTIVE:        /* The host's lease is not active. */
case LSBE_QUEUE_ADRSV:           /* The advance reserved host is not on queue. */
case LSBE_HOST_NOT_EXPORTED:     /* The specified host(s) is not exported. */
case LSBE_HOST_ADRSV:            /* The user specified host is not inn advance reservation */
case LSBE_MC_CONN_NONEXIST:      /* The remote cluster is not connected */
case LSBE_RL_BREAK:              /* The general resource limit broken */

		case LSBE_LSF2TP_PREEMPT: /* cannot submit a job with special topology
		                           * requirement to a preemptive queue
		                           */
		case LSBE_LSF2TP_RESERVE: /* cannot submit a job with special topology
		                           * requirement to a queue with slot reservation
		                           */
case LSBE_LSF2TP_BACKFILL: /* cannot submit a job with special
					 * topology requirement to a queue
					 * with backill
					 */
case LSBE_RSV_POLICY_NAME_BAD:     /* none existed policy name */
case LSBE_RSV_POLICY_PERMISSION_DENIED:        /* All normal user has no privilege */
case LSBE_RSV_POLICY_USER:        /* user has no privilege */
case LSBE_RSV_POLICY_HOST:       /* user has no privilege to create reservation on host */
case LSBE_RSV_POLICY_TIMEWINDOW:        /* time window is not allowed by policy */
case LSBE_RSV_POLICY_DISABLED:   /* the feature is disabled */
  /* the general limit related errors */
case LSBE_LIM_NO_GENERAL_LIMIT:  /* There are no general limit defined */
case LSBE_LIM_NO_RSRC_USAGE:  /* There are no resource usage */
case LSBE_LIM_CONVERT_ERROR:  /* Convert data error */
case LSBE_RSV_NO_HOST:        /* There are no qualified host found in cluster*/
case LSBE_MOD_JGRP_ARRAY:     /* Cannot modify job group on element of job array */
case LSBE_MOD_MIX:            /* Cannot combine modify job group or service class option with others */
case LSBE_SLA_NULL:           /* the service class doesn't exist */
case LSBE_MOD_JGRP_SLA:       /* Modify job group for job in service class is not supported*/
case LSBE_SLA_MEMBER:         /* User or user group is not a member of the specified service class */
case LSBE_NO_EXCEPTIONAL_HOST: /* There is no exceptional host found */
case LSBE_WARNING_INVALID_ACTION: /* warning action (signal) is invalid */

case LSBE_EXTSCHED_SYNTAX:    /* Extsched option syntax error */
case LSBE_SLA_RMT_ONLY_QUEUE: /* SLA doesn't work with remote only queues  */
case LSBE_MOD_SLA_ARRAY:      /* Cannot modify service class on element of job array */
case LSBE_MOD_SLA_JGRP:       /* Modify service class for job in job group is not supported*/
case LSBE_MAX_PEND:           /* Max. Pending job error */
case LSBE_CONCURRENT:         /* System concurrent query exceeded */
case LSBE_FEATURE_NULL:       /* Requested feature not enabled */

case LSBE_DYNGRP_MEMBER:      /* Host is already member of group */
case LSBE_BAD_DYN_HOST:       /* Host is not a dynamic host */
case LSBE_NO_GRP_MEMBER:      /* Host was not added with badmin hghostadd */
case LSBE_JOB_INFO_FILE:      /* Cannot create job info file */
case LSBE_MOD_OR_RUSAGE:      /* Cannot modify rusage to a new || (or) expression after the job is dispatched */
case LSBE_BAD_GROUP_NAME:     /* Bad host group name */
case LSBE_BAD_HOST_NAME:      /* Bad host name */
case LSBE_DT_BSUB:            /* Bsub is not permitted on DT cluster */

case LSBE_PARENT_SYM_JOB:     /* The parent symphony job/group was
                                     * gone when submitting jobs
                                     */
case LSBE_PARTITION_NO_CPU:   /* The partition has no cpu alllocated */
case LSBE_PARTITION_BATCH:    /* batch partition does not accept online jobs: obsolete */
case LSBE_PARTITION_ONLINE:   /* online partition does not accept batch jobs */
case LSBE_NOLICENSE_BATCH:    /* no batch licenses */
case LSBE_NOLICENSE_ONLINE:   /* no online licenses */
case LSBE_SIGNAL_SRVJOB:      /* signal is not supported for service job */
case LSBE_BEGIN_TIME_INVALID: /* the begin time is not later than current time. */
case LSBE_END_TIME_INVALID:   /* the end time is not later than current time. */
case LSBE_BAD_REG_EXPR:       /* Bad regular expression */

case LSBE_GRP_REG_EXPR:       /* Host group has regular expression */
case LSBE_GRP_HAVE_NO_MEMB:   /* Host group have no member */
case LSBE_APP_NULL:           /* the application doesn't exist */
case LSBE_PROC_JOB_APP:       /* job's proclimit rejected by App */
case LSBE_PROC_APP_QUE:       /* app's proclimit rejected by Queue */
case LSBE_BAD_APPNAME:        /* application name is too long  */
case LSBE_APP_OVER_LIMIT:     /* Over hard limit of queue */
case LSBE_REMOVE_DEF_APP:     /* Cannot remove default application */
case LSBE_EGO_DISABLED:       /* Host is disabled by EGO */
case LSBE_REMOTE_HOST:        /* Host is a remote host. Remote hosts cannot be added to a local host group. */
case LSBE_SLA_EXCLUSIVE:      /* SLA is exclusive, only accept exclusive job. */
case LSBE_SLA_NONEXCLUSIVE:   /* SLA is non-exclusive, only accept non-exclusive job */
case LSBE_PERFMON_STARTED:    /* The feature has already been started */
case LSBE_PERFMON_STOPED:     /* The Featurn has already been turn down */
case LSBE_PERFMON_PERIOD_SET: /* Current sampling period is already set to %%s,seconds. Ignored*/
case LSBE_DEFAULT_SPOOL_DIR_DISABLED: /* Default spool dir is disabled */
case LSBE_APS_QUEUE_JOB:      /* job belongs to an APS queue and cannot be moved */
case LSBE_BAD_APS_JOB:        /* job is not in an absolute priority enabled queue */
case LSBE_BAD_APS_VAL:        /* Wrong aps admin value */
case LSBE_APS_STRING_UNDEF:   /* Trying to delete a non-existent APS string */
case LSBE_SLA_JOB_APS_QUEUE:  /* A job cannot be assigned an SLA and an APS queue with factor FS */
case LSBE_MOD_MIX_APS:        /* bmod -aps | -apsn option cannot be mixed with other option */
case LSBE_APS_RANGE:          /* specified ADMIN factor/system APS value out of range */
case LSBE_APS_ZERO:          /* specified ADMIN factor/system APS value is zero */

case LSBE_DJOB_RES_PORT_UNKNOWN:         /* res port is unknown */
case LSBE_DJOB_RES_TIMEOUT:              /* timeout on res communication */
case LSBE_DJOB_RES_IOERR:                /* I/O error on remote stream */
case LSBE_DJOB_RES_INTERNAL_FAILURE:     /* res internal failure */

case LSBE_DJOB_CAN_NOT_RUN:              /* can not run outside LSF */
case LSBE_DJOB_VALIDATION_BAD_JOBID:     /* distributed job's validation failed due to incorrect job ID or index */
case LSBE_DJOB_VALIDATION_BAD_HOST:      /* distributed job's validation failed due to incorrect host selection */
case LSBE_DJOB_VALIDATION_BAD_USER:      /* distributed job's validation failed due to incorrect user */
case LSBE_DJOB_EXECUTE_TASK:             /* failed while executing tasks */
case LSBE_DJOB_WAIT_TASK:                /* failed while waiting for tasks to finish*/

case LSBE_APS_HPC:      /* HPC License not exist */
case LSBE_DIGEST_CHECK_BSUB:  /* Integrity check of bsub command failed */
case LSBE_DJOB_DISABLED:  /* Distributed Application Framework disabled */

/* Error codes related to runtime estimation and cwd */
case LSBE_BAD_RUNTIME:        /* Bad runtime specification */
case LSBE_BAD_RUNLIMIT:       /* RUNLIMIT: Cannot exceed RUNTIME*JOB_RUNLIMIT_RATIO */
case LSBE_OVER_QUEUE_LIMIT:        /* RUNTIME: Cannot exceed the hard runtime limit in the queue */
case LSBE_SET_BY_RATIO:      /* RUNLIMIT: Is not set by command line */
case LSBE_BAD_CWD:              /* current working directory name too long */

case LSBE_JGRP_LIMIT_GRTR_THAN_PARENT: /* Job group limit is greater than its parent group */
case LSBE_JGRP_LIMIT_LESS_THAN_CHILDREN: /* Job group limit is less than its children groups */
case LSBE_NO_ARRAY_END_INDEX: /* Job Array end index should be specified explicitly */
case LSBE_MOD_RUNTIME:        /* cannot bmod runtime without LSB_MOD_ALL_JOBS=y defined */

/* EP3 */
case LSBE_BAD_SUCCESS_EXIT_VALUES:
case LSBE_DUP_SUCCESS_EXIT_VALUES:
case LSBE_NO_SUCCESS_EXIT_VALUES:

case LSBE_JOB_REQUEUE_BADARG:
case LSBE_JOB_REQUEUE_DUPLICATED:
case LSBE_JOB_REQUEUE_INVALID_DIGIT: /* "all" with number */
case LSBE_JOB_REQUEUE_INVALID_TILDE: /* ~digit without "all" */
case LSBE_JOB_REQUEUE_NOVALID:

case LSBE_NO_JGRP:            /* No matching job group found */
case LSBE_NOT_CONSUMABLE:

/* AR pre/post */
case LSBE_RSV_BAD_EXEC:       /* Cannot parse an Advance Reservation -exec string */
case LSBE_RSV_EVENTTYPE:      /* Unknown AR event type */
case LSBE_RSV_SHIFT:          /* pre/post cannot have postive offset */
case LSBE_RSV_USHIFT:         /* pre-AR command cannot have offset < 0 in user-created AR */
case LSBE_RSV_NUMEVENTS:      /* only one pre- and one post- cmd permitted per AR */

/*Error codes related to AR Modification*/
case LSBE_ADRSV_ID_VALID:          /* ID does not correspond to a known AR. */
case LSBE_ADRSV_DISABLE_NONRECUR:  /* disable non-recurrent AR. */
case LSBE_ADRSV_MOD_ACTINSTANCE:   /* modification is rejected because AR is activated. */
case LSBE_ADRSV_HOST_NOTAVAIL:     /* modification is rejected because host slots is not available. */
case LSBE_ADRSV_TIME_MOD_FAIL:     /* the  time of the AR cannot be modified since resource is not available. */
case LSBE_ADRSV_R_AND_N:           /* resource requirement (-R) must be followed a slot requirment (-n) */
case LSBE_ADRSV_EMPTY:             /*modification is rejected because trying to empty the AR. */
case LSBE_ADRSV_SWITCHTYPE:        /*modification is rejected because switching AR type. */
case LSBE_ADRSV_SYS_N:             /*modification is rejected because specifying -n for system AR. */
case LSBE_ADRSV_DISABLE:           /* disable string is not valid. */
#endif


		case LSBE_ADRSV_ID_UNIQUE:         /* Unique AR ID required */
		case LSBE_BAD_RSVNAME:             /* Bad reservation name */
		case LSBE_BAD_CHKPNTDIR:           /* The checkpoint directory
		                                      is too long */
		case LSBE_JOB_REQUEUE_BADEXCLUDE:
			return FSD_ERRNO_INVALID_VALUE;

		case LSBE_ADVRSV_ACTIVESTART:      /* Cannot change the start time of an
		                                      active reservation. */
		case LSBE_ADRSV_ID_USED:           /* AR ID is refernced by a job */
		case LSBE_ADRSV_PREVDISABLED:      /* The disable period has already
		                                      been disabled */
		case LSBE_ADRSV_DISABLECURR:       /* An active period of a recurring
		                                      reservation cannot be disabled */
		case LSBE_ADRSV_NOT_RSV_HOST:      /* Modification is rejected because
		                                      specified hosts or host groups do
		                                      not belong to the reservation */
			return FSD_ERRNO_DENIED_BY_DRM;

		case LSBE_ADRSV_MOD_REMOTE:        /* Trying to modify in
		                                      a remote cluster */
		case LSBE_JOB_RUSAGE_EXCEED_LIMIT: /* Queue level limitation */
		case LSBE_APP_RUSAGE_EXCEED_LIMIT: /* Queue level limitation */
			return FSD_ERRNO_DENIED_BY_DRM;

		case LSBE_ADRSV_DISABLE_DATE:      /* Trying to disable for a date
		                                      in the past */
			return FSD_ERRNO_INVALID_VALUE;

		case LSBE_ADRSV_DETACH_MIX:        /* Cannot mix the -Un option
		                                      with others for started jobs */
		case LSBE_ADRSV_DETACH_ACTIVE:     /* Cannot detach a started job when
		                                      the reservation is active */
			return FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES;

		case LSBE_MISSING_START_END_TIME:  /* Invalid time expression:
		                                      must specify day for both
		                                      start and end time */
			return FSD_ERRNO_INVALID_ARGUMENT;

		default:
			return FSD_ERRNO_INTERNAL_ERROR;
	 }
}


int
lsfdrmaa_map_lserrno( int _lserrno )
{
	switch( _lserrno )
	 {
		case LSE_NO_ERR:
			return FSD_ERRNO_SUCCESS;

		case LSE_SIG_SYS:            /* A signal related system call failed */
		case LSE_NORCHILD:           /* no remote child */
		case LSE_MALLOC:             /* memory allocation failed */
		case LSE_LSFCONF:            /* unable to open lsf.conf */
		case LSE_BAD_ENV:            /* bad configuration environment */
		case LSE_LIM_NREG:           /* Lim is not a registered service */
		case LSE_RES_NREG:           /* Res is not a registered service */
		case LSE_BADUSER:            /* Bad user ID for REX */
		case LSE_BAD_OPCODE:         /* bad op code */
		case LSE_PROTOC_RES:         /* RES Protocol error */
		case LSE_RES_CALLBACK:       /* RES callback fails */
		case LSE_RES_NOMEM:          /* RES malloc fails */
		case LSE_RES_FATAL:          /* RES system call error */
		case LSE_RES_PTY:            /* RES cannot alloc pty */
		case LSE_RES_SOCK:           /* RES socketpair fails */
		case LSE_RES_FORK:           /* RES fork fails */
		case LSE_NOMORE_SOCK:        /* Privileged socks run out */
		case LSE_WDIR:               /* getwd() failed */
		case LSE_RES_INVCHILD:       /* No such remote child */
		case LSE_RES_KILL:           /* Remote kill permission denied */
		case LSE_PTYMODE:            /* ptymode inconsistency */
		case LSE_PROTOC_NIOS:        /* NIOS protocol error */
		case LSE_WAIT_SYS:           /* A wait system call failed */
		case LSE_RES_VERSION:        /* Incompatible versions of tty params */
		case LSE_EXECV_SYS:          /* Failed in a execv() sys call*/
		case LSE_LIMIT_SYS:          /* A resource limit sys call failed*/
		case LSE_LIM_NOMEM:          /* lim malloc failure */
		case LSE_NIO_INIT:           /* nio not initialized. */
		case LSE_CONF_SYNTAX:        /* Bad lsf.conf/lsf.sudoers syntax */
		case LSE_FILE_SYS:           /* A file operation failed */
		case LSE_CONN_SYS:           /* A connect sys call failed */
		case LSE_SELECT_SYS:         /* A select system call failed */
		case LSE_NIOS_DOWN:          /* nios has not been started. */
		case LSE_EOF:                /* Reached the end of file */
		case LSE_ACCT_FORMAT:        /* Bad lsf.acct file format */
		case LSE_FORK:               /* Unable to fork child */
		case LSE_PIPE:               /* Failed to setup pipe */
		case LSE_ESUB:               /* esub/eexec file not found */
		case LSE_DCE_EXEC:           /* dce task exec fail */
		case LSE_NO_FILE:            /* cannot open file */
		case LSE_BAD_CHAN:           /* bad communication channel */
		case LSE_INTERNAL:           /* internal library error */
		case LSE_PROTOCOL:           /* protocol error with server */
		case LSE_THRD_SYS:           /* A thread system call failed (NT only)*/
		case LSE_MISC_SYS:           /* A system call failed */
		case LSE_RES_RUSAGE:         /* Failed to get rusage from RES */
		case LSE_RES_PARENT:         /* res child Failed to contact parent */
		case LSE_SUDOERS_CONF:       /* lsf.sudoers file error */
		case LSE_SUDOERS_ROOT:       /* lsf.sudoers not owned by root */
		case LSE_I18N_SETLC:         /* i18n setlocale failed */
		case LSE_I18N_CATOPEN:       /* i18n catopen failed */
		case LSE_I18N_NOMEM:         /* i18n malloc failed */
		case LSE_NO_MEM:             /* Cannot alloc memory */
		case LSE_REGISTRY_SYS:       /* A registry system call failed (NT) */
		case LSE_FILE_CLOSE:         /* Close a NULL-FILE pointer */
		case LSE_LIMCONF_NOTREADY:   /* LIM configuration is not ready yet */
		case LSE_MASTER_LIM_DOWN:    /* for LIM_CONF master LIM down */
		case LSE_MLS_INVALID:        /* invalid MLS label */
		case LSE_MLS_CLEARANCE:      /* not enough clearance */
		case LSE_MLS_DOMINATE:       /* require label not dominate */
		case LSE_GETCONF_FAILED:     /* Failed to get configuration
		                              * information from hosts specified by
		                              * the LSF_SERVER_HOSTS parameter in
		                              * lsf.conf */
		case LSE_TSSINIT:            /* Win32: terminal service not properly
		                              * initialized */
#if 0
		case LSE_DYNM_DENIED:        /* Dynamic addHost denied */
		case LSE_EGOCONF:            /* unable to open ego.conf */
		case LSE_BAD_EGO_ENV:        /* bad configuration environment */
		case LSE_EGO_CONF_SYNTAX:    /* Bad ego.conf syntax */
		case LSE_EGO_GETCONF_FAILED: /* Failed to get configuration
		                              * information from hosts specified by
		                              * the EGO_SERVER_HOSTS parameter in
		                              * ego.conf
		                              */
		case LSE_NS_LOOKUP:          /* name service lookup failure */
		case LSE_NOT_WINHOST:        /* The host is not a Windows host */
		case LSE_NOT_MASTERCAND:     /* The host is not a master candidate host */
		case LSE_UNRESOLVALBE_HOST:  /* master Lim can not reslove the host
		                              * name of slave lim */
#endif
		default:
			return FSD_ERRNO_INTERNAL_ERROR;

		case LSE_BAD_ARGS:           /* supplied arguments invalid */
		case LSE_BAD_TASKF:          /* Bad LSF task file format */
		case LSE_LIM_BADHOST:        /* host name not recognizable by LIM */
		case LSE_LIM_BADMOD:         /* unknown host model. */
		case LSE_BAD_EXP:            /* bad resource req. expression*/
		case LSE_BAD_HOST:           /* Bad hostname */
		case LSE_SETPARAM:           /* Bad parameters for setstdin */
		case LSE_RPIDLISTLEN:        /* Insufficient list len for rpids */
		case LSE_BAD_CLUSTER:        /* Invalid cluster name */
		case LSE_RES_DIR:            /* No such directory */
		case LSE_RES_DIRW:           /* The directory may not be accessible */
		case LSE_BAD_SERVID:         /* the service ID is invalid */
		case LSE_UNKWN_RESNAME:      /* unknown resource name specified */
		case LSE_UNKWN_RESVALUE:     /* unknown resource value */
		case LSE_TASKEXIST:          /* the task already registered */
		case LSE_BAD_TID:            /* the task does not exist */
		case LSE_BAD_NAMELIST:       /* bad index name list */
		case LSE_BAD_TIME:           /* Bad time specification */
		case LSE_NO_RESOURCE:        /* no shared resource defined */
		case LSE_BAD_RESOURCE:       /* Bad resource name */
			return FSD_ERRNO_INVALID_VALUE;

		case LSE_NO_HOST:            /* No enough ok hosts found by LIM */
		case LSE_NO_ELHOST:          /* No host is found eligible by LIM */
		case LSE_LIM_IGNORE:         /* Operation ignored by LIM */
		case LSE_LIM_ALOCKED:        /* LIM already locked */
		case LSE_LIM_NLOCKED:        /* LIM was not locked. */
		case LSE_NO_LICENSE:         /* no software license for host */
		case LSE_MLS_RHOST:          /* reject by rhost.conf */
			return FSD_ERRNO_DENIED_BY_DRM;

		case LSE_RES_NOMORECONN:     /* RES is serving too many connections*/
		case LSE_TOOMANYTASK:        /* the task table is full */
		case LSE_NO_CHAN:            /* out of communication channels */
		case LSE_NO_CAL:             /* Win32: No more connections can be
		                              * made to this remote computer at this
		                              * time because there are already as
		                              * many connections as the computer can
		                              * accept. */
#ifdef LSE_LIC_OVERUSE
		case LSE_LIC_OVERUSE:        /* In license overuse status */
#endif
			return FSD_ERRNO_TRY_LATER;

		case LSE_LIM_DENIED:         /* Operation permission denied by LIM */
		case LSE_RES_ROOTSECURE:     /* Root user rejected */
		case LSE_RES_DENIED:         /* User permission denied */
#ifdef LSE_HOST_UNAUTH
		case LSE_HOST_UNAUTH:        /* Permission denied. This command must
		                              * be issued from a master, master
		                              * candidate, or local host */
#endif
			return FSD_ERRNO_AUTHZ_FAILURE;

		case LSE_NLSF_HOST:          /* request from a non lsf host */
		case LSE_EAUTH:              /* external authentication failed */
		case LSE_LOGON_FAIL:         /* Failed to logon user (NT only) */
		case LSE_NO_PASSWD:          /* no password for user */
#ifdef LSE_BAD_PASSWD
		case LSE_BAD_PASSWD:         /* User password incorrect */
#endif
#ifdef LSE_UNKWN_USER
		case LSE_UNKWN_USER:         /* User name is not in password database */
#endif
			return FSD_ERRNO_AUTH_FAILURE;

		case LSE_BAD_XDR:            /* Error during XDR */
		case LSE_MSG_SYS:            /* Failed in sending/receiving a msg */
		case LSE_MASTR_UNKNW:        /* cannot find out the master LIM*/
		case LSE_LIM_DOWN:           /* LIM does not respond */
		case LSE_PROTOC_LIM:         /* LIM protocol error */
		case LSE_SOCK_SYS:           /* A socket operation has failed */
		case LSE_ACCEPT_SYS:         /* Failed in a accept system call */
		case LSE_TIME_OUT:           /* communication timed out */
		case LSE_LOSTCON:            /* Connection has been lost */
		case LSE_NO_NETWORK:         /* Win32: The network location cannot be
		                              * reached. For information about
		                              * network troubleshooting, see Windows
		                              * Help. */
			return FSD_ERRNO_DRM_COMMUNICATION_FAILURE;
	 }
}

