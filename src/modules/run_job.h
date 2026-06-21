#include "../structs/cmd_structs.h"
#include "../structs/program_structs.h"

#ifndef RUN_JOB_H
#define RUN_JOB_H

int run_job(Job job, char **cwd, BackgroundJobs *mgr);

#endif