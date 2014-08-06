/* $Id: native.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __LSF_DRMAA__NATIVE_H
#define __LSF_DRMAA__NATIVE_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <lsf/lsbatch.h>

void
lsfdrmaa_native_parse( const char *native_spec, struct submit *req );

#endif /* __LSF_DRMAA__NATIVE_H */

