/* $Id: drmaa_util.c 2 2009-10-12 09:51:22Z mamonski $ */
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

#include <string.h>
#include <stdlib.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/drmaa_attrib.h>
#include <drmaa_utils/drmaa_util.h>
#include <drmaa_utils/logging.h>
#include <drmaa_utils/template.h>
#include <drmaa_utils/util.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: drmaa_util.c 2 2009-10-12 09:51:22Z mamonski $";
#endif


static char *
fsd_expand_drmaa_ph_expand( fsd_expand_drmaa_ph_t *self, char *s, unsigned set )
{
	unsigned mask;

	for(
			mask = 1;
			mask <= FSD_DRMAA_PH_INCR;
			mask <<= 1
			)
	 {
		const char *ph;
		const char *value;

		switch( mask )
		 {
			case FSD_DRMAA_PH_HD:
				ph = DRMAA_PLACEHOLDER_HD;
				value = self->home_directory;
				break;
			case FSD_DRMAA_PH_WD:
				ph = DRMAA_PLACEHOLDER_WD;
				value = self->working_directory;
				break;
			case FSD_DRMAA_PH_INCR:
				ph = DRMAA_PLACEHOLDER_INCR;
				value = self->bulk_incr;
				break;
			default:
				fsd_assert( false );
		 }
		if( value == NULL )
			value = "";

		if( mask & set )
			s = fsd_replace( s, ph, value );
		else if( strstr( s, ph ) != NULL )
		 {
			fsd_log_warning((
						"'%s' can not be expanded in '%s'; removing from string",
						ph, s ));
			s = fsd_replace( s, ph, "" );
		 }
	 }

	return s;
}

static void
fsd_expand_drmaa_ph_set( fsd_expand_drmaa_ph_t *self,
		int placeholder, char *value )
{
	char **var;
	switch( placeholder )
	 {
		case FSD_DRMAA_PH_HD:  var = &self->home_directory;  break;
		case FSD_DRMAA_PH_WD:  var = &self->working_directory;  break;
		case FSD_DRMAA_PH_INCR:  var = &self->bulk_incr;  break;
		default:
			fsd_free( value );
			fsd_exc_raise_code( FSD_ERRNO_INVALID_ARGUMENT );
	 }
	fsd_free( *var );
	*var = value;
}

static void
fsd_expand_drmaa_ph_destroy( fsd_expand_drmaa_ph_t *self )
{
	if( self != NULL )
	 {
		fsd_free( self->home_directory );
		fsd_free( self->working_directory );
		fsd_free( self->bulk_incr );
		fsd_free( self );
	 }
}

fsd_expand_drmaa_ph_t *
fsd_expand_drmaa_ph_new(
		char *home_directory,
		char *working_directory,
		char *bulk_incr
		)
{
	fsd_expand_drmaa_ph_t *volatile p = NULL;

	TRY
	 {
		fsd_expand_drmaa_ph_t *self;
		fsd_malloc( p, fsd_expand_drmaa_ph_t );
		self = p;
		self->expand = fsd_expand_drmaa_ph_expand;
		self->set = fsd_expand_drmaa_ph_set;
		self->destroy = fsd_expand_drmaa_ph_destroy;

		self->home_directory = home_directory;
		self->working_directory = working_directory;
		self->bulk_incr = bulk_incr;

		if( self->home_directory == NULL )
		 {
			const char *home = getenv("HOME");
			if( home == NULL )
				home = "";
			self->home_directory = fsd_strdup( home );
		 }

		if( self->working_directory == NULL )
			self->working_directory = fsd_strdup(
					self->home_directory );
		else
			self->working_directory = self->expand( self,
					self->working_directory, FSD_DRMAA_PH_HD );

		if( self->bulk_incr == NULL )
			self->bulk_incr = fsd_strdup( "" );
	 }
	EXCEPT_DEFAULT
	 {
		if( p )
			p->destroy( p );
		fsd_exc_reraise();
	 }
	END_TRY
	return p;
}


const char *
drmaa_control_to_str( int action )
{
	switch( action )
	 {
		case DRMAA_CONTROL_SUSPEND:
			return "suspend";
		case DRMAA_CONTROL_RESUME:
			return "resume";
		case DRMAA_CONTROL_HOLD:
			return "hold";
		case DRMAA_CONTROL_RELEASE:
			return "release";
		case DRMAA_CONTROL_TERMINATE:
			return "terminate";
		default:
			return "?";
	 }
}


const char *
drmaa_job_ps_to_str( int ps )
{
	switch( ps )
	 {
		case DRMAA_PS_UNDETERMINED:
			return "undetermined";
		case DRMAA_PS_QUEUED_ACTIVE:
			return "queued_active";
		case DRMAA_PS_SYSTEM_ON_HOLD:
			return "systen_on_hold";
		case DRMAA_PS_USER_ON_HOLD:
			return "user_on_hold";
		case DRMAA_PS_USER_SYSTEM_ON_HOLD:
			return "user_systen_on_hold";
		case DRMAA_PS_RUNNING:
			return "running";
		case DRMAA_PS_SYSTEM_SUSPENDED:
			return "system_suspended";
		case DRMAA_PS_USER_SUSPENDED:
			return "user_suspended";
		case DRMAA_PS_USER_SYSTEM_SUSPENDED:
			return "user_system_suspended";
		case DRMAA_PS_DONE:
			return "done";
		case DRMAA_PS_FAILED:
			return "failed";
		default:
			return "?";
	 }
}


int
fsd_drmaa_code( int fsd_errno )
{
	switch( fsd_errno )
	 {
		case FSD_ERRNO_SUCCESS:
			return DRMAA_ERRNO_SUCCESS;
		case FSD_ERRNO_INTERNAL_ERROR:
			return DRMAA_ERRNO_INTERNAL_ERROR;
		case FSD_ERRNO_NO_MEMORY:
			return DRMAA_ERRNO_NO_MEMORY;
		case FSD_ERRNO_INVALID_ARGUMENT:
			return DRMAA_ERRNO_INVALID_ARGUMENT;
		case FSD_ERRNO_INVALID_VALUE:
			return DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE;
		case FSD_ERRNO_INVALID_VALUE_FORMAT:
			return DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT;
		case FSD_ERRNO_STOP_ITERATION:
			return DRMAA_ERRNO_NO_MORE_ELEMENTS;
		case FSD_ERRNO_NOT_IMPLEMENTED:
			return DRMAA_ERRNO_INTERNAL_ERROR;
		case FSD_ERRNO_NOT_INITIALIZED:
			return DRMAA_ERRNO_NO_ACTIVE_SESSION;
		case FSD_ERRNO_TIMEOUT:
			return DRMAA_ERRNO_EXIT_TIMEOUT;
		case FSD_ERRNO_AUTH_FAILURE:
			return DRMAA_ERRNO_AUTH_FAILURE;
		case FSD_ERRNO_AUTHZ_FAILURE:
			return DRMAA_ERRNO_AUTH_FAILURE;
		case FSD_ERRNO_TRY_LATER:
			return DRMAA_ERRNO_TRY_LATER;
		case FSD_ERRNO_DRM_COMMUNICATION_FAILURE:
			return DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE;
		case FSD_ERRNO_DRMS_INIT_FAILED:
			return DRMAA_ERRNO_DRMS_INIT_FAILED;
		case FSD_ERRNO_DRMS_EXIT_ERROR:
			return DRMAA_ERRNO_DRMS_EXIT_ERROR;
		case FSD_ERRNO_DENIED_BY_DRM:
			return DRMAA_ERRNO_DENIED_BY_DRM;

		case FSD_DRMAA_ERRNO_NO_ACTIVE_SESSION:
			return DRMAA_ERRNO_NO_ACTIVE_SESSION;
		case FSD_DRMAA_ERRNO_INVALID_CONTACT_STRING:
			return DRMAA_ERRNO_INVALID_CONTACT_STRING;
		case FSD_DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR:
			return DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR;
		case FSD_DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED:
			return DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED;
		case FSD_DRMAA_ERRNO_ALREADY_ACTIVE_SESSION:
			return DRMAA_ERRNO_ALREADY_ACTIVE_SESSION;
		case FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT:
			return DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT;
		case FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE:
			return DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE;
		case FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES:
			return DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES;
		case FSD_DRMAA_ERRNO_INVALID_JOB:
			return DRMAA_ERRNO_INVALID_JOB;
		case FSD_DRMAA_ERRNO_RESUME_INCONSISTENT_STATE:
			return DRMAA_ERRNO_RESUME_INCONSISTENT_STATE;
		case FSD_DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE:
			return DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE;
		case FSD_DRMAA_ERRNO_HOLD_INCONSISTENT_STATE:
			return DRMAA_ERRNO_HOLD_INCONSISTENT_STATE;
		case FSD_DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE:
			return DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE;
		case FSD_DRMAA_ERRNO_EXIT_TIMEOUT:
			return DRMAA_ERRNO_EXIT_TIMEOUT;
		case FSD_DRMAA_ERRNO_NO_RUSAGE:
			return DRMAA_ERRNO_NO_RUSAGE;

		default:
			return DRMAA_ERRNO_INTERNAL_ERROR;
	 }
}


int
fsd_errno_from_drmaa_code( int drmaa_errno )
{
	switch( drmaa_errno )
	 {
		case DRMAA_ERRNO_SUCCESS:
			return FSD_ERRNO_SUCCESS;
		case DRMAA_ERRNO_INTERNAL_ERROR:
			return FSD_ERRNO_INTERNAL_ERROR;
		case DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE:
			return FSD_ERRNO_DRM_COMMUNICATION_FAILURE;
		case DRMAA_ERRNO_AUTH_FAILURE:
			return FSD_ERRNO_AUTHZ_FAILURE;
		case DRMAA_ERRNO_INVALID_ARGUMENT:
			return FSD_ERRNO_INVALID_ARGUMENT;
		case DRMAA_ERRNO_NO_ACTIVE_SESSION:
			return FSD_ERRNO_NOT_INITIALIZED;
		case DRMAA_ERRNO_NO_MEMORY:
			return FSD_ERRNO_NO_MEMORY;
		case DRMAA_ERRNO_INVALID_CONTACT_STRING:
			return FSD_DRMAA_ERRNO_INVALID_CONTACT_STRING;
		case DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR:
			return FSD_DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR;
		case DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED:
			return FSD_DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED;
		case DRMAA_ERRNO_DRMS_INIT_FAILED:
			return FSD_ERRNO_DRMS_INIT_FAILED;
		case DRMAA_ERRNO_ALREADY_ACTIVE_SESSION:
			return FSD_DRMAA_ERRNO_ALREADY_ACTIVE_SESSION;
		case DRMAA_ERRNO_DRMS_EXIT_ERROR:
			return FSD_ERRNO_DRMS_EXIT_ERROR;
		case DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT:
			return FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT;
		case DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE:
			return FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE;
		case DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES:
			return FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES;
		case DRMAA_ERRNO_TRY_LATER:
			return FSD_ERRNO_TRY_LATER;
		case DRMAA_ERRNO_DENIED_BY_DRM:
			return FSD_ERRNO_DENIED_BY_DRM;
		case DRMAA_ERRNO_INVALID_JOB:
			return FSD_DRMAA_ERRNO_INVALID_JOB;
		case DRMAA_ERRNO_RESUME_INCONSISTENT_STATE:
			return FSD_DRMAA_ERRNO_RESUME_INCONSISTENT_STATE;
		case DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE:
			return FSD_DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE;
		case DRMAA_ERRNO_HOLD_INCONSISTENT_STATE:
			return FSD_DRMAA_ERRNO_HOLD_INCONSISTENT_STATE;
		case DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE:
			return FSD_DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE;
		case DRMAA_ERRNO_EXIT_TIMEOUT:
			return FSD_DRMAA_ERRNO_EXIT_TIMEOUT;
		case DRMAA_ERRNO_NO_RUSAGE:
			return FSD_DRMAA_ERRNO_NO_RUSAGE;
		case DRMAA_ERRNO_NO_MORE_ELEMENTS:
			return FSD_ERRNO_STOP_ITERATION;
		default:
			fsd_assert(false);
			return FSD_ERRNO_INTERNAL_ERROR;
	 }
}


const char *
fsd_drmaa_strerror( int drmaa_errno )
{
	switch( drmaa_errno )
	 {
		case DRMAA_ERRNO_SUCCESS:
			return "Success.";
		case DRMAA_ERRNO_INTERNAL_ERROR:
			return "Unexpected or internal DRMAA error.";
		case DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE:
			return "Could not contact DRM system for this request.";
		case DRMAA_ERRNO_AUTH_FAILURE:
			return "Authorization failure.";
		case DRMAA_ERRNO_INVALID_ARGUMENT:
			return "Invalid argument value.";
		case DRMAA_ERRNO_NO_ACTIVE_SESSION:
			return "No active DRMAA session.";
		case DRMAA_ERRNO_NO_MEMORY:
			return "Not enough memory.";
		case DRMAA_ERRNO_INVALID_CONTACT_STRING:
			return "Invalid contact string.";
		case DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR:
			return "Can not determine default contact to DRM system.";
		case DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED:
			return "Contact to DRM must be set explicitly "
				"because there is no default.";
		case DRMAA_ERRNO_DRMS_INIT_FAILED:
			return "Unable to initialize DRM system.";
		case DRMAA_ERRNO_ALREADY_ACTIVE_SESSION:
			return "DRMAA session already exist.";
		case DRMAA_ERRNO_DRMS_EXIT_ERROR:
			return "Disengagement from the DRM system failed.";
		case DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT:
			return "Invalid format of job attribute.";
		case DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE:
			return "Invalid value of job attribute.";
		case DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES:
			return "Value of attribute conflicts with other attribute value.";
		case DRMAA_ERRNO_TRY_LATER:
			return "DRM system is overloaded.  Try again later.";
		case DRMAA_ERRNO_DENIED_BY_DRM:
			return "DRM rejected job due to its configuration or job attributes.";
		case DRMAA_ERRNO_INVALID_JOB:
			return "Job does not exist in DRMs queue.";
		case DRMAA_ERRNO_RESUME_INCONSISTENT_STATE:
			return "Can not resume job (not in valid state).";
		case DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE:
			return "Can not suspend job (not in valid state).";
		case DRMAA_ERRNO_HOLD_INCONSISTENT_STATE:
			return "Can not hold job (not in valid state).";
		case DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE:
			return "Can not release job (not in valid state).";
		case DRMAA_ERRNO_EXIT_TIMEOUT:
			return "Waiting for job to terminate finished due to time-out.";
		case DRMAA_ERRNO_NO_RUSAGE:
			return "Job finished but resource usage information "
				"and/or termination status could not be provided.";
		case DRMAA_ERRNO_NO_MORE_ELEMENTS:
			return "Vector have no more elements.";

		default:
			return "Unknown DRMAA error code!?";
	 }
}

