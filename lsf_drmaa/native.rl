/* $Id: native.rl 2298 2009-04-09 16:11:25Z lukasz $ */
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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/datetime.h>
#include <drmaa_utils/logging.h>
#include <drmaa_utils/util.h>
#include <lsf/lsbatch.h>

#define min(a,b) ((a)<(b) ? (a) : (b))


static void
impl_native_add(
		struct submit *req,
		bool long_option,
		const char *option, size_t optlen,
		const char *argument
		);

static time_t
lsfdrmaa_parse_datetime( const char *datetime_string );

static int
lsfdrmaa_parse_timedelta( const char *timedelta_string );

static void
lsfdrmaa_parse_asked_hosts(
		struct submit *req,
		const char *asked_hosts_string
		);

static void
lsfdrmaa_parse_file_transfer(
		struct xFile *transfer,
		const char *transfer_string );


void
lsfdrmaa_native_parse( const char *native_spec, struct submit *req )
{
%%{
	machine lsf_native_specification;
	action short_opt { opt_type = OPT_SHORT; }
	action long_opt  { opt_type = OPT_LONG; }
	action opt_begin { opt_begin = fpc; }
	action opt_end   { opt_end = fpc; }
	action arg       { arg[arglen++] = fc; }
	action option {
		arg[arglen] = '\0';
		impl_native_add(
				req, (opt_type == OPT_LONG),
				opt_begin, opt_end-opt_begin,
				arg
		);
		fsd_log_debug(( "native parse: %s%.*s '%.*s'",
				(opt_type == OPT_SHORT ? "-" : "--"),
				(int)(opt_end - opt_begin), opt_begin,
				(int)arglen, arg
				));
		arglen = 0;
	}
	action error {
		fsd_exc_raise_fmt(
				FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
				"Error in native specification: %s",
				native_spec );
	}
	quoted_string = '"' ([^"]* $arg ) '"'
		| "'" ([^']* $arg) "'";
	char = ^(space | ['"]) $arg;
	argument = char+ | char* (quoted_string char*)+;
	short_with_arg = ([LcCFWkPpGgqRmnJbtuUMDSvfa] | 'i' 's'? | [oe] 'o'?
		| 'w' [at]? | 'E' 'p'? | 'sp' | 'sla' | 'ext' 'sched'?
		| 'jsdl' '_strict'? | 'We' | 'cwd' | 'ar' | 'app' | 'Lp' )
		>opt_begin %opt_end;
	short_without_arg = ([xHNBK] | 'r' 'n'? | 'I' [ps]? | 'Zs')
		>opt_begin %opt_end;
	short_opt = ('-' short_without_arg | '-' short_with_arg space+ argument)
		>short_opt;
	long_with_arg = ('arid') >opt_begin %opt_end;
	long_opt = ('--' @long_opt long_with_arg ( '=' | space+ ) argument);
	option = (short_opt | long_opt) %option;
	option_list = space* option (space+ option)* space*;
	main := (space* | option_list) @eof(error) $err(error);
}%%

	%% write data;
	char *volatile arg = NULL;
	size_t arglen = 0;
	/* size_t argsize = 0; */
	enum{ OPT_SHORT, OPT_LONG } opt_type;
	const char *opt_begin;
	const char *opt_end;

	const char *p = native_spec;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;

	TRY
	 {
		fsd_calloc( arg, strlen(native_spec)+1, char );
		%%write init;
		%%write exec;
	 }
	FINALLY
	 {
		fsd_free( arg );
	 }
	END_TRY

	fsd_log_debug(( "characters parsed=%ld, left=%ld\n",
			(long)(p-native_spec), (long)(pe-p) ));
}


static void
impl_native_add(
		struct submit *req,
		bool long_option,
		const char *option, size_t optlen,
		const char *argument
		)
{
#define ASSIGN_ARG( dest ) \
	do { \
		if( dest != NULL )  fsd_free(dest);  \
		(dest) = fsd_strdup(argument); \
	} while(0)
#define ASSIGN_INTARG( dest ) \
	do { \
		(dest) = fsd_atoi(argument); \
	} while(0)

	if( long_option )
	 {
		if( optlen==4  &&  0==strncmp(option, "arid", 4) )
			goto reservation_id;
		else
			goto unimplemented_option;
	 }
	else
	 {
		switch( option[0] )
		 {
			case 'B':  /* -B: Sends mail to you when the job
			                  is dispatched and begins execution. */
				req->options |= SUB_NOTIFY_BEGIN;
				break;

			case 'H':  /* -H: Holds the job in the PSUSP state
			                  when the job is submitted. */
				req->options2 |= SUB2_HOLD;
				break;

			case 'I':  /* -I[ps]: Interactive job */
				goto unimplemented_option;

			case 'K':  /* -K: Submits a batch job and waits
			                  for the job to complete. */
				goto unimplemented_option;

			case 'N':  /* -N: Sends the job report to you by mail
			                  when the job finishes. */
				req->options |= SUB_NOTIFY_END;
				break;

			case 'r':  /* -r -rn: Job is (not) rerunnable. */
				if( optlen == 1 ) /* -r: rerunnable */
				 {
					req->options |= SUB_RERUNNABLE;
#ifdef SUB3_NOT_RERUNNABLE
					req->options3 &= ~SUB3_NOT_RERUNNABLE;
#endif
				 }
				else /* -rn: not rerunnable */
				 {
					req->options &= ~SUB_RERUNNABLE;
#ifdef SUB3_NOT_RERUNNABLE
					req->options3 |= SUB3_NOT_RERUNNABLE;
#else
					goto unimplemented_option;
#endif
				 }
				break;

			case 'x': /* -x: Puts the host running your job
			                 into exclusive execution mode. */
				req->options |= SUB_EXCLUSIVE;
				break;

			case 'a': /* -a esub_application
			             -app application_profile_name
			             -ar reservation_id (SGE like)
			           */
				switch( optlen )
				 {
					case 1: /* -a */
						goto unimplemented_option; /* ?? TODO */
					case 2: /* -ar */
						goto reservation_id;
					case 3: /* -app */
#ifdef SUB3_APP
						req->options3 |= SUB3_APP;
						ASSIGN_ARG( req->app );
#else
						goto unimplemented_option;
#endif
						break;
				 }
				break;

			case 'b':
				/*
				 * -b [[month:]day:]hour:minute
				 *   Dispatches the job for execution on or after
				 *   the specified date and time.
				 */
				req->beginTime = lsfdrmaa_parse_datetime( argument );
				break;

			case 'C':  /* -C core_limit: Core limit in KiB */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_CORE] );
				break;

			case 'c':
				/*
				 * -c [hour:]minute[/host_name | /host_model]
				 *   cpu time limit
				 * -cwd working_directory
				 */
				if( optlen == 1 )
					ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_CPU] );
				else if( optlen == 3 )
				 {
#ifdef SUB3_CWD
					req->options3 |= SUB3_CWD;
					ASSIGN_ARG( req->cwd );
#else
					goto unimplemented_option;
#endif
				 }
				break;

			case 'D':  /* -D data_limit */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_DATA] );
				break;

			case 'E':  /* -E pre_exec_command
			              -Ep post_exec_command */
				if( optlen == 1 )
				 {
					req->options |= SUB_PRE_EXEC;
					ASSIGN_ARG( req->preExecCmd );
				 }
				else
				 {
#ifdef SUB3_POST_EXEC
					req->options3 |= SUB3_POST_EXEC;
					ASSIGN_ARG( req->postExecCmd );
#else
					goto unimplemented_option;
#endif
				 }
				break;

			case 'e':  /* -e[o] error_file
			              -ext[sched] external_scheaduler_options */
				if( optlen <= 2 )
				 {
					ASSIGN_ARG( req->errFile );
					req->options |= SUB_ERR_FILE;
					if( optlen == 2 )
					 {
#ifdef SUB2_OVERWRITE_OUT_FILE
						req->options2 |= SUB2_OVERWRITE_ERR_FILE;
#else
						goto unimplemented_option;
#endif
					 }
				 }
				else
				 {
					req->options2 |= SUB2_EXTSCHED;
					ASSIGN_ARG( req->extsched );
				 }
				break;

			case 'F':  /* -F file_limit */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_FSIZE] );
				break;

			case 'f':  /* -f local_file operator [remote_file] */
			 {
				struct xFile *t;
				fsd_realloc( req->xf, req->nxf+1, struct xFile );
				t = &req->xf[ req->nxf++ ];
				lsfdrmaa_parse_file_transfer( t, argument );
				break;
			 }

			case 'G':  /* -G user_group */
				req->options |= SUB_USER_GROUP;
				ASSIGN_ARG( req->userGroup );
				break;

			case 'g':  /* -g job_group_name */
				req->options |= SUB2_JOB_GROUP;
				ASSIGN_ARG( req->jobGroup );
				break;

			case 'i':  /* -i[s] input_file */
				req->options |= SUB_IN_FILE;
				ASSIGN_ARG( req->inFile );
				if( optlen == 2 )
					req->options2 |= SUB2_IN_FILE_SPOOL;
				break;

			case 'J':  /* -J job_name */
				req->options |= SUB_JOB_NAME;
				ASSIGN_ARG( req->jobName );
				break;

			case 'j':  /* -jsdl[_strict] file_name */
#if LSF_PRODUCT_MAJOR_VERSION >= 7  /* FIXME */
				ASSIGN_ARG( req->jsdlDoc );
				if( optlen == 4 )
					req->jsdlFlag = 1;
				else
					req->jsdlFlag = 0;
#else
				goto unimplemented_option;
#endif
				break;

			case 'k':  /* -k checkpoint_dir */
				req->options |= SUB_CHKPNT_DIR;
				ASSIGN_ARG( req->chkpntDir );
				break;

			case 'L':  /* -L login_shell
			              -Lp ls_project_name */
				if( optlen == 1 )
				 {
					req->options |= SUB_LOGIN_SHELL;
					ASSIGN_ARG( req->loginShell );
				 }
				else
				 {
#ifdef SUB2_LICENSE_PROJECT
					req->options2 |= SUB2_LICENSE_PROJECT;
					ASSIGN_ARG( req->licenseProject );
#else
					goto unimplemented_option;
#endif
				 }
				break;

			case 'M':  /* -M memory_limit: Memory limit in KiB*/
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_RSS] );
				break;

			case 'm':  /* -m host_name...: runs job on specified hosts */
			 {
				lsfdrmaa_parse_asked_hosts( req, argument );
				break;
			 }

			case 'n':  /* -n min_proc[,max_proc] */
			 {
				const char *p = argument, *end;
				int value;
				value = strtol( p, (char**)&end, 10 );
				if( p < end  &&  (end[0] == '\0' || end[0] == ',') )
				 {
					req->numProcessors = value;
					p = end;
					if( p[0] == ',' )
						p++;
				 }
				else
					fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
							"Invalid argument to -n option in native specification: %s",
							argument );

				value = strtol( p, (char**)&end, 10 );
				if( p < end  &&  end[0] == '\0' )
					req->maxNumProcessors = value;
				else if( p[-1] != ','  &&  end[0] == '\0' )
					req->maxNumProcessors = req->numProcessors;
				else
					fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
							"Invalid argument to -n option in native specification: %s",
							argument );
				break;
			 }

			case 'o':  /* -o[o] output_file */
				req->options |= SUB_OUT_FILE;
				ASSIGN_ARG( req->outFile );
				if( optlen==2 )
				 {
#ifdef SUB2_OVERWRITE_ERR_FILE
					req->options2 |= SUB2_OVERWRITE_OUT_FILE;
#else
					goto unimplemented_option;
#endif
				 }
				break;

			case 'P':  /* -P project_name */
				req->options |= SUB_PROJECT_NAME;
				ASSIGN_ARG( req->projectName );
				break;

			case 'p':  /* -p process_limit */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_PROCESS] );
				break;

			case 'q':  /* -q queue_name */
				req->options |= SUB_QUEUE;
				ASSIGN_ARG( req->queue );
				break;

			case 'R':  /* -R res_req */
				req->options |= SUB_RES_REQ;
				ASSIGN_ARG( req->resReq );
				break;

			case 'S':  /* -S stack_limit: stack limit in KiB */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_STACK] );
				break;

			case 's':
				/* -s signal: sends signal when queue run window closes
				 * -sla service_class_name
				 * -sp priority
				 */
				if( optlen == 1 )
				 {
					req->sigValue = SIGTERM; /* TODO */
					req->options = SUB_WINDOW_SIG;
				 }
				else if( optlen == 3 )
				 {
					req->options2 |= SUB2_SLA;
					ASSIGN_ARG( req->sla );
				 }
				else if( optlen == 2 ) /* -sp */
				 {
					req->options2 |= SUB2_JOB_PRIORITY;
					ASSIGN_INTARG( req->userPriority );
				 }
				break;

			case 'T':  /* -T thread_limit */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_THREAD] );
				break;

			case 't':  /* -t [[month:]day:]hour:minute
			                 Specifies the job termination deadline. */
			 {
				req->termTime = lsfdrmaa_parse_datetime( argument );
				break;
			 }

			case 'U':  /* -U reservation_id */
			reservation_id:
				req->options2 |= SUB2_USE_RSV;
				ASSIGN_ARG( req->rsvId );
				break;

			case 'u':
				/* -ul
				 *   Passes the current operating system user shell
				 *   limits for the job submission user to the
				 *   execution host.
				 * -u mail_user
				 */
				if( optlen == 2 )
				 {
#ifdef SUB3_USER_SHELL_LIMITS
					req->options3 |= SUB3_USER_SHELL_LIMITS;
#else
					goto unimplemented_option;
#endif
				 }
				else
				 {
					req->options |= SUB_MAIL_USER;
					ASSIGN_ARG( req->mailUser );
				 }
				break;

			case 'v':  /* -v swap_limt: Swap limit in KiB */
				ASSIGN_INTARG( req->rLimits[LSF_RLIMIT_SWAP] );
				break;

			case 'W':
				/*
				 * -W [hour:]minute[/host_name | /host_model]
				 *   Sets the runtime limit of the batch job.
				 * -We [hour:]minute[/host_name | /host_model]
				 *   Specifies an estimated run time for the job.
				 */
				if( optlen==1 )
					req->rLimits[LSF_RLIMIT_RUN] = lsfdrmaa_parse_timedelta( argument );
				else
				 {
#ifdef SUB3_RUNTIME_ESTIMATION
					req->options3 = SUB3_RUNTIME_ESTIMATION;
					req->runtimeEstimation = lsfdrmaa_parse_timedelta( argument );
#else
					goto unimplemented_option;
#endif
				 }
				break;

			case 'w':
				/*
				 * -w 'dependency_expression'
				 * -wa ’[signal | command | CHKPNT]’
				 *   Specifies the job action to be taken before a job control
				 *   action occurs.
				 * -wt ’[hour:]minute’
				 *   Specifies the amount of time before a job control action
				 *   occurs that a job warning action is to be taken.
				 */
				if( optlen == 1 )
				 {
					req->options |= SUB_DEPEND_COND;
					ASSIGN_ARG( req->dependCond );
				 }
				else if( option[1] == 'a' )
				 {
					req->options2 |= SUB2_WARNING_ACTION;
					ASSIGN_ARG( req->warningAction );
				 }
				else
				 {
					req->options2 |= SUB2_WARNING_TIME_PERIOD;
					req->warningTimePeriod = lsfdrmaa_parse_timedelta( argument );
				 }
				break;

			case 'Z':
				/*
				 * -Zs
				 *   Spools a job command file to the directory specified by the
				 *   JOB_SPOOL_DIR parameter in lsb.params, and uses the spooled
				 *   file as the command file for the job.
				 */
				req->options2 |= SUB2_JOB_CMD_SPOOL;
				break;

			default:
			unimplemented_option:
				break;
		 }
	 }
#undef ASSIGN_ARG
#undef ASSIGN_INTARG
}


time_t
lsfdrmaa_parse_datetime( const char *datetime_string )
{
	%%{
		machine lsfdrmaa_datetime;
		action error {
			fsd_exc_raise_fmt(
					FSD_ERRNO_INVALID_VALUE_FORMAT,
					"date/time syntax error: %s", datetime_string );
		}
		action next_field {
			fields[ n_fields++ ] = v;
		}
		integer = [0-9]+ >{ v = 0; } ${ v *= 10;  v += fc - '0'; };
		datetime = integer %next_field (':' integer %next_field){1,3};
		main := datetime @eof(error) $err(error);
	}%%

	%%write data;
	fsd_datetime_t dt;
	int fields[4];
	int n_fields = 0;
	const char *i;
	int v = 0;
	time_t result;

	const char *p = datetime_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;

	fsd_log_enter(( "(%s)", datetime_string ));
	%%write init;
	%%write exec;

	fsd_assert( 2 <= n_fields  &&  n_fields <= 4 );
	dt.minute = fields[--n_fields];
	dt.hour = fields[--n_fields];
	dt.mask = FSD_DT_MINUTE | FSD_DT_HOUR;
	if( n_fields > 0 )
	 {
		dt.day = fields[--n_fields];
		dt.mask |= FSD_DT_DAY;
	 }
	if( n_fields > 0 )
	 {
		dt.month = fields[--n_fields];
		dt.mask |= FSD_DT_MONTH;
	 }
	fsd_assert( n_fields == 0 );

	result = fsd_datetime_after( &dt, time(NULL) );

	fsd_log_return(( " =%ld", (long)result ));
	return result;
}


int
lsfdrmaa_parse_timedelta( const char *timedelta_string )
{
%%{
	machine lsfdrmaa_timedelta;
	action error {
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_VALUE_FORMAT,
				"time delta syntax error: %s", timedelta_string );
	}
	integer = [0-9]+ >{ v = 0; } ${ v *= 10;  v += fc - '0'; };
	timedelta = (integer %{ hours = v; }) ':' (integer %{ minutes = v; });
	main := timedelta %{ timedelta = (60*hours + minutes)*60; }
				@eof(error) $err(error);
}%%

	%%write data;
	int v;
	int hours, minutes;
	int timedelta;
	const char *p = timedelta_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;

	fsd_log_enter(( "(%s)", timedelta_string ));
	%%write init;
	%%write exec;
	fsd_log_return(( " =%d", timedelta ));
	return timedelta;
}


void
lsfdrmaa_parse_asked_hosts(
		struct submit *req,
		const char *asked_hosts_string
		)
{
	%%{
		machine lsfdrmaa_asked_hosts;
		action add_host {
			fsd_log_debug(( "-m %.*s",(int)name_len, name ));
			fsd_realloc( req->askedHosts, req->numAskedHosts+1, char* );
			req->askedHosts[ req->numAskedHosts++ ] = NULL;
			req->askedHosts[ req->numAskedHosts-1 ]
				= fsd_strndup( name, name_len );
			req->options |= SUB_HOST; 
		}
		action error{
			fsd_exc_raise_fmt(
					FSD_ERRNO_INVALID_VALUE_FORMAT,
					"-m: asked hosts syntax error: %s",
					asked_hosts_string );
		}
		ws = [ \t];
		hostname = [^ \t]+ >{ name = fpc; } %{ name_len = fpc - name; };
		asked_hosts = ws* hostname %add_host
			(ws+ hostname %add_host)* ws*;
		main := asked_hosts @eof(error) $err(error);
	}%%

	%%write data;
	const char *name;
	size_t name_len;

	const char *p = asked_hosts_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;

	fsd_log_enter(( "(%s)", asked_hosts_string ));
	%%write init;
	%%write exec;
	fsd_log_return(( "" ));
}


void
lsfdrmaa_parse_file_transfer(
		struct xFile *transfer,
		const char *transfer_string )
{
	%%{
		machine lsfdrmaa_transfer_files;
		action error{
			fsd_exc_raise_fmt(
					FSD_ERRNO_INVALID_VALUE_FORMAT,
					"-f: file transfer syntax error: %s",
					transfer_string );
		}
		ws = [ \t];
		filename = [^ \t<>]+;
		operator = [<>]{1,2};
		transfer =
			ws* (filename >{ local_file = fpc; }
					%{ local_file_len = fpc - local_file; })
			ws* (operator >{ op = fpc; } %{ op_len = fpc - op; })
			ws* (filename >{ remote_file = fpc; }
					%{ remote_file_len = fpc - remote_file; })?
			ws*;
		main := transfer @eof(error) $err(error);
	}%%

	%%write data;
	const char *local_file = NULL;
	size_t local_file_len;
	const char *remote_file = NULL;
	size_t remote_file_len;
	const char *op = NULL;
	size_t op_len;
	unsigned i;

	const char *p = transfer_string;
	const char *pe = p + strlen(p);
	const char *eof = pe;
	int cs;

	fsd_log_enter(( "(%s)", transfer_string ));
	transfer->options = 0;
	if( sizeof(transfer->subFn) != MAXFILENAMELEN )
	 { /* LSF 7 */
		*(char**)&transfer->subFn = NULL;
		*(char**)&transfer->execFn = NULL;
	 }
	%%write init;
	%%write exec;

	if( op_len > 2 )
		fsd_exc_raise_fmt(
				FSD_ERRNO_INVALID_VALUE_FORMAT,
				"-f: invalid operator: %.*s", (int)op_len, op );
	for( i = 0;  i < op_len;  i++ )
		if( op[i] == '>' )
			transfer->options |= XF_OP_SUB2EXEC;
		else
			transfer->options |= XF_OP_EXEC2SUB;
	if( op_len==2  &&  op[0] == op[1] )
	 {
		if( op[0] == '>' )
			transfer->options |= XF_OP_SUB2EXEC_APPEND;
		else
			transfer->options |= XF_OP_EXEC2SUB_APPEND;
	 }

	if( remote_file == NULL )
	 {
		remote_file = local_file;
		remote_file_len = local_file_len;
	 }
	if( sizeof(transfer->subFn) == MAXFILENAMELEN )
	 { /* LSF 6 */
		strlcpy( transfer->subFn, local_file,
						min(local_file_len+1, MAXFILENAMELEN) );
		strlcpy( transfer->execFn, remote_file,
						min(remote_file_len+1, MAXFILENAMELEN) );
	 }
	else
	 { /* LSF 7 */
		*(char**)&transfer->subFn = fsd_strndup( local_file, local_file_len );
		*(char**)&transfer->execFn = fsd_strndup( remote_file, remote_file_len );
	 }
	fsd_log_return(( ": %.*s %.*s %.*s",
				(int)local_file_len, local_file, (int)op_len, op,
				(int)remote_file_len, remote_file ));
}


/* vim: set ft=ragel: */
