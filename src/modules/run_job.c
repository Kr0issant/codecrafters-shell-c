#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../structs/cmd_structs.h"
#include "../structs/program_structs.h"
#include "../utils/file_utils.h"
#include "../utils/bg_utils.h"
#include "run_program.h"
#include "builtins.h"
#include "run_job.h"

#include <unistd.h>

int run_cmd(Command cmd, char **cwd, int input_fd, int output_fd, int is_bg, BackgroundJobs *mgr, pid_t *spawned_pid) {
    if (cmd.num_tokens == 0 || cmd.tokens[0] == NULL || strlen(cmd.tokens[0]) == 0) return 0;

    else if (strcmp(cmd.tokens[0], "exit") == 0) return -2;

    int is_builtin = 0;
    for (int i = 0; i < valid_builtins_count; i++) {
        if (strcmp(cmd.tokens[0], valid_builtins[i]) == 0) { is_builtin = 1; break; }
    }
        
    if (is_builtin) {
        run_builtin(cmd, cwd, NULL, output_fd, 0, is_bg, mgr);
        return 0;
    }

    char *exe_path = find_executable(cmd.tokens[0]);
    if (exe_path != NULL) {
        Program program = run_program(exe_path, cmd.tokens, NULL, input_fd, output_fd, 0, is_bg);
        if (is_bg && program.pid > 0) add_background_job(mgr, program.pid, cmd);

        if (spawned_pid != NULL) *spawned_pid = program.pid;
        
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

    int exit_status = 0;
    int is_bg = 0;

    if (job.num_commands > 0) {
        char *last_link = job.commands[job.num_commands - 1].link;
        if (last_link != NULL && strcmp(last_link, "&") == 0) is_bg = 1;
    }

    int in_fd = STDIN_FILENO;
    int pipefds[2];

    pid_t *pids = malloc(job.num_commands * sizeof(pid_t));
    int spawned_count = 0;

    for (int i = 0; i < job.num_commands; i++) {
        Command cmd = job.commands[i];
        char *link = cmd.link;
        int is_pipe = (link != NULL && strcmp(link, "|") == 0);
        int has_file_redirect = 0;
        int out_fd = STDOUT_FILENO;

        if (is_pipe) {
            if (pipe(pipefds) < 0) {
                perror("pipe creation failed");
                free(pids);
                return 1;
            }
            out_fd = pipefds[1];
        }

        if (link != NULL && (strstr(link, ">") != NULL)) {
            char *op = link;
            int target_sys_fd = STDOUT_FILENO;

            if (link[0] >= '0' && link[0] <= '2') {
                target_sys_fd = link[0] - '0';
                op++;
            }

            int flags = O_WRONLY | O_CREAT;
            if (strcmp(op, ">>") == 0) flags |= O_APPEND;
            else flags |= O_TRUNC;

            char *output_file = job.commands[i + 1].tokens[0];
            int file_fd = open(output_file, flags, 0644);
            
            if (file_fd >= 0) {
                out_fd = file_fd;
                has_file_redirect = 1;
            } else {
                perror("Failed to open redirection file");
            }
        }

        int is_builtin = 0;
        for (int j = 0; j < valid_builtins_count; j++) {
            if (strcmp(cmd.tokens[0], valid_builtins[j]) == 0) { is_builtin = 1; break; }
        }

        if (is_builtin && (is_pipe || in_fd != STDIN_FILENO)) {
            pid_t builtin_pid = fork();
            if (builtin_pid == 0) {
                if (in_fd != STDIN_FILENO) {
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                }
                if (out_fd != STDOUT_FILENO) {
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                }
                if (is_pipe) close(pipefds[0]);
                
                int rc = run_cmd(cmd, cwd, STDIN_FILENO, STDOUT_FILENO, is_bg, mgr, NULL);
                exit(rc < 0 ? 0 : rc);
            } else if (builtin_pid > 0) {
                pids[spawned_count++] = builtin_pid;
            }
        } else {
            pid_t ext_pid = 0;
            exit_status = run_cmd(cmd, cwd, in_fd, out_fd, is_bg, mgr, &ext_pid);
            if (ext_pid > 0) pids[spawned_count++] = ext_pid;
            if (exit_status == -2) { free(pids); return -2; }
        }

        if (in_fd != STDIN_FILENO) close(in_fd);

        if (has_file_redirect) {
            close(out_fd);
            if (is_pipe) {
                close(pipefds[1]);
                in_fd = pipefds[0];
            } else in_fd = STDIN_FILENO;
            i++;
        } else if (is_pipe) {
            close(pipefds[1]);
            in_fd = pipefds[0];
        } else in_fd = STDIN_FILENO;

        if (link == NULL || strcmp(link, ";") == 0) continue;
        if (is_bg && strcmp(link, "&") == 0) break;
        if (strcmp(link, "||") == 0 && exit_status == 0) break;
        if (strcmp(link, "&&") == 0 && exit_status != 0) break;

    }

    if (!is_bg) {
        for (int i = 0; i < spawned_count; i++) {
            int status;
            waitpid(pids[i], &status, 0);
            if (i == spawned_count - 1) { exit_status = WEXITSTATUS(status); }
        }
    }

    free(pids);
    return exit_status;
}