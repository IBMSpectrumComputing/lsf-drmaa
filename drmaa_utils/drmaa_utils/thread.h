/* $Id: thread.h 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file thread.h
 *
 * Thread and synchronization primitives.
 */

#ifndef __DRMAA_UTILS__THREAD_H
#define __DRMAA_UTILS__THREAD_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <pthread.h>
#include <time.h>

#include <drmaa_utils/compat.h>

/**
 * @defgroup recursive_mutex  Recursive mutexes implementation.
 * It uses recursive mutexes if supplied by POSIX threads library
 * or implements it over plain mutexes.
 */
/* @{ */

#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
#	define HAVE_RECURSIVE_MUTEXES 1
#else
#	define HAVE_RECURSIVE_MUTEXES 0
#endif /* ! PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP */


#if HAVE_RECURSIVE_MUTEXES

	typedef pthread_mutex_t fsd_mutex_t;
#	define FSD_MUTEX_INITIALIZER  PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
	typedef pthread_cond_t  fsd_cond_t;
#	define FSD_COND_INITIALIZER  PTHREAD_COND_INITAILIZER

#else /* !HAVE_RECURSIVE_MUTEXES */

	/**
	 * Recursive mutex build on top of non-recursive mutex
	 * (used when recursive mutexes are not provided by POSIX
	 * thread library).
	 */
	typedef struct fsd_mutex_s {
		pthread_mutex_t mutex; /**< Non-recursive mutex. */
		pthread_t       owner; /**< Thread which owns critical section. */
		int             acquired; /**< How many times
			owning thread acquired this mutex. */
	} fsd_mutex_t;
#	define FSD_MUTEX_INITIALIZER  { PTHREAD_MUTEX_INITIALIZER, (pthread_t)-1, 0 }
	typedef pthread_cond_t  fsd_cond_t;
#	define FSD_COND_INITIALIZER  PTHREAD_COND_INITAILIZER

#endif /* ! HAVE_RECURSIVE_MUTEXES */


/** pthread_mutex_init wrapper.  Initializes recursive mutex. */
void fsd_mutex_init     ( fsd_mutex_t *mutex );
void fsd_mutex_destroy  ( fsd_mutex_t *mutex );
bool fsd_mutex_lock     ( fsd_mutex_t *mutex );
bool fsd_mutex_unlock   ( fsd_mutex_t *mutex );
bool fsd_mutex_trylock  ( fsd_mutex_t *mutex );

/**
 * Try to unlock mutex as many times as possible
 * returning the count of locks previously granted
 * to current thread.
 */
int fsd_mutex_unlock_times( fsd_mutex_t *mutex );

void fsd_cond_init      ( fsd_cond_t *cond );
void fsd_cond_destroy   ( fsd_cond_t *cond );
void fsd_cond_signal    ( fsd_cond_t *cond );
void fsd_cond_broadcast ( fsd_cond_t *cond );
void fsd_cond_wait      ( fsd_cond_t *cond, fsd_mutex_t *mutex );
bool fsd_cond_timedwait ( fsd_cond_t *cond, fsd_mutex_t *mutex,
		const struct timespec *abstime );
/* @} */


/**
 * @defgroup thread  Wrapper around POSIX thread functions.
 */
/* @{ */
typedef pthread_t fsd_thread_t;
/** pthread_create wrapper */
void fsd_thread_create( fsd_thread_t *thread, void* (*func)(void*), void *arg );
#define fsd_thread_self    pthraed_self
#define fsd_thraed_equal   pthread_equal
#define fsd_thread_exit    pthread_exit
/** pthread_join wrapper */
void fsd_thread_join( fsd_thread_t th, void **thread_return );
/** pthread_detach wrapper */
void fsd_thread_detach( fsd_thread_t th );
/* void fsd_thread_cancel( fsd_thread_t th ); */
/* @} */


/** Returns thread identifier. */
int fsd_thread_id(void);

#endif /* __DRMAA_UTILS__THREAD_H */

