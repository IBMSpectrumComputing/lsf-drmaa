/* $Id: common.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__COMMON_H
#define __DRMAA_UTILS__COMMON_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/compat.h>
#include <drmaa_utils/exception.h>
#include <drmaa_utils/logging.h>
#include <drmaa_utils/xmalloc.h>

#define FSD_SAFE_STR(str) ( str != NULL ? str : "(null)" )

/* configuration */
typedef struct fsd_conf_option_s  fsd_conf_option_t;
typedef struct fsd_conf_dict_s    fsd_conf_dict_t;

/* iterator */
typedef struct fsd_iter_s fsd_iter_t;

/* template type */
typedef struct fsd_attribute_s  fsd_attribute_t;
typedef struct fsd_template_s   fsd_template_t;

/* DRMAA structures */
typedef struct fsd_drmaa_singletone_s  fsd_drmaa_singletone_t;
typedef struct fsd_drmaa_session_s     fsd_drmaa_session_t;
typedef struct fsd_job_set_s           fsd_job_set_t;
typedef struct fsd_job_s               fsd_job_t;
typedef struct fsd_expand_drmaa_ph_s   fsd_expand_drmaa_ph_t;

#endif /* __DRMAA_UTILS__COMMON_H */

