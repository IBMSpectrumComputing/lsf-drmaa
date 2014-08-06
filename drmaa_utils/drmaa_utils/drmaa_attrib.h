/* $Id: drmaa_attrib.h 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file attrib.h
 * DRMAA attributes.
 */
#ifndef __DRMAA_UTILS__DRMAA_ATTRIB_H
#define __DRMAA_UTILS__DRMAA_ATTRIB_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/compat.h>
#include <drmaa_utils/drmaa.h>
#include <drmaa_utils/common.h>

/**
 * Attributes codes.
 */
enum {
	/* DRMAA 1.0 attributes: */
	DRMAA_ATTR_REMOTE_COMMAND,
	DRMAA_ATTR_ARGS, /* vector */
	DRMAA_ATTR_JOB_SUBMISSION_STATE,
	DRMAA_ATTR_JOB_ENVIRONMENT, /* vector */
	DRMAA_ATTR_WORKING_DIRECTORY,
	DRMAA_ATTR_JOB_CATEGORY,
	DRMAA_ATTR_NATIVE_SPECIFICATION,
	DRMAA_ATTR_EMAIL, /* vector */
	DRMAA_ATTR_BLOCK_EMAIL,
	DRMAA_ATTR_START_TIME,
	DRMAA_ATTR_JOB_NAME,
	DRMAA_ATTR_INPUT_PATH,
	DRMAA_ATTR_OUTPUT_PATH,
	DRMAA_ATTR_ERROR_PATH,
	DRMAA_ATTR_JOIN_FILES,
	/* optional DRMAA attributes: */
	DRMAA_ATTR_TRANSFER_FILES,
	DRMAA_ATTR_DEADLINE_TIME,
	DRMAA_ATTR_HARD_WALL_CLOCK_TIME_LIMIT,
	DRMAA_ATTR_SOFT_WALL_CLOCK_TIME_LIMIT,
	DRMAA_ATTR_HARD_RUN_DURATION_LIMIT,
	DRMAA_ATTR_SOFT_RUN_DURATION_LIMIT,

	DRMAA_N_ATTRIBUTES
};

fsd_template_t *
drmaa_template_new(void);

#endif /* __DRMAA_UTILS__DRMAA_ATTRIB_H */

