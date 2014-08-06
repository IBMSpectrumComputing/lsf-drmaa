/* $Id: exception.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__EXCEPTION_H
#define __DRMAA_UTILS__EXCEPTION_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdarg.h>
#include <setjmp.h>

#include <drmaa_utils/compat.h>

#if defined(__GNUC__) && defined(DEBUGGING)
extern int setjmp( jmp_buf __env )
		__attribute__(( returns_twice ));
#endif


/** Exception type. */
typedef struct fsd_exc_s fsd_exc_t;

fsd_exc_t *
fsd_exc_new( int code, char *message, bool own_message );

/** Exception structure. */
struct fsd_exc_s {
	int (*code)( const fsd_exc_t* );
	const char* (*message)( const fsd_exc_t* );
	void (*destroy)( fsd_exc_t* );

	int _code;
	char *_message;
	bool _own_message;
	bool _own_self;
};

void
fsd_exc_raise( fsd_exc_t *exc )
		__attribute__(( noreturn ));

void
fsd_exc_raise_code( int error_code )
		__attribute__(( noreturn ));

void
fsd_exc_raise_msg( int error_code, const char *message )
		__attribute__(( noreturn ));

void
fsd_exc_raise_fmt( int error_code, const char *fmt, ... )
		__attribute__(( noreturn, format( printf, 2, 3 ) ));

void
fsd_exc_raise_fmtv( int error_code, const char *fmt, va_list args )
		__attribute__(( noreturn ));

void
fsd_exc_raise_sys( int errno_code )
		__attribute__(( noreturn ));

void
fsd_exc_reraise(void)
		__attribute__(( noreturn ));

const fsd_exc_t *
fsd_exc_get(void);

void
fsd_exc_clear(void);


#define TRY \
	 { \
		fsd_exc_try_block_t* volatile _fsd_exc_try_block = NULL; \
		int _fsd_exc_rc; \
		_fsd_exc_try_block = fsd_exc_try( __FUNCTION__, __LINE__ ); \
		if( _fsd_exc_try_block != NULL ) \
			_fsd_exc_rc = setjmp( _fsd_exc_try_block->env ); \
		else \
			_fsd_exc_rc = FSD_ERRNO_EXC_END; \
		while(1) \
		 { \
			bool _fsd_exc_handled = false; \
			fsd_exc_control( _fsd_exc_try_block, &_fsd_exc_rc ); \
			if( _fsd_exc_rc == FSD_ERRNO_EXC_END ) \
				break; \
			switch( _fsd_exc_rc ) \
			 { \
				case 0: {
#define EXCEPT(error_code) \
				 } break; \
				case error_code: { \
					_fsd_exc_handled = true;
#define EXCEPT_DEFAULT \
				 } break; \
				default: \
					if( _fsd_exc_rc > 0 ) \
					 { \
						_fsd_exc_handled = true;
#define FINALLY \
				 } break; \
				case FSD_ERRNO_EXC_FINALLY: {
#define ELSE \
				 } break; \
				case FSD_ERRNO_EXC_ELSE: {
#define END_TRY \
				 } break; \
			 } \
			if( _fsd_exc_handled ) \
			 { \
				fsd_assert( _fsd_exc_try_block->handled_exc != NULL ); \
				_fsd_exc_try_block->handled_exc->destroy( \
						_fsd_exc_try_block->handled_exc ); \
				_fsd_exc_try_block->handled_exc = NULL; \
			 } \
		 } \
	 }


/** The state of exception handling block. */
typedef enum {
	FSD_EXC_ENTER, /**< Before try. */
	FSD_EXC_TRY_BLOCK, /**< Insidie try-block. */
	FSD_EXC_EXCEPTION_HANDLE, /**< Inside exception handling block. */
	FSD_EXC_ELSE_BLOCK, /**< Inside else-block. */
	FSD_EXC_FINALLY_BLOCK, /**< Inside finally block. */
	FSD_EXC_LEAVE /**< Leaving try/except/finally block (after finally). */
} fsd_exc_try_block_state_t;

/**
 * Try/except/else/finally block data structure.
 * Holds the state for exception handling module.
 *
 * It also represents a point on the stack and other state of
 * thread to wich it can be restored during "stack rollback"
 * process after exception is raised.
 */
typedef struct fsd_exc_try_block_s {
	jmp_buf env;  /**< Stack restore point for \c longjmp. */
	fsd_exc_t *handled_exc; /**< Exception handled within block */
	fsd_exc_try_block_state_t state;
	const char *function; /**< Name of function */
	int lineno; /**< Line number where try-block starts */
} fsd_exc_try_block_t;


fsd_exc_try_block_t *
fsd_exc_try( const char *function, int lineno );

void
fsd_exc_control( fsd_exc_try_block_t *block, int *rc );


const char *
fsd_strerror( int error_code );

/** Error codes. */
typedef enum {
	FSD_ERRNO_SUCCESS                    =  0,

	FSD_ERRNO_EXC_ELSE                   = -1,
	FSD_ERRNO_EXC_FINALLY                = -2,
	FSD_ERRNO_EXC_END                    = -3,

	/* _FSD_ERRNO_START = 1000, */

	FSD_ERRNO_INTERNAL_ERROR             = 1001,
	FSD_ERRNO_NO_MEMORY                  = 1002,
	FSD_ERRNO_INVALID_ARGUMENT           = 1003,
	FSD_ERRNO_INVALID_VALUE              = 1004,
	FSD_ERRNO_INVALID_VALUE_FORMAT       = 1005,
	FSD_ERRNO_STOP_ITERATION             = 1006,
	FSD_ERRNO_NOT_IMPLEMENTED            = 1007,
	FSD_ERRNO_NOT_INITIALIZED            = 1008,
	FSD_ERRNO_TIMEOUT                    = 1009,
	FSD_ERRNO_AUTH_FAILURE               = 1010,
	FSD_ERRNO_AUTHZ_FAILURE              = 1011,
	FSD_ERRNO_TRY_LATER                  = 1012,

	FSD_ERRNO_DRM_COMMUNICATION_FAILURE  = 1013,
	FSD_ERRNO_DRMS_INIT_FAILED           = 1014,
	FSD_ERRNO_DRMS_EXIT_ERROR            = 1015,
	FSD_ERRNO_DENIED_BY_DRM              = 1016,

	/* DRMAA specific error codes: */
	FSD_DRMAA_ERRNO_NO_ACTIVE_SESSION                    = 1032,
	FSD_DRMAA_ERRNO_INVALID_CONTACT_STRING               = 1033,
	FSD_DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR         = 1034,
	FSD_DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED   = 1035,
	FSD_DRMAA_ERRNO_ALREADY_ACTIVE_SESSION               = 1036,
	FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT             = 1037,
	FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE              = 1038,
	FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES         = 1039,
	FSD_DRMAA_ERRNO_INVALID_JOB                          = 1040,
	FSD_DRMAA_ERRNO_RESUME_INCONSISTENT_STATE            = 1041,
	FSD_DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE           = 1042,
	FSD_DRMAA_ERRNO_HOLD_INCONSISTENT_STATE              = 1043,
	FSD_DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE           = 1044,
	FSD_DRMAA_ERRNO_EXIT_TIMEOUT                         = 1045,
	FSD_DRMAA_ERRNO_NO_RUSAGE                            = 1046,

	/* ARes specific error codes: */
	FSD_ARES_ERRNO_INVALID_CONTACT_STRING                = 1052,
	FSD_ARES_ERRNO_INVALID_ATTRIBUTE_FORMAT              = 1053,
	FSD_ARES_ERRNO_INVALID_ATTRIBUTE_VALUE               = 1054,
	FSD_ARES_ERRNO_CONFLICTING_ATTRIBUTE_VALUES          = 1055,
	FSD_ARES_ERRNO_INVALID_ARES                          = 1056,

	FSD_MIN_ERRNO = FSD_ERRNO_INTERNAL_ERROR,
	FSD_MAX_ERRNO = FSD_ARES_ERRNO_INVALID_ARES
} fsd_errno_t;


#ifndef NDEBUG
#	define fsd_assert( precondition ) \
		do { \
			if( ! (precondition) ) \
				fsd_assertion_failed( __FILE__, __LINE__, \
						__FUNCTION__, #precondition ); \
		} while(0)
#else
#	define fsd_assert( precondition )  do{/* nothing */}while(0)
#endif

void
fsd_assertion_failed( const char *file, int lineno,
		const char *function, const char *precondition )
		__attribute__(( noreturn ));

void *
fsd_exc_try_except(
		void*(*f)(void*), void *data,
		int *error_code,
		char **error_message
		);

#endif /* __DRMAA_UTILS__EXCEPTION_H */

