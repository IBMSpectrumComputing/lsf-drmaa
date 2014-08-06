/* $Id: datetime.h 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file datetime.h
 * DRMAA and ISO-8601 date/time parser.
 */

#ifndef __DRMAA_UTILS__DATETIME_H
#define __DRMAA_UTILS__DATETIME_H

#include <drmaa_utils/common.h>

/**
 * @defgroup datetime  DRMAA date/time parser.
 *
 * It parses date/time string in format of
 * drmaa_start_time and drmaa_deadline_time attributes.
 * In other words it accepts time in following format:
 *
 * <tt>  [[[[CC]YY/]MM/]DD] hh:mm[:ss] [{-|+}UU:uu]  </tt>
 *
 * where
 *    CC is the first two digits of the year (century-1),
 *    YY is the last two digits of the year,
 *    MM is the two digits of the month [01,12],
 *    DD is the two-digit day of the month [01,31],
 *    hh is the two-digit hour of the day [00,23],
 *    mm is the two-digit minute of the day [00,59],
 *    ss is the two-digit second of the minute [00,61],
 *    UU is the two-digit hours since (before) UTC,
 *    uu is the two-digit minutes since (before) UTC.
 */
/* @{ */

typedef struct fsd_datetime_s fsd_datetime_t;

/**
 * Parses date/time.
 * @param string  Textual representation to date/time.
 * @return Absolute time according to string.
 */
time_t fsd_datetime_parse( const char *string );

/**
 * Return first timestamp after \a t
 * which match date/time pattern \a dt.
 */
time_t fsd_datetime_after( fsd_datetime_t *dt, time_t t );

/**
 * Fill unset fields of fsd_datetime_t structure according to timestamp.
 * @param dt   Will be filled with local time representation of filler.
 * @param filler  Seconds since epoch.
 */
void fsd_datetime_fill( fsd_datetime_t *dt, time_t filler );

/** Makes UTC datetime from (possibly not absolute) fsd_datetime_t. */
time_t fsd_datetime_mktime( const fsd_datetime_t *dt );

enum{
	FSD_DT_YEAR         = 1<<0,
	FSD_DT_MONTH        = 1<<1,
	FSD_DT_DAY          = 1<<2,
	FSD_DT_HOUR         = 1<<3,
	FSD_DT_MINUTE       = 1<<4,
	FSD_DT_SECOND       = 1<<5,
	FSD_DT_TZ_DELTA     = 1<<6,
	FSD_DT_ALL = FSD_DT_YEAR | FSD_DT_MONTH | FSD_DT_DAY
		| FSD_DT_HOUR | FSD_DT_MINUTE | FSD_DT_SECOND | FSD_DT_TZ_DELTA
};

/** Intermediate result of parsing date/time string (may be incomplete). */
struct fsd_datetime_s {
	unsigned mask;  /**< Bit-set of fields which were set. */
	int year;       /**< Year. */
	int month;      /**< Month. */
	int day;        /**< Day. */
	int hour;       /**< Hour. */
	int minute;     /**< Minute. */
	int second;     /**< Second. */
	long tz_delta;  /**< Timezone; Number of seconds ahead of UTC. */
};


/**
 * Guess local timezone for given UTC time
 * @param t UTC timestamp (time from epoch).
 * @return Numbef of seconds east (since/before) UTC.  For example in CET
 * +3600 is returned (UTC + 1 hour).
 */
long fsd_timezone( time_t t );

/* @} */

#endif /* __DRMAA_UTILS__DATETIME_H */

