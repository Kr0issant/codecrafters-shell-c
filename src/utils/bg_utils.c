#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../structs/program_structs.h"
#include "misc.h"
#include "bg_utils.h"

void add_background_job(BackgroundJobs *mgr, int pid, Command cmd) {
    if (mgr->count >= mgr->capacity) {
        mgr->capacity = mgr->capacity == 0 ? 4 : mgr->capacity * 2;
        BackgroundJob *temp = realloc(mgr->jobs, mgr->capacity * sizeof(BackgroundJob));
        if (temp == NULL) {
            perror("Realloc failed for background jobs");
            return;
        }
        mgr->jobs = temp;
    }
    
    int idx = mgr->count;
    mgr->jobs[idx].job_no = ++mgr->total_launched;
    mgr->jobs[idx].pid = pid;
    mgr->jobs[idx].status = strdup("Running");
    
    mgr->jobs[idx].command.num_tokens = cmd.num_tokens;
    mgr->jobs[idx].command.is_file = cmd.is_file;
    mgr->jobs[idx].command.status = cmd.status;
    mgr->jobs[idx].command.link = cmd.link ? strdup(cmd.link) : NULL;

    mgr->jobs[idx].command.tokens = malloc((cmd.num_tokens + 1) * sizeof(char*));
    for (int i = 0; i < cmd.num_tokens; i++) {
        if (cmd.tokens[i] != NULL) {
            mgr->jobs[idx].command.tokens[i] = strdup(cmd.tokens[i]);
        } else {
            mgr->jobs[idx].command.tokens[i] = NULL;
        }
    }
    mgr->jobs[idx].command.tokens[cmd.num_tokens] = NULL;

    printf("[%d]+ %d\n", mgr->jobs[idx].job_no, pid);
    mgr->count++;
}

void check_background_jobs(BackgroundJobs *mgr) {
    int status;
    pid_t pid;
    
    for (int i = 0; i < mgr->count; i++) {
        pid = waitpid(mgr->jobs[i].pid, &status, WNOHANG);
        if (pid > 0) {
            if (mgr->jobs[i].status) free(mgr->jobs[i].status);
            free_command(&(mgr->jobs[i].command));

            for (int j = i; j < mgr->count - 1; j++) {
                mgr->jobs[j] = mgr->jobs[j + 1];
            }
            mgr->count--;
            i--; 
        }
    }
}