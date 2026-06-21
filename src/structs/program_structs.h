#include <unistd.h>
#include "cmd_structs.h"

#ifndef PROGRAM_STRUCTS_H
#define PROGRAM_STRUCTS_H

typedef struct {
    int pid;
    int status;
} Program;

typedef struct {
    int job_no;
    int pid;
    char *status;
    Command command;
} BackgroundJob;

typedef struct {
    BackgroundJob *jobs;
    int count;
    int capacity;
    int total_launched;
} BackgroundJobs;

#endif