/* $Id: thread.c 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file thread.c
 * Implementation of recursive mutexes for systems without native support
 * for it.
 */


#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/thread.h>
#include <drmaa_utils/common.h>
#include <errno.h>
#include <unistd.h>

#ifdef HAVE_GETTID
#include <sys/types.h>
#include <sys/syscall.h>
pid_t gettid(void)
{
	return (pid_t)syscall( __NR_gettid );
}
#endif

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: thread.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


void
fsd_thread_create( fsd_thread_t *thread, void* (*func)(void*), void *arg )
{
	int errno_ = 0;
	errno_ = pthread_create( thread, NULL, func, arg );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_thread_join( fsd_thread_t th, void **thread_return )
{
	int errno_ = 0;
	errno_ = pthread_join( th, thread_return );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_thread_detach( fsd_thread_t th )
{
	int errno_ = 0;
	errno_ = pthread_detach( th );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}


#if HAVE_RECURSIVE_MUTEXES

void
fsd_mutex_init( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	pthread_mutexattr_t attr;
	do {
		errno_ = pthread_mutexattr_init( &attr );
		if( errno_ )  break;
		errno_ = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
		if( errno_ )  break;
		errno_ = pthread_mutex_init( mutex, &attr );
		if( errno_ )  break;
		errno_ = pthread_mutexattr_destroy( &attr );
	} while( false );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_mutex_destroy( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	errno_ = pthread_mutex_destroy( mutex );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

bool
fsd_mutex_lock( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	errno_ = pthread_mutex_lock( mutex );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
	return true;
}

bool
fsd_mutex_unlock( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	errno_ = pthread_mutex_unlock( mutex );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
	return false;
}

bool
fsd_mutex_trylock( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	errno_ = pthread_mutex_trylock( mutex );
	switch( errno_ )
	 {
		case 0:
			return true;
		case EBUSY:
			return false;
		default:
			fsd_exc_raise_sys( errno_ );
	 }
}

int
fsd_mutex_unlock_times( fsd_mutex_t *mutex )
{
	int count = 0;
	int errno_ = 0;
	while( errno_ == 0 )
	 {
		errno_ = pthread_mutex_unlock( mutex );
		if( !errno_ )
			count++;
	 }
	if( errno_ != EPERM  ||  count == 0 )
		fsd_exc_raise_sys( errno_ );
	return count;
}

void
fsd_cond_init( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_init( cond, NULL );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_destroy( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_destroy( cond );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_signal( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_signal( cond );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_broadcast( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_broadcast( cond );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_wait( fsd_cond_t *cond, fsd_mutex_t *mutex )
{
	int errno_ = 0;
	errno_ = pthread_cond_wait( cond, mutex );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

bool
fsd_cond_timedwait( fsd_cond_t *cond, fsd_mutex_t *mutex,
		const struct timespec *abstime )
{
	int errno_ = 0;
	errno_ = pthread_cond_timedwait( cond, mutex, abstime );
	switch( errno_ )
	 {
		case 0:
			return true;
		case ETIMEDOUT:
			return false;
		default:
			fsd_exc_raise_sys( errno_ );
	 }
}

#else /* ! HAVE_RECURSIVE_MUTEXES */

void
fsd_mutex_init( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	mutex->owner = (pthread_t)-1;
	mutex->acquired = 0;
	errno_ = pthread_mutex_init( &mutex->mutex, NULL );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_mutex_destroy( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	errno_ = pthread_mutex_destroy( &mutex->mutex );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

bool
fsd_mutex_lock( fsd_mutex_t *mutex )
{
	/* Note: The order of checks is significant. */
	if( mutex->acquired  &&  pthread_equal( mutex->owner, pthread_self() ) )
		mutex->acquired ++;
	else
	 {
		int errno_ = 0;
		errno_ = pthread_mutex_lock( &mutex->mutex );
		if( errno_ == 0 )
		 {
			mutex->owner    = pthread_self();
			mutex->acquired = 1;
		 }
		else
			fsd_exc_raise_sys( errno_ );
	 }
	return true;
}

bool
fsd_mutex_unlock( fsd_mutex_t *mutex )
{
	fsd_assert( mutex->acquired
			&&  pthread_equal( mutex->owner, pthread_self() ) );
	if( -- (mutex->acquired) == 0 )
	 {
		int errno_ = 0;
		errno_ = pthread_mutex_unlock( &mutex->mutex );
		if( errno_ )
			fsd_exc_raise_sys( errno_ );
	 }
	return false;
}

bool
fsd_mutex_trylock( fsd_mutex_t *mutex )
{
	if( mutex->acquired  &&  pthread_equal( mutex->owner, pthread_self() ) )
	 {
		mutex->acquired ++;
		return true;
	 }
	else
	 {
		int errno_ = 0;
		errno_ = pthread_mutex_trylock( &mutex->mutex );
		switch( errno_ )
		 {
			case 0:
				mutex->owner    = pthread_self();
				mutex->acquired = 1;
				return true;
			case ETIMEDOUT:
				return false;
			default:
				fsd_exc_raise_sys( errno_ );
		 }
	 }
}

int
fsd_mutex_unlock_times( fsd_mutex_t *mutex )
{
	int errno_ = 0;
	int count = 0;
	fsd_assert( mutex->acquired
			&&  pthread_equal( mutex->owner, pthread_self() ) );
	count = mutex->acquired;
	mutex->acquired = 0;
	errno_ = pthread_mutex_unlock( &mutex->mutex );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
	return count;
}

void
fsd_cond_init( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_init( cond, NULL );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_destroy( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_destroy( cond );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_signal( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_signal( cond );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_broadcast( fsd_cond_t *cond )
{
	int errno_ = 0;
	errno_ = pthread_cond_broadcast( cond );
	if( errno_ )
		fsd_exc_raise_sys( errno_ );
}

void
fsd_cond_wait( fsd_cond_t *cond, fsd_mutex_t *mutex )
{
	int errno_ = 0;
	int acquired_save = mutex->acquired;
	fsd_assert( mutex->acquired
			&&  pthread_equal( mutex->owner, pthread_self() ) );
	errno_ = pthread_cond_wait( cond, &mutex->mutex );
	if( errno_ == 0 )
	 {
		mutex->owner = pthread_self();
		mutex->acquired = acquired_save;
	 }
	else
		fsd_exc_raise_sys( errno_ );
}

bool
fsd_cond_timedwait( fsd_cond_t *cond, fsd_mutex_t *mutex,
		const struct timespec *abstime )
{
	int errno_ = 0;
	int acquired_save = mutex->acquired;
	fsd_assert( mutex->acquired
			&&  pthread_equal( mutex->owner, pthread_self() ) );
	errno_ = pthread_cond_timedwait( cond, &mutex->mutex, abstime );
	switch( errno_ )
	 {
		case 0:
			mutex->owner = pthread_self();
			mutex->acquired = acquired_save;
			return true;
		case ETIMEDOUT:
			mutex->owner = pthread_self();
			mutex->acquired = acquired_save;
			return false;
		default:
			fsd_exc_raise_sys( errno_ );
	 }
}

#endif /* ! HAVE_RECURSIVE_MUTEXES */



int
fsd_thread_id(void)
{
#if HAVE_GETTID
	/*
	 * On Linux 2.6 (with NPTL) getpid() returns
	 * same value for all threads in single process.
	 */
	return (int)gettid();
#else
	return (int)getpid();
#endif
}

