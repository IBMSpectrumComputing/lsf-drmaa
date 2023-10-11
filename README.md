# lsf-drmaa

This is a fork of the original FedStage DRMAA for LSF created in 2008.  It currently has been certified to work on LSF upto version 9.1.2, but likely works with more recent versions.  Community contributions are allowed so long as you follow the IBM modified DCO process articulated below and in the IBMDCO.md file within this repository.  We welcome your pull requests.

## Release Information

* lsf-drmaa for LSF
* Supporting LSF Release: 9.1.2
* Version: 1.1.1
* Publication date: September 2013
* Last modified: 13 August 2014

## Contents

* Introduction
* Installation
* Configuration
* Native Specification
* Core Functionality
* Developer Tools
* DRMAA Release Notes
* Community Contribution
* Copyright

## Introduction

FedStage DRMAA for LSF is an implementation of Open Grid Forum DRMAA (Distributed Resource Management Application API) 1.0 specification 
for submission and control of jobs to Platform LSF.  Using DRMAA, grid applications builders, portal developers and ISVs can use the same
high-level API to link their software with different cluster/resource management systems.

This software also enables the integration of FedStage Computing with the underlying LSF system for remote multi-user job submission and control LSF over Web Services. 

This is an IBM Platform Computing fork of the original LSF 

DRMAA which can be found at http://sourceforge.net/projects/lsf-drmaa/.

IBM Spectrum Computing will provide formal support for this software to entitled clients via the normal IBM support channels.

## Installation

Before compiling the library, set LSF environment variables

To compile and install the library, go to the main source directory and type:

`$ ./configure [options] && make`

`$ sudo make install`

The library requires LSF version 8.3 or later.  Compatibility with older versions may require some patching.

* tips for running on RHEL8:
  Make sure libnsl and libtirpc installed, then do below link:
    - `sudo ln -s /lib64/libnsl.so.2.0.0    /lib64/libnsl.so`
    - `sudo ln -s /lib64/libtirpc.so.3.0.0  /lib64/libtirpc.so`
    - edit ./configure file as below before execute it.
       change line 12117, "ac_link='$CC -o conftest$ac_exeext $CFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&5'"
       to "ac_link='$CC -ltirpc -o conftest$ac_exeext $CFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&5'".
       There are multiple lines with same content, while line 12117 is the last line of same content in ./configure file.

* tips:
  If lsf_drmaa/native.rl has been modified to add some new functions, please run "sudo make clean" before running "./configure && make", but this will need ragel being installed on current host. 

The following are the ./configure script options:

* -with-lsf-inc LSF_INCLUDE_PATH
    Path to LSF header files (include dir). This option and with-lsf-lib option are unnecessary if the LSF_ENVDIR environment variable is set correctly (e.g., by $LSF_TOP/conf/profile.lsf).

* -with-lsf-lib LSF_LIBRARY_PATH
    Path to LSF libraries (lib dir).

* -with-lsf-static
    Links DRMAA against static LSF libraries instead of shared ones.

* -prefix INSTALLATION_DIRECTORY
    Root directory where FedStage DRMAA for LSF should be installed.  If not defined, the library is installed alongside LSF.	

* -enable-debug
    Compiles library with debugging enabled (with debugging symbols not stripped, without optimizations, and with many log messages enabled).  Useful when debugging DRMAA enabled applications or investigating problems with DRMAA library. 

The only requirements for basic usage of the library are an ANSI C compiler and a standard make program.  If you take sources directly 
from SVN repository or wish to run a test-suite, then additional development tools are required.  For further information regarding
the GNU build system, see the INSTALL file.

## Configuration

During DRMAA session initialization (drmaa_init) the library tries to read its configuration parameters from the following locations: 

  - /etc/lsf_drmaa.conf
  - ~/.lsf_drmaa.conf
  - A file defined by the LSF_DRMAA_CONF environment variable 
    (if set to non-empty string).  

If multiple configuration sources are present then all configurations are merged with values from user-defined files in the following order:

  1. $LSF_DRMAA_CONF
  2. ~/.lsf_drmaa.conf
  3. /etc/lsf_drmaa.conf`)

Currently recognized configuration parameters are:

  - pool_delay
    Amount of time (in seconds) between successive checks of queue(s).
    
    Type: integer. Default: 5.

  - cache_job_state
    According to DRMAA specification every drmaa_job_ps() call should query the DRM system for job state. With this option you can optimize communication with DRM. If set to a positive integer, drmaa_job_ps() returns remembered job states without communicating with DRM for cache_job_state seconds since the last update. By default, the library conforms to specification (no caching will be performed).

    Type: integer. Default: 0.

  - wait_thread
    If set to 0, every call to drmaa_wait() or drmaa_synchronize() pools DRM for selected/all jobs. By default, the library creates an additional thread which checks the state of all jobs for the duration of the DRMAA session. drmaa_wait()/drmaa_synchronize() calls are blocked until a finished job is found.

    Type: integer. Default: 1.

  - job_categories
    Dictionary of job categories. Its keys are job category names mapped to native specification strings. Attributes set by job category can be overridden by corresponding DRMAA attributes or native specification. Special category name "default" is used when drmaa_job_category job attribute is not set.

    Type: dictionary with string values. Default: empty dictionary

  - lsb_events_file
    The location of the lsb.events file. If set, the library polls the LSF events logfile instead of the LSF deamons.
    
    Type: path. Default: none.

### Configuration file syntax

The configuration file is in the form of a dictionary. The dictionary is a set of zero or more key-value pairs. Key is a string while value could be a string, an integer or another dictionary.

   * configuration: dictionary | dictionary_body
   * dictionary: '{' dictionary_body '}'
   * dictionary_body: (string ':' value ',')*
   * value: integer | string | dictionary
   * string: unquoted-string | single-quoted-string | double-quoted-string
   * unquoted-string: [^ \t\n\r:,0-9][^ \t\n\r:,]*
   * single-quoted-string: '[^']*'
   * double-quoted-string: "[^"]*"
   * integer: [0-9]+

## Native Specification

The DRMAA interface allows you to pass DRM dependent job submission options. Those options may be specified by settings in drmaa_native_specification or in the drmaa_job_category job attribute.  drmaa_native_specification accepts space delimited bsub options while drmaa_job_category is the name of the job category defined in the configuration file.  -a and bsub options which are meant for interactive job submission (-I, -Ip, -Is and -K) are not supported.

Attributes set in the native specification override corresponding DRMAA job attributes, which override those set by job category.

Native specification strings with corresponding DRMAA attributes:
  
| DRMAA attribute       |  Native specification   |
| --------------------- |  ---------------------- |
| drmaa_job_name        |  -J job name            |
| drmaa_input_path      |  -i input_path          |
| ..                    |  -is input_path         |
| drmaa_output_path     |  -o output path         |   
| ..                    |  -oo output_path        |   
| drmaa_error_path      |  -e error path          |     
| ..                    |  -eo error_path         |    
| drmaa_start_time      |  -b start time          |     
| drmaa_deadline_time   |  -t end_deadline        |   
| drmaa_js_state        |  -H                     |  
| drmaa_transfer_files  |  -f file_stage_op       | 
| drmaa_v_email         |  -u mail_user           |  
| ..                    |  -B, -N                 |   
| ..                    |  -m asked_hosts         |   
| ..                    |  -x                     |  
| ..                    |  -n min_proc[,max_proc] |
| ..                    |  -R res_req             |   
| drmaa_duration_hlimit | -c cpu_limit            | 
| drmaa_wct_hlimit      | -W runtime_limit        | 
| drmaa_wct_slimit      | -We estimated_runtime   |
| ..                    | -M memory_limit         |
| ..                    | -D data_limit           | 
| ..                    | -S stack_limit          | 
| ..                    | -v swap_limit           |  
| ..                    | -F file_limit           | 
| ..                    | -C core_limit           | 
| ..                    | -p process_limit        | 
| ..                    | -T thread_limit         | 
| ..                    | -ul                     | 
| ..                    | -U reservation_id       | 
| ..                    | -ar reservation_id      |
| ..                    | -wt warning_time        |  
| ..                    | -wa warning_action      |
| ..                    | -s signal               |  
| ..                    | -q queue_name           |   
| ..                    | -w dependency           | 
| ..                    | -sp priority            |  
| ..                    | -r, -rn                 |  
| ..                    | -G user_group           | 
| ..                    | -g job_group_name       | 
| ..                    | -P project_name         | 
| ..                    | -Lp ls_project_name     |
| ..                    | -E pre_exec_cmd         |  
| ..                    | -Ep post_exec_cmd       | 
| ..                    | -app app_profile        |
| ..                    | -ext sched_options      |
| ..                    | -jsdl jsdl_doc          | 
| ..                    | -jsdl_strict jsdl_doc   |
| ..                    | -k checkpoint_dir       |
| ..                    | -L login_shell          |   
| ..                    | -sla service_class_name |
| ..                    | -Z                      |     

## Core Functionality

Core functionality of DRMAA is put into the drmaa_utils library. 

As it is independent from any particular DRM, you may find this library useful for developing other DRMAAs. For detailed information please look at the source code documentation.

## Developer Tools

Although not needed for library users, the following tools may be required if you intend to develop FedStage DRMAA for LSF:

 * GNU autotools (autoconf, automake, libtool),
 * Bison parser generator (http://www.gnu.org/software/bison)
 * gperf perfect hash function generator (http://www.gnu.org/software/gperf)
 * Ragel finite state machine compiler (http://www.colm.net/open-source/ragel/)
 * Docutils for processing documentation (http://docutils.sourceforge.net)
 * LaTeX for creating documentation in PDF format (http://www.latex-project.org)
 * Doxygen for generating source code documentation (http://www.stack.nl/~dimitri/doxygen)

## DRMAA Release Notes

### Changes on 2014/08/13
 - Fix for LSF DRMAA lib to work with Galaxy
   
 The problem behavior is described at:
 http://osdir.com/ml/galaxy-development-source-control/2014-03/msg00052.html
   
 1) In Galaxy logic, drmaa.py invokes the drmaa_external_runner.py to submit the job and drmaa.py uses the stdout info that drmaa_external_runner.py prints as an lsf jobID. Invoking the LSF drmma api to submit a job, the DRMAA API prints the following message:

  "Job <78> is submitted to default queue <normal>"

  This means the drmaa.py will receive an incorrect lsf jobID.  If Galaxy must use this API, the drmaa_external_runner.py script must be modified to handle this message. The LSF environment variable "BSUB_QUIET" will disable this message.
      
  The following code change is a temporary fix for the drmaa_external_runner.py file: 

----------------------------------------------------------------------------
          diff -u drmaa_external_runner.orig.py drmaa_external_runner.py
      --- drmaa_external_runner.orig.py       2014-08-12 02:10:46.141585000 -0400
      +++ drmaa_external_runner.py    2014-08-12 02:10:20.007293000 -0400
      @@ -124,6 +124,7 @@
           userid, json_filename, assign_all_groups = validate_paramters()
           set_user(userid, assign_all_groups)
           json_file_exists(json_filename)
      +    os.environ['BSUB_QUIET'] = 'Y'
           s = drmaa.Session()
           s.initialize()
           jt = s.createJobTemplate()
---------------------------------------------------------------------------

2) In LSF, the default behaviour is that a user can query only his own jobs and cannot see other users' job information. However, the user who submits the job and queries job is not same in Galaxy. A normal job query operation will fail, making Galaxy think the job failed.  This fix updates the DRMAA lib, so that if querying a job with an effective jobID, the user can also query other users' job info.

### Changes in DRMAA release 1.1.1
 
* IBM Platform Computing refreshed LSF-DRMAA 1.0.4 and provides formal support.  Tested on LSF 9.1.2.

* Fixed the memory issue in DRMAA release 1.0.4.

### Changes in DRMAA release 1.0.4

* Fixed the core limit (-C) parsing in the native specification attribute.

* Fixed infinite loop on calling drmaa_wait/drmaa_synchronize routines after the CLEAN_PERIOD

### Changes in DRMAA release 1.0.3

* Fixed segfault when drmaa_v_env was set.  It now uses setenv and unsetenv calls to modify environ instead of substituting environ pointer.

* drmaa_transfer_files works.

* By default, when  prefix is not given at configure time, the library is installed alongside LSF.

* When waiting for any job with waiting thread enabled, the status of all jobs is pooled from DRM in one LSF API call.

* There is a new configuration option: cache_job_state.

* Error messages contain more detail.

* DRMAA now compiles against LSF version 6.0 or later, although it was not tested at runtime.

### Changes in DRMAA release 1.0.2
 
* drmaa_remote_command and drmaa_v_argv are quoted and not interpreted by the shell (e.g., spaces are allowed in commands and arguments).  Jobs are created with the exec command (i.e., unnecessary shell process dangling for duration of job was eliminated).

* drmaa_wifexited follows refinement on DRMAA Working Group mailing list; it returns 1 only for exit statuses not greater than 128.  Previously it returned 1 for all jobs which were run (not aborted).

* It has been reported that in some situations, a job which was recently submitted is not always immediately visible through the LSF API.  There is now a workaround for such behaviour.

* drmaa_transfer_files is ignored because it produces segfaults.

* Bugfixes: Segfault occurs when drmaa_v_argv is not set.  Native specification is used for parsing bugs.  Various other segfaults and memory leaks are fixed. 


### Changes in DRMAA release 1.0.1
 
* Version 1.0.1 of the library was previously released with 2.0 version number. This was misleading and did not reflect the version of DRMAA specification implemented by the library.

* Many attributes were implemented, including:
   - drmaa_start_time
   - drmaa_native_specification
   - drmaa_transfer_files
   - job limits

* Integrates with FedStage Advance Reservation Library for LSF.

* Job category now points to native specification string in configuration file instead of job group.

* Thread safe design.

* Includes configuration files.

* Many bug fixes.

* More robust code.

* Meaningful logging, error messages and codes.

## Known bugs and limitations

The library covers the entire DRMAA 1.0 specification with exceptions listed below.  It was successfully tested with IBM Platform LSF 9.1.2 on Linux OS and passed the official DRMAA test-suite.  All mandatory and nearly all optional job attributes (except job run duration soft limit) are implemented.

### Known limitations:

* $drmaa_incr_ph$ is replaced only within input, output and error file paths. While adhering to specification, $drmaa_incr_ph$ should also be substituted in the job working directory.

* Host name is ignored in input, output and error paths.  They are always copied to and from the submission host.

* Input file is copied from the submission host when it is not present on the execution host, even when "i" was not in transfer files attribute.

* drmaa_wcoredump() always returns false.

## Community Contribution Requirements

Community contributions to this repository must follow the [IBM Developer's Certificate of Origin (DCO)](https://github.com/IBMSpectrumComputing/lsf-drmaa/blob/master/LSF-Base/IBMDCO.md) process and only through GitHub Pull Requests:

 1. Contributor proposes new code to community.

 2. Contributor signs off on contributions 
    (i.e. attachs the DCO to ensure contributor is either the code 
    originator or has rights to publish. The template of the DCO is included in
    this package).
 
 3. IBM Spectrum LSF development reviews contribution to check for:
    i)  Applicability and relevancy of functional content 
    ii) Any obvious issues

 4. If accepted, posts contribution. If rejected, work goes back to contributor and is not merged.

## Copyrights

(C) Copyright IBM Corporation 2013-2017

U.S. Government Users Restricted Rights - Use, duplication or disclosure 
restricted by GSA ADP Schedule Contract with IBM Corp.

IBM(R), the IBM logo and ibm.com(R) are trademarks of International Business Machines Corp., 
registered in many jurisdictions worldwide. Other product and service names might be trademarks 
of IBM or other companies. A current list of IBM trademarks is available on the Web at 
"Copyright and trademark information" at www.ibm.com/legal/copytrade.shtml.

(C) Copyright 2007-2008  FedStage Systems
