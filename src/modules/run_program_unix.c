#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../structs/cmd_structs.h"
#include "../structs/program_structs.h"
#include "builtins.h"
#include "run_program.h"

Program run_program(char *program_path, char *args[], int input_fd, int output_fd, int target_fd, int is_bg) {
    pid_t pid;
    int status = 0;

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);

    if (input_fd != STDIN_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, input_fd, STDIN_FILENO);
    }

    if (output_fd != STDOUT_FILENO || target_fd != STDOUT_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, output_fd, target_fd);
    }

    if (posix_spawn(&pid, program_path, &actions, NULL, args, NULL) == 0) {
        if (!is_bg && input_fd == STDIN_FILENO && output_fd == STDOUT_FILENO) { 
            waitpid(pid, &status, 0);
            status = WEXITSTATUS(status);
        }
    } else {
        printf("Failed to spawn process\n");
        posix_spawn_file_actions_destroy(&actions);
        return (Program){-1, 1};
    }

    posix_spawn_file_actions_destroy(&actions);
    return (Program){pid, status};
}

void run_builtin(Command cmd, char **cwd, char *output_file, int fd, int append, int is_bg, BackgroundJobs *mgr) {
    int saved_fd = -1;

    if (output_file != NULL) {
        saved_fd = dup(fd);

        int flags = O_WRONLY | O_CREAT;
        if (append) flags |= O_APPEND;
        else flags |= O_TRUNC;

        int file_fd = open(output_file, flags, 0644);
        if (file_fd >= 0) {
            dup2(file_fd, fd);
            close(file_fd);
        }
    }

    if (strcmp(cmd.tokens[0], "echo") == 0) echo(cmd);
    else if (strcmp(cmd.tokens[0], "type") == 0) type(cmd);
    else if (strcmp(cmd.tokens[0], "pwd") == 0) pwd(*cwd);
    else if (strcmp(cmd.tokens[0], "cd") == 0) cd(cmd.tokens[1], cwd);
    else if (strcmp(cmd.tokens[0], "jobs") == 0) jobs(mgr);

    if (output_file != NULL && saved_fd >= 0) {
        dup2(saved_fd, fd);
        close(saved_fd);
    }
}