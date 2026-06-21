#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../structs/cmd_structs.h"
#include "../structs/program_structs.h"
#include "../utils/file_utils.h"
#include "../utils/bg_utils.h"
#include "run_program.h"
#include "builtins.h"
#include "run_job.h"

#include <unistd.h>

int run_cmd(Command cmd, char **cwd, char *output_file, int fd, int append, int is_pipe, int is_bg, BackgroundJobs *mgr) {
    if (cmd.num_tokens == 0 || cmd.tokens[0] == NULL || strlen(cmd.tokens[0]) == 0) return 0;

    else if (strcmp(cmd.tokens[0], "exit") == 0) return -2;
    
    for (int i = 1; i < valid_builtins_count; i++) {
        if (strcmp(cmd.tokens[0], valid_builtins[i]) == 0) {
            run_builtin(cmd, cwd, output_file, fd, append, is_bg);
            return 0;
        }
    }

    char *exe_path = find_executable(cmd.tokens[0]);
    if (exe_path != NULL) {
        Program program = run_program(exe_path, cmd.tokens, output_file, fd, append, is_bg);
        if (is_bg) add_background_job(mgr, program.pid, cmd);
        
        free(exe_path);
        return program.status;
    }

    else {
        printf("%s: command not found\n", cmd.tokens[0]);
        return 1;
    }
}

int run_job(Job job, char **cwd, BackgroundJobs *mgr) {
    if (mark_files_in_job(&job, *cwd)) return 1;

    int exit;

    char *link;
    char *output_file;
    int fd;
    int append;
    int is_pipe;
    int pipefds[2];
    int is_bg = 0;

    if (job.commands[job.num_commands - 1].link != NULL && strcmp(job.commands[job.num_commands - 1].link, "&") == 0) {
        is_bg = 1;
    }

    for (int i = 0; i < job.num_commands; i++) {
        output_file = NULL;
        link = job.commands[i].link;
        fd = 1;
        append = 0;
        is_pipe = 0;

        if (link != NULL && i < job.num_commands - 1) {
            char *op = link;

            if (link[0] >= '0' && link[0] <= '2') {
                fd = link[0] - '0';
                op++;
            }
            if (strcmp(op, ">") == 0) {
                append = 0;
                output_file = job.commands[i + 1].tokens[0];
            }
            else if (strcmp(op, ">>") == 0) {
                append = 1;
                output_file = job.commands[i + 1].tokens[0];
            }
        } else if (link != NULL && strcmp(link, "|") == 0) {
            is_pipe = 1;
            if (pipe(pipefds) < 0) return 1;
            
            // WIP
        }

        exit = run_cmd(job.commands[i], cwd, output_file, fd, append, is_pipe, is_bg, mgr);
        if (exit == -2) return -2;
        
        if (output_file != NULL) i++;

        if (link == NULL || strcmp(link, ";") == 0) continue;

        if (is_bg) {
            if (link == NULL || strcmp(link, "&") == 0) break;
            continue;
        }

        if (strcmp(link, "||") == 0 && exit == 0) break;
        if (strcmp(link, "&&") == 0 && exit != 0) break;
    }

    return 0;
}