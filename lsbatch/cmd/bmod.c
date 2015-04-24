/*
 * Copyright (C) 2015 David Bigagli
 * Copyright (C) 2007 Platform Computing Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 */

#include "cmd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>


#define NL_SETN 8

extern void sub_perror (char *usrMsg);
extern int getJobIdList(char *, LS_LONG_INT **);

int
main (int argc, char **argv)
{
    struct submit req;
    struct submitReply  reply;
    char *job;
    LS_LONG_INT jobId = -1, *jobIdList = NULL;
    int numJobIds;
    time_t beginTime, terminTime;

    if (lsb_init(argv[0]) < 0) {
        sub_perror("lsb_init");
        fprintf(stderr, ". Job not modified.\n");
        exit (-1);
    }

    if (fillReq (argc, argv, CMD_BMODIFY, &req) < 0) {
        fprintf(stderr, ". Job not modified.\n");
        exit (-1);
    }

    job = req.command;
    beginTime = req.beginTime;
    terminTime = req.termTime;

    if ((numJobIds = getJobIdList(job, &jobIdList)) < 0) {
        exit(-1);
    }

    jobId = jobIdList[0];

    if ((jobId = lsb_modify(&req, &reply, jobId)) < 0) {
       if (lsberrno == LSBE_JOB_ARRAY) {
            fprintf(stderr, "Options -q and -O cannot be applied on job array");
       } else {
            prtErrMsg (&req, &reply);
       }
	fprintf(stderr, ". Job not modified.\n");
        if (req.nxf)
            free(req.xf);
        exit (-1);
    }

    printf("Parameters of job <%s> are being changed\n", lsb_jobid2str(jobId));
    if (beginTime > 0 || terminTime > 0)
        prtBETime_(&req);
    if (req.nxf)
        free(req.xf);

    return 0;
}
