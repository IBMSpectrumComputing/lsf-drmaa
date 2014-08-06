/**************************************************************
* A sample for job submission, in this program a job "sleep 20"
* will submit to LSF cluster and wait until it finished.
**************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "drmaa.h"
#define MAX_LEN_JOBID 100


int main(int argc, char *argv[])
{
    char diagnosis[DRMAA_ERROR_STRING_BUFFER];
    int drmaa_errno = 0;
    char job_id[MAX_LEN_JOBID];
    const char *job_path = "sleep";
    const char * job_argv[2] = {"20", NULL};
    drmaa_job_template_t *jt = NULL;
    char *job_name    = NULL;
    const char* job_native = "-q normal";
    char job_id_out[MAX_LEN_JOBID];
    int stat;
    int exited, exit_status, signaled;

    memset(job_id, 0, sizeof(job_id));
    memset(job_id_out, 0, sizeof(job_id_out));
    if (drmaa_init(NULL, diagnosis, sizeof(diagnosis)-1) != DRMAA_ERRNO_SUCCESS) {
        fprintf(stderr, "drmaa_init() failed: %s\n", diagnosis);
        return 1;
    }

    if (drmaa_allocate_job_template(&jt, NULL, 0)!=DRMAA_ERRNO_SUCCESS) {
   	    fprintf(stderr, "drmaa_allocate_job_template() failed: %s\n", diagnosis);
        return 1;
    }

    if(job_name)
        drmaa_set_attribute(jt, DRMAA_JOB_NAME, job_name, NULL, 0);
  
    
    /* run in users home directory */
    drmaa_set_attribute(jt, DRMAA_WD, DRMAA_PLACEHOLDER_HD, NULL, 0);

    /* the job to be run */
    drmaa_set_attribute(jt, DRMAA_REMOTE_COMMAND, job_path, NULL, 0);

    /* the job's arguments */
    drmaa_set_vector_attribute(jt, DRMAA_V_ARGV, job_argv, NULL, 0);

    /* join output/error file */
    drmaa_set_attribute(jt, DRMAA_JOIN_FILES, "y", NULL, 0);  
   
    /* path for output */
    drmaa_set_attribute(jt, DRMAA_OUTPUT_PATH, ":"DRMAA_PLACEHOLDER_HD"/DRMAA_JOB", 
                          NULL, 0);
    
    /* the job's native specification */
   drmaa_set_attribute(jt, DRMAA_NATIVE_SPECIFICATION, job_native, NULL, 0);

    /*run a job*/
    drmaa_errno=drmaa_run_job(job_id, 
                              sizeof(job_id)-1, jt, diagnosis,
							  sizeof(diagnosis)-1);
    
    if (drmaa_errno != DRMAA_ERRNO_SUCCESS) 
	{
	    fprintf(stderr, "drmaa_run_job() failed: %s\n", diagnosis);
	    return 1;
    }

    drmaa_delete_job_template(jt, NULL, 0);

    printf("Waiting for job<%s> finish ...\n", job_id);

    /*wait job finished*/
    drmaa_errno = drmaa_wait(job_id, job_id_out, sizeof(job_id_out)-1, &stat, 
                           DRMAA_TIMEOUT_WAIT_FOREVER, NULL, diagnosis,
                           sizeof(diagnosis)-1);

    if (drmaa_errno != DRMAA_ERRNO_SUCCESS) {
        fprintf(stderr, "drmaa_wait(%s) failed: %s\n", job_id, diagnosis);
        return 1;
    }
    printf("job stat is %d\n", stat);

    drmaa_wifexited(&exited, stat, NULL, 0);

    if (exited) {
        drmaa_wexitstatus(&exit_status, stat, NULL, 0);
        printf("job<%s> finished with exit code %d\n", 
           job_id, exit_status);
    } 
	else 
    {
        drmaa_wifsignaled(&signaled, stat, NULL, 0);

        if (signaled) 
		{
            char termsig[DRMAA_SIGNAL_BUFFER+1];
            drmaa_wtermsig(termsig, DRMAA_SIGNAL_BUFFER, stat, NULL, 0);
            printf("job<%s> finished due to signal %s\n", 
                job_id, termsig);
        }else{
            printf("job<%s> is aborted\n", job_id);
        }
    }
	
    if (drmaa_exit(diagnosis, sizeof(diagnosis)-1) != DRMAA_ERRNO_SUCCESS) {
        fprintf(stderr, "drmaa_exit() failed: %s\n", diagnosis);
        return 1;
    }
   
    return 0;
}


