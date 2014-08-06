/* $Id: datetime_impl.h 2 2009-10-12 09:51:22Z mamonski $ */
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
 * @file datetime_impl.h
 * DRMAA date/time parser - Bison interface functions.
 */
#ifndef __DRMAA_UTILS__DATETIME_IMPL_H
#define __DRMAA_UTILS__DATETIME_IMPL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/datetime.h>

/** @addtogroup datetime */
/* @{ */

typedef struct fsd_dt_parser_s fsd_dt_parser_t;
typedef struct fsd_dt_lexer_s fsd_dt_lexer_t;
union YYSTYPE;

/** Date/time parser data. */
struct fsd_dt_parser_s {
	fsd_dt_lexer_t *lexer;  /**< Lexical analyzer. */
	fsd_datetime_t result;  /**< Parsing result. */
	int n_errors; /**< Number of parse errors. */
};

/** Date/time lexical analyzer. */
struct fsd_dt_lexer_s {
	fsd_dt_parser_t *parser;  /**< Date/time parser. */
	const unsigned char *begin; /**< Begin of parsed string. */
	const unsigned char *end;   /**< End of parsed string. */
	const unsigned char *p;     /**< Scanner position
		(points to first not parsed character). */
};

/** Parser interface function (Bison generated). */
int fsd_dt_parse( fsd_dt_parser_t *parser, fsd_dt_lexer_t *lexer );

/**
 * Error reporting function (hand written).
 */
void fsd_dt_error(
		fsd_dt_parser_t *parser, fsd_dt_lexer_t *lexer,
		const char *fmt, ...
		);

/** Lexer interface (hand written). */
int fsd_dt_lex( union YYSTYPE *lvalp, fsd_dt_lexer_t *lexer );

/* @} */

#endif /* __DRMAA_UTILS__DATETIME_IMPL_H */

