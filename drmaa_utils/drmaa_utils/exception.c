/* $Id: exception.c 183 2010-05-13 16:47:45Z mamonski $ */
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

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/exception.h>


#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: exception.c 183 2010-05-13 16:47:45Z mamonski $";
#endif


int fsd_exc_code( const fsd_exc_t *self );
const char *fsd_exc_message( const fsd_exc_t *self );
void fsd_exc_destroy( fsd_exc_t *self );

#define EXC_INITIALIZER( code, message ) \
 { \
	fsd_exc_code, fsd_exc_message, fsd_exc_destroy, \
	code, message, false, false \
 }

static const fsd_exc_t no_memory_exception \
	= EXC_INITIALIZER( FSD_ERRNO_NO_MEMORY, "Not enough memory." );


/**
 * Thread specific stack of restore points
 * @see fsd_exc_try_block_t
 */
typedef struct fsd_exc_stack_s {
	fsd_exc_try_block_t **restore_points;
	int n_restore_points;
} fsd_exc_stack_t;


static pthread_key_t fsd_exc_stack;
static pthread_once_t fsd_exc_init_once = PTHREAD_ONCE_INIT;


static void
fsd_exc_stack_destroy( fsd_exc_stack_t *stack );


void
fsd_exc_init(void)
{
	int rc;
	fsd_log_enter((""));
	rc = pthread_key_create(
			&fsd_exc_stack,
			(void (*)(void*))fsd_exc_stack_destroy
			);
	if( rc )
	 {
		fsd_log_fatal(( "pthread_key_create: %m" ));
		abort();
	 }
	fsd_log_return((""));
}


fsd_exc_stack_t *
fsd_exc_get_stack( bool create )
{
	fsd_exc_stack_t *stack = NULL;
	int rc;

	rc = pthread_once( &fsd_exc_init_once, fsd_exc_init );
	if( rc )
	 {
		fsd_log_fatal(( "pthread_once: %m" ));
		abort();
	 }
	stack = (fsd_exc_stack_t*)pthread_getspecific( fsd_exc_stack );
	if( stack == NULL  &&  create )
	 {
		rc = fsd_malloc_noraise( stack, fsd_exc_stack_t );
		if( rc )  return NULL;
		stack->restore_points = NULL;
		stack->n_restore_points = 0;
		rc = pthread_setspecific( fsd_exc_stack, stack );
		if( rc  &&  errno != ENOMEM )
		 {
			fsd_log_fatal(( "pthread_setspecific: %m" ));
			abort();
		 }
		else if( rc )
		 {
			fsd_exc_stack_destroy( stack );
		 }
	 }
	else
		fsd_assert( stack != NULL );
	return stack;
}


void
fsd_exc_stack_destroy( fsd_exc_stack_t *stack )
{
	int i;
	fsd_log_enter((""));
	for( i = 0;  i < stack->n_restore_points;  i++ )
	 {
		fsd_exc_try_block_t *b = stack->restore_points[i];
		if( b->handled_exc != NULL )
			b->handled_exc->destroy( b->handled_exc );
		fsd_free( b );
	 }
	fsd_free( stack->restore_points );
	fsd_free( stack );
	fsd_log_return((""));
}


fsd_exc_try_block_t *
fsd_exc_try( const char *function, int lineno )
{
	fsd_exc_stack_t *stack = NULL;
	fsd_exc_try_block_t *p = NULL;
	int rc;

	/* fsd_log_enter(( "(%s, %d)", function, lineno )); */
	stack = fsd_exc_get_stack( true );
	if( stack == NULL )
		return NULL;

	rc = fsd_realloc_noraise(
			stack->restore_points, stack->n_restore_points+1,
			fsd_exc_try_block_t* );
	if( rc )
		return NULL;
	rc = fsd_malloc_noraise( p, fsd_exc_try_block_t );
	if( rc )
		return NULL;
	p->handled_exc = NULL;
	p->state = FSD_EXC_ENTER;
	p->function = function;
	p->lineno = lineno;
	stack->restore_points[ stack->n_restore_points++ ] = p;
	/* fsd_log_return(( " =%p", (void*)p )); */
	return p;
}


void
fsd_exc_control( fsd_exc_try_block_t *block, int *rc )
{
	/* fsd_log_enter(( "(block=%p, rc=%d)", (void*)block, *rc )); */
	if( block == NULL  ||  *rc == FSD_ERRNO_EXC_END )
		return;

	switch( block->state )
	 {
		case FSD_EXC_ENTER:
			block->state = FSD_EXC_TRY_BLOCK;
			fsd_assert( *rc == 0 );
			/* fsd_log_return(( ": rc=%d => %s:%d",
						*rc, block->function, block->lineno )); */
			return;

		case FSD_EXC_TRY_BLOCK:
			if( *rc == 0 )
			 {
				block->state = FSD_EXC_ELSE_BLOCK;
				/* fsd_log_return(( ": rc=FSD_ERRNO_EXC_ELSE => %s:%d",
							block->function, block->lineno )); */
				*rc = FSD_ERRNO_EXC_ELSE;
				return;
			 }
			else
			 {
				fsd_assert( *rc > 0 );
				block->state = FSD_EXC_EXCEPTION_HANDLE;
				/* fsd_log_return(( ": rc=%d => %s:%d",
							*rc, block->function, block->lineno )); */
				return;
			 }

		case FSD_EXC_EXCEPTION_HANDLE:
		case FSD_EXC_ELSE_BLOCK:
			block->state = FSD_EXC_FINALLY_BLOCK;
			/* fsd_log_return(( ": rc=FSD_ERRNO_EXC_FINALLY => %s:%d",
						block->function, block->lineno )); */
			*rc = FSD_ERRNO_EXC_FINALLY;
			return;

		case FSD_EXC_FINALLY_BLOCK:
		 {
			fsd_exc_try_block_t *current = NULL;
			fsd_exc_try_block_t *upper = NULL;
			fsd_exc_stack_t *stack = NULL;

			block->state = FSD_EXC_LEAVE;

			stack = fsd_exc_get_stack( false );
			current = stack->restore_points[ stack->n_restore_points-1 ];
			fsd_assert( block == current );
			if( stack->n_restore_points > 1 )
				upper = stack->restore_points[ stack->n_restore_points-2 ];
			stack->n_restore_points --;

			if( current->handled_exc  &&  upper )
			 {
				if( upper->handled_exc )
				 {
					fsd_assert( upper->state == FSD_EXC_EXCEPTION_HANDLE
							|| upper->state == FSD_EXC_FINALLY_BLOCK );
					if( upper->state == FSD_EXC_FINALLY_BLOCK )
						fsd_log_warning((
									"overriding previously raised exception: <%d:%s>",
									upper->handled_exc->_code, upper->handled_exc->_message ));
					upper->handled_exc->destroy( upper->handled_exc );
				 }
				upper->handled_exc = current->handled_exc;
				/* current->handled_exc = NULL; */
				fsd_free_noraise( current );
				/* fsd_log_return(( ": longjmp(..., %d) => %s:%d",
							upper->handled_exc->_code, upper->function, upper->lineno )); */
				longjmp( upper->env, upper->handled_exc->_code );
			 }
			else
			 {
				if( current->handled_exc )
					current->handled_exc->destroy( current->handled_exc );
				fsd_free_noraise( current );
				/* fsd_log_return(( ": rc=FSD_ERRNO_EXC_END => %s:%d",
							block->function, block->lineno )); */
				*rc = FSD_ERRNO_EXC_END;
				return;
			 }
		 }

		default:
			fsd_assert(false);
	 }

	/* no return */
}


const fsd_exc_t *
fsd_exc_get(void)
{
	fsd_exc_stack_t *stack;
	fsd_exc_try_block_t *block;
	stack = fsd_exc_get_stack( false );
	fsd_assert( stack->n_restore_points > 0 );
	block = stack->restore_points[ stack->n_restore_points-1 ];
	return block->handled_exc;
}


void
fsd_exc_clear(void)
{
	fsd_exc_stack_t *stack;
	fsd_exc_try_block_t *block;
	fsd_log_enter((""));
	stack = fsd_exc_get_stack( false );
	fsd_assert( stack->n_restore_points > 0 );
	block = stack->restore_points[ stack->n_restore_points-1 ];
	if( block->handled_exc )
		block->handled_exc->destroy( block->handled_exc );
	block->handled_exc = NULL;
	fsd_log_return((""));
}


fsd_exc_t *
fsd_exc_new( int code, char *message, bool own_message )
{
	fsd_exc_t *exc = NULL;
	char *volatile message_buffer = NULL;

	fsd_log_error(("fsd_exc_new(%d,%s,%d)", code, message, own_message));

	TRY
	 {
		if( own_message )
			message_buffer = message;
		fsd_malloc( exc, fsd_exc_t );
		exc->_code = code;
		exc->_message = message;
		exc->_own_message = own_message;
		exc->_own_self = true;
		exc->code = fsd_exc_code;
		exc->message = fsd_exc_message;
		exc->destroy = fsd_exc_destroy;
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( message_buffer );
		fsd_exc_reraise();
	 }
	END_TRY
	return exc;
}

int
fsd_exc_code( const fsd_exc_t *self )
{
	return self->_code;
}

const char *
fsd_exc_message( const fsd_exc_t *self )
{
	return self->_message;
}

void
fsd_exc_destroy( fsd_exc_t *self )
{
	if( self->_own_message )
		fsd_free_noraise( self->_message );
	if( self->_own_self )
		fsd_free_noraise( self );
}


void
fsd_exc_raise( fsd_exc_t *exc )
{
	fsd_exc_stack_t *stack = NULL;
	fsd_exc_try_block_t *block = NULL;
	fsd_assert(( exc->_code > 0 ));
	stack = fsd_exc_get_stack( false );
	fsd_assert(( stack->n_restore_points > 0 ));
	block = stack->restore_points[ stack->n_restore_points - 1 ];
	if( block->handled_exc )
		block->handled_exc->destroy( block->handled_exc );
	block->handled_exc = exc;
	longjmp( block->env, exc->_code );
}


void
fsd_exc_raise_code( int error_code )
{
	fsd_exc_t *exc;
	if( error_code == FSD_ERRNO_NO_MEMORY )
		exc = (fsd_exc_t*)&no_memory_exception;
	else
		exc = fsd_exc_new( error_code, (char*)fsd_strerror(error_code), false );
	fsd_exc_raise( exc );
}

void
fsd_exc_raise_msg( int error_code, const char *message )
{
	fsd_exc_raise(
			fsd_exc_new( error_code, fsd_strdup(message), true ) );
}

void
fsd_exc_raise_fmt( int error_code, const char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	fsd_exc_raise_fmtv( error_code, fmt, args );
	va_end( args );
}

void
fsd_exc_raise_fmtv( int error_code, const char *fmt, va_list args )
{
	fsd_exc_t *exc = NULL;
	char *volatile message = NULL;
	TRY
	 {
		message = fsd_vasprintf( fmt, args );
		exc = fsd_exc_new( error_code, message, true );
		message = NULL;
	 }
	FINALLY
	 {
		fsd_free( message );
	 }
	END_TRY
	fsd_exc_raise( exc );
}

void
fsd_exc_raise_sys( int errno_code )
{
	fsd_exc_t *exc = NULL;

	if( errno_code == 0 )
		errno_code = errno;
	if( errno_code == ENOMEM )
		exc = (fsd_exc_t*)&no_memory_exception;
	else
	 {
		int code;
		char* volatile message = NULL;
		volatile bool own_message = false;
		TRY
		 {
			switch( errno_code )
			 {
				case ETIMEDOUT:  code = FSD_ERRNO_TIMEOUT;  break;
				default:         code = FSD_ERRNO_INTERNAL_ERROR;  break;
			 }
			message = (char*)fsd_astrerror( errno_code, (bool*)&own_message );
			exc = fsd_exc_new( code, message, own_message );
		 }
		EXCEPT_DEFAULT
		 {
			if( message && own_message )
				fsd_free( message );
			fsd_exc_reraise();
		 }
		END_TRY
	 }
	fsd_exc_raise( exc );
}


void
fsd_exc_reraise(void)
{
	fsd_exc_stack_t *stack = NULL;
	fsd_exc_try_block_t *block = NULL;
	stack = fsd_exc_get_stack( false );
	fsd_assert(( stack->n_restore_points > 0 ));
	block = stack->restore_points[ stack->n_restore_points - 1 ];
	fsd_assert(( block->handled_exc->_code > 0 ));
	longjmp( block->env, block->handled_exc->_code );
}


const char *
fsd_strerror( int error_code )
{
	switch( error_code )
	 {
		case FSD_ERRNO_SUCCESS:
			return "Success.";
		case FSD_ERRNO_INTERNAL_ERROR:
			return "Unexpected or internal error.";
		case FSD_ERRNO_NO_MEMORY:
			return "Not enough memory.";
		case FSD_ERRNO_INVALID_ARGUMENT:
			return "Invalid argument value.";
		case FSD_ERRNO_INVALID_VALUE:
			return "Invalid value.";
		case FSD_ERRNO_INVALID_VALUE_FORMAT:
			return "Invalid value format.";
		case FSD_ERRNO_STOP_ITERATION:
			return "Vector have no more elements.";
		case FSD_ERRNO_NOT_IMPLEMENTED:
			return "Functionality is not implemented.";
		case FSD_ERRNO_NOT_INITIALIZED:
			return "Library is not initialized";
		case FSD_ERRNO_TIMEOUT:
			return "Routine returned due to time-out.";
		case FSD_ERRNO_AUTH_FAILURE:
			return "Authentication failure.";
		case FSD_ERRNO_AUTHZ_FAILURE:
			return "Authorization failure";
		case FSD_ERRNO_TRY_LATER:
			return "System is overloaded.  Try again later.";
		case FSD_ERRNO_DRM_COMMUNICATION_FAILURE:
			return "Could not contact DRM system for this request.";
		case FSD_ERRNO_DRMS_INIT_FAILED:
			return "Unable to initialize DRM system.";
		case FSD_ERRNO_DRMS_EXIT_ERROR:
			return "Disengagement from the DRM system failed.";
		case FSD_ERRNO_DENIED_BY_DRM:
			return "DRM rejected request due to its configuration "
				"or request attributes.";

		case FSD_DRMAA_ERRNO_NO_ACTIVE_SESSION:
			return "No active DRMAA session.";
		case FSD_DRMAA_ERRNO_INVALID_CONTACT_STRING:
			return "Invalid contact string.";
		case FSD_DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR:
			return "Can not determine default contact to DRM system.";
		case FSD_DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED:
			return "Contact to DRM must be set explicitly "
				"because there is no default.";
		case FSD_DRMAA_ERRNO_ALREADY_ACTIVE_SESSION:
			return "DRMAA session already exist.";
		case FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT:
			return "Invalid format of job attribute.";
		case FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE:
			return "Invalid value of job attribute.";
		case FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES:
			return "Value of attribute conflicts with other attribute value.";
		case FSD_DRMAA_ERRNO_INVALID_JOB:
			return "Job does not exist in DRMs queue.";
		case FSD_DRMAA_ERRNO_RESUME_INCONSISTENT_STATE:
			return "Can not resume job (not in valid state).";
		case FSD_DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE:
			return "Can not suspend job (not in valid state).";
		case FSD_DRMAA_ERRNO_HOLD_INCONSISTENT_STATE:
			return "Can not hold job (not in valid state).";
		case FSD_DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE:
			return "Can not release job (not in valid state).";
		case FSD_DRMAA_ERRNO_EXIT_TIMEOUT:
			return "Waiting for job to terminate finished due to time-out.";
		case FSD_DRMAA_ERRNO_NO_RUSAGE:
			return "Job finished but resource usage information "
				"and/or termination status could not be provided.";

		case FSD_ARES_ERRNO_INVALID_CONTACT_STRING:
			return "Invalid contact string.";
		case FSD_ARES_ERRNO_INVALID_ATTRIBUTE_FORMAT:
			return "Invalid format of job attribute.";
		case FSD_ARES_ERRNO_INVALID_ATTRIBUTE_VALUE:
			return "Invalid value of job attribute.";
		case FSD_ARES_ERRNO_CONFLICTING_ATTRIBUTE_VALUES:
			return "Value of attribute conflicts with other attribute value.";
		case FSD_ARES_ERRNO_INVALID_ARES:
			return "Invalid advance reservation identifier..";

		default:
			return "Unknown error code!?";
	 }
}


static const int fsd_stacktrace_length = 32;

void
fsd_assertion_failed(
		const char *file, int lineno,
		const char *function, const char *precondition
		)
{
	char *message = NULL;
	if( asprintf( &message, "%s:%d: %s: Assertion `%s' failed.",
			file, lineno, function, precondition ) == -1 )
		message = NULL;
	if( message != NULL )
		fsd_log_fatal(( "%s", message ));
	fsd_log_stacktrace( 1, fsd_stacktrace_length );
	if( message != NULL )
		free( message );
	abort();
}


void *
fsd_exc_try_except(
		void*(*f)(void*), void *data,
		int *error_code,
		char **error_message
		)
{
	void *result = NULL;
	TRY
	 {
		result = f( data );
	 }
	EXCEPT_DEFAULT
	 {
		const fsd_exc_t *e = fsd_exc_get();
		*error_code = e->code(e);
		*error_message = fsd_strdup( e->message(e) );
	 }
	ELSE
	 {
		*error_code = FSD_ERRNO_SUCCESS;
		*error_message = NULL;
	 }
	END_TRY
	return result;
}


