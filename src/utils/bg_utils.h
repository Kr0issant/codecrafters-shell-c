#include "../structs/program_structs.h"

#ifndef BG_UTILS_H
#define BG_UTILS_H

void add_background_job(BackgroundJobs *mgr, int pid, Command cmd);
void check_background_jobs(BackgroundJobs *mgr);

#endif