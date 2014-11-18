/* $Id: job.h 2 2009-10-12 09:51:22Z mamonski $ */
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

/**
 * @file job.h
 * Job managing functions.
 */

#ifndef __DRMAA_UTILS__JOB_H
#define __DRMAA_UTILS__JOB_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <sys/time.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/thread.h>

/**
 * Create new job structure.
 * @return Sole reference to newly created job.
 */
fsd_job_t *
fsd_job_new( char *job_id );

/** Job state flags. */
typedef enum {
	/**
	 * Job is in queued state (either active or hold in queue).
	 */
	FSD_JOB_QUEUED             = 1<<0,
	/** Job is hold in queue. */
	FSD_JOB_HOLD               = 1<<1,
	/** Job is running (suspended or not). */
	FSD_JOB_RUNNING            = 1<<2,
	/**
	 * Set when job was suspended within session by
	 * drmaa_control(DRMAA_CONTROL_SUSPEND).
	 */
	FSD_JOB_SUSPENDED          = 1<<3,
	/**
	 * Whether we know that job terminated and its status
	 * is waiting to rip.
	 */
	FSD_JOB_TERMINATED         = 1<<4,
	/**
	 * It is known that job was aborted by user.
	 */
	FSD_JOB_ABORTED            = 1<<5,

	/**
	 * Job was submitted in current session.
	 * It is set for all jobs which are or was
	 * in fsd_drmaa_session_t#jobs set.
	 */
	FSD_JOB_CURRENT_SESSION    = 1<<6,

	/**
	 * Job is removed from session (but references to job still
	 * exist).  No such job shall exist in session's submited
	 * jobs set.
	 */
	FSD_JOB_DISPOSED           = 1<<7,

	FSD_JOB_MISSING            = 1<<8,

	FSD_JOB_QUEUED_MASK      = FSD_JOB_QUEUED | FSD_JOB_HOLD,
	FSD_JOB_RUNNING_MASK     = FSD_JOB_RUNNING | FSD_JOB_SUSPENDED,
	FSD_JOB_TERMINATED_MASK  = FSD_JOB_TERMINATED | FSD_JOB_ABORTED,
	FSD_JOB_STATE_MASK       = FSD_JOB_HOLD | FSD_JOB_RUNNING
		| FSD_JOB_SUSPENDED | FSD_JOB_TERMINATED
} fsd_job_flag_t;


/** Submitted job data. */
struct fsd_job_s {
	/** Release reference to job. */
	void (*release)( fsd_job_t *self );

	/** Destroy job data. */
	void (*destroy)( fsd_job_t *self );

	/** Implements drmaa_control(). */
	void (*control)( fsd_job_t *self, int action );

	/** Fetch job state from DRM. */
	void (*update_status)( fsd_job_t *self );

	/** Return job termination status and resource usage for drmaa_wait(). */
	void (*get_termination_status)( fsd_job_t *self,
			int *status, fsd_iter_t **rusage_out );

	/**
	 * Called whenever job is detected to be missing from DRM queue
	 * while stil being within DRMAA session.
	 */
	void (*on_missing)( fsd_job_t *self );

	/**
	 * Points to next job in list.
	 * Used by #fsd_job_set_t.
	 */
	fsd_job_t *next;

	/** Number of references. */
	int ref_cnt;

	/** Job identifier (as null terminated string). */
	char *job_id;

	/** DRMAA session which job was submitted in. */
	fsd_drmaa_session_t *session;

	/**
	 * Time of last update of job status and rusage information
	 * (when status, exit_status, cpu_usage, mem_usage and flags
	 * fields was updated according to DRM).
	 */
	time_t last_update_time;

	/** Job state flags.  @see job_flag_t */
	unsigned flags;

	/**
	 * State of job (as returned by drmaa_job_ps())
	 * from last retrieval from DRM.
	 */
	int state;

	/** Exit status of job as from <tt>wait(2)</tt>. */
	int exit_status;
	/** Time of job submission (local). */
	time_t submit_time;
	/** Time when job started execution (taken from DRM). */
	time_t start_time;
	/** Time when job ended execution (taken from DRM). */
	time_t end_time;
	/** CPU time usage in seconds. */
	long cpu_usage;
	/** Resident memory usage in bytes. */
	long mem_usage;
	/** Virtual memory usage in bytes. */
	long vmem_usage;
	/** Total run time in seconds. */
	long walltime;
	/** Number of execution hosts. */
	int n_execution_hosts;
	/** List of space separated execution host names. */
	char *execution_hosts;

	/** Mutex for accessing fsd_job_s structure (beside #next pointer). */
	fsd_mutex_t  mutex;
	/** Job status changed condition. */
	fsd_cond_t   status_cond;
	/** Able to destroy condition variable (ref_cnt==1). */
	fsd_cond_t   destroy_cond;

	int retry_cnt;
};



/** Create empty set of jobs. */
fsd_job_set_t *
fsd_job_set_new(void);

struct fsd_job_set_s {
	/** Destroy set of jobs (including contained job handles). */
	void (*
	destroy)( fsd_job_set_t *self );

	/** Adds job to set. */
	void (*
	add)( fsd_job_set_t *self, fsd_job_t *job );

	/** Remove job from set. */
	void (*
	remove)( fsd_job_set_t *self, fsd_job_t *job );

	/**
	 * Finds job with given job_id.
	 * @param job_set Set of jobs to search in.
	 * @param job_id Job identifier.
	 * @return If successful job handle is returned
	 * and caller have exclusive access right to it.
	 * It should be released by #release().
	 * \c NULL is returned when job was not found.
	 */
	fsd_job_t* (*
	get)( fsd_job_set_t *self, const char *job_id );

	/** Whether the set is empty. */
	bool (*
	empty)( fsd_job_set_t *self );

	/**
	 * Find any job in set which was terminated (either successfully or not).
	 * It is usefull for drmaa_wait( DRMAA_JOB_IDS_ANY ) implementation.
	 * @param job_set Set of jobs to search in.
	 * @return New reference to terminated job
	 *   or \c NULL if no such job is present in set.
	 * @see fsd_job_find
	 */
	fsd_job_t* (*
	find_terminated)( fsd_job_set_t *self );

	/**
	 * Return idenetifiers of all jobs in set.
	 * @param job_set Set of jobs.
	 * @return Vector of job idenetifiers
	 *   when done free it with fsd_free_vector.
	 */
	char** (*
	get_all_job_ids)( fsd_job_set_t *self );

	void (*
	signal_all)( fsd_job_set_t *self );

	fsd_job_t    **tab;
	size_t         tab_size;
	uint32_t       tab_mask;
	/** Number of jobs in set. */
	unsigned       n_jobs;
	/** Mutex for job set data (e.g. for adding/removing job from set). */
	fsd_mutex_t    mutex;
};

#endif /* __DRMAA_UTILS__JOB_H */
