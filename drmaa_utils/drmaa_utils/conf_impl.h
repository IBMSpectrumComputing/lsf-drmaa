/* $Id: conf_impl.h 2 2009-10-12 09:51:22Z mamonski $ */
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

#ifndef __DRMAA_UTILS__CONF_IMPL_H
#define __DRMAA_UTILS__CONF_IMPL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/conf.h>

typedef struct fsd_conf_parser_s fsd_conf_parser_t;
typedef struct fsd_conf_lexer_s fsd_conf_lexer_t;
union YYSTYPE;
struct YYLTYPE;
typedef unsigned char uchar;


int
fsd_conf_parse( fsd_conf_parser_t *parser, fsd_conf_lexer_t *lexer );

int
fsd_conf_lex( union YYSTYPE *lvalp, struct YYLTYPE *locp,
		fsd_conf_lexer_t *lexer );

void
fsd_conf_error(
		struct YYLTYPE *locp,
		fsd_conf_parser_t *parser, fsd_conf_lexer_t *lexer,
		const char *fmt, ...
		);


/** DRMAA configuration file parser data. */
struct fsd_conf_parser_s {
	fsd_conf_lexer_t *lexer;

	/** Parsing result - root of syntax tree. */
	fsd_conf_dict_t *result;

	int n_errors;  /**< Number of parse/lexical errors. */
	char **errors;
};

/** DRMAA configuration file lexical analyzer data. */
struct fsd_conf_lexer_s {
	fsd_conf_parser_t *parser;  /**< Parser which use this lexer. */
	const char *filename; /**< Name of configuration file. */

	const uchar *buffer;  /**< Entire content of parsed configuration file. */
	size_t buflen;  /**< Length of \a buffer. */

	const uchar *pos;  /**< Current position of lexical analyzer. */
	int lineno; /**< Current line number (counted from 1). */
	const uchar *cline;  /**< Points to first character (byte) of current line. */
};

typedef struct fsd_conf_pair_s {
	char *key;
	fsd_conf_option_t *value;
} fsd_conf_pair_t;

#endif /* __DRMAA_UTILS__CONF_IMPL_H */

