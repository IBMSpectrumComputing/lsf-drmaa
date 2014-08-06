/* $Id: session.h 2 2009-10-12 09:51:22Z mamonski $ */
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

/**
 * @file session.h
 * DRMAA session.
 */

#ifndef __DRMAA_UTILS__SESSION_H
#define __DRMAA_UTILS__SESSION_H

#include <sys/time.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/thread.h>

/** Creates new DRMAA session. */
fsd_drmaa_session_t *
fsd_drmaa_session_new( const char *contact );

/**
 * Get current (global) DRMAA session
 * and increase it's reference count.
 */
fsd_drmaa_session_t *
fsd_drmaa_session_get(void);

/**
 * An action to be taken when job is missing from queue.
 */
typedef enum {
	/**
	 * Always raise #FSD_ERRNO_INTERNAL_ERROR (one per job)
	 * in drmaa_wait() or drmaa_synchronize()
	 * when job disappearance is spotted.
	 */
	FSD_REVEAL_MISSING_JOBS,
	/**
	 * Treat missing jobs as terminated with status
	 * depending on last known state and action (drmaa_control()).
	 */
	FSD_IGNORE_MISSING_JOBS,
	/**
	 * Treat missing jobs as terminated
	 * but only before they enter into running state.
	 */
	FSD_IGNORE_QUEUED_MISSING_JOBS
} fsd_missing_jobs_behaviour_t;


/** DRMAA session data. */
struct fsd_drmaa_session_s {
	/** Release one reference to DRMAA session. */
	void (*
	release)( fsd_drmaa_session_t *self );

	/**
	 * Destroy session after ensuring no other references to it are held.
	 *
	 * May block until reference count decreases to 1.
	 * May return before actual session destruction if another thread
	 * have already taken responsibility for freeing session.
	 */
	void (*
	destroy)(
			fsd_drmaa_session_t *self );

	/**
	 * Destroy session without waiting for releasing references.
	 */
	void (*
	destroy_nowait)( fsd_drmaa_session_t *self );


	/** Implements drmaa_run_job(). */
	char* (*
	run_job)(
			fsd_drmaa_session_t *self,
			const fsd_template_t *jt
			);

	/** Implements drmaa_run_bulk_jobs(). */
	fsd_iter_t* (*
	run_bulk)(
			fsd_drmaa_session_t *self,
			const fsd_template_t *jt,
			int start, int end, int incr
			);

	/** Implements drmaa_control(). */
	void (*
	control_job)(
			fsd_drmaa_session_t *self,
			const char *job_id, int action
			);

	/** Implements drmaa_job_ps(). */
	void (*
	job_ps)(
			fsd_drmaa_session_t *self,
			const char *job_id, int *remote_ps
			);

	/** Implements drmaa_synchronize(). */
	void (*
	synchronize)(
			fsd_drmaa_session_t *self,
			const char **input_job_ids, const struct timespec *timeout,
			bool dispose
			);

	/**
	 * Implements drmaa_wait() call.
	 * @param self     DRMAA session.
	 * @param job_id   Job identifer to wait for or #DRMAA_JOB_IDS_SESSION_ANY
	 * @param timeout  Timeout after which call shall raise
	 *   #FSD_DRMAA_ERRNO_EXIT_TIMEOUT if job not terminated or \c NULL.
	 * @param status   Terminated job status (output only).
	 * @param rusage   Job resource usage (output only).
	 * @see drmaa_wait()
	 * @see #wait_for_single_job  #wait_for_any_job
	 */
	char* (*
	wait)(
			fsd_drmaa_session_t *self,
			const char *job_id, const struct timespec *timeout,
			int *status, fsd_iter_t **rusage
			);

	/**
	 * Construct new job object.
	 * @return Reference to newly created job.
	 */
	fsd_job_t* (*
	new_job)( fsd_drmaa_session_t *self, const char *job_id );

	char* (*
	run_impl)(
			fsd_drmaa_session_t *self,
			const fsd_template_t *jt, int bulk_incr
			);

	/**
	 * Wait until given job terminates (either successfuly or not).
	 * @param self     DRMAA session.
	 * @param job_id   Identifier of job to wait for.
	 * @param timeout  If not \c NULL and job does not terminate
	 *   in given amount of time function returns and
	 *   #FSD_DRMAA_ERRNO_EXIT_TIMEOUT is raised.
	 * @param status   If not \c NULL job status code is stored here.
	 * @param rusage   If not \c NULL list of used resources is returned.
	 * @param dispose  If \c true job information is removed from session
	 *   at the end of call and further accesses to this job_id will
	 *   raise #FSD_DRMAA_ERRNO_INVALID_JOB.  Otherwise job data is held.
	 * @see #wait_for_any_job #wait
	 */
	void (*
	wait_for_single_job)(
			fsd_drmaa_session_t *self,
			const char *job_id, const struct timespec *timeout,
			int *status, fsd_iter_t **rusage, bool dispose
			);

	/**
	 * Wait until and job left in session terminates.
	 * @return  Identifier of waited job.
	 *   Freeing responsobility is left to the callee.
	 * @see #wait_for_single_job #wait
	 */
	char* (*
	wait_for_any_job)(
			fsd_drmaa_session_t *self,
			const struct timespec *timeout,
			int *status, fsd_iter_t **rusage,
			bool dispose
			);

	void (*
	wait_for_job_status_change)(
			fsd_drmaa_session_t *self,
			fsd_cond_t *wait_condition,
			fsd_mutex_t *mutex,
			const struct timespec *timeout
			);

	void* (*
	wait_thread)( fsd_drmaa_session_t *self );

	void (*
	stop_wait_thread)( fsd_drmaa_session_t *self );

	/**
	 * Make status of all jobs held in session up to date.
	 */
	void (*
	update_all_jobs_status)( fsd_drmaa_session_t *self );

	/** Return list of all jobs within session. */
	char** (*
	get_submited_job_ids)(
			fsd_drmaa_session_t *self
			);

	/**
	 * Get job with given identifier.
	 * @return New reference to job or \c NULL
	 *   if no job found.
	 */
	fsd_job_t* (*
	get_job)(
			fsd_drmaa_session_t *self, const char *job_id
			);

	/**
	 * Tries to load configuration from following locations in order
	 * (configuration settings from later locations override
	 * those from earlier):
	 * /etc/basename.conf, ~/.basename.conf, $basename_CONF.
	 */
	void (*
	load_configuration)(
			fsd_drmaa_session_t *self, const char *basename
			);

	void (*
	read_configuration)(
			fsd_drmaa_session_t *self,
			const char *filename, bool must_exist,
			const char *configuration, size_t config_len
			);

	void (*
	apply_configuration)(
			fsd_drmaa_session_t *self
			);

	/** Reference counter. */
	int ref_cnt;

	/**
	 * Whether #destroy was called and waits for releasing
	 * references.  If set pending #wait calls shall
	 * return with #FSD_DRMAA_ERRNO_NO_ACTIVE_SESSION error.
	 */
	bool destroy_requested;

	/**
	 * Copy of contact string passed to drmaa_init()
	 * or default contact if drmaa_init() was called with \c NULL.
	 */
	char *contact;

	/**
	 * Jobs submitted within this DRMAA session
	 * and not disposed yet.
	 */
	fsd_job_set_t *jobs;

	/** DRMAA configuration. */
	fsd_conf_dict_t *configuration;

	/** Queue pooling delay (time delta). */
	struct timespec pool_delay;

	/**
	 * Cache job state for number of seconds.
	 * If positive drmaa_job_ps() returns remembered state without
	 * communicating with DRM for \a cache_job_state seconds since last
	 * update.
	 */
	int cache_job_state;

	/** Whether to wait for jobs in separate thread. */
	bool enable_wait_thread;

	/**
	 * Configuration dictionary which maps
	 * job categories to native specification.
	 */
	fsd_conf_dict_t *job_categories;

	/**
	 * How to behave when submitted job disappears from DRM queue.
	 */
	fsd_missing_jobs_behaviour_t missing_jobs;

	fsd_mutex_t mutex; /**< Mutex for accessing session data. */
	fsd_cond_t wait_condition;  /**< Conditional for drmaa_wait() */
	fsd_cond_t destroy_condition;  /**< Conditional for ref_cnt==1 */

	/**
	 * Mutex for accessing connection to DRM.
	 *
	 * To prevent deadlocks #mutex should be acquired first
	 * when both session data and DRM connection are needed.
	 */
	fsd_mutex_t drm_connection_mutex;

	fsd_thread_t wait_thread_handle;
	bool wait_thread_started;
	bool wait_thread_run_flag;
};


typedef struct fsd_drmaa_ctx_s {
	fsd_drmaa_session_t *session;
	fsd_mutex_t session_mutex;
	
} fsd_drmaa_ctx_t;


extern fsd_drmaa_ctx_t fsd_drmaa_ctx;

#endif /* __DRMAA_UTILS__SESSION_H */

